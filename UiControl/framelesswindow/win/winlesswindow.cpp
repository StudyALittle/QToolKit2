#include "winlesswindow.h"

#include <QApplication>
#include <WinUser.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
#include <gdiplus.h>
#include <GdiPlusColor.h>
#pragma comment (lib,"Dwmapi.lib") // Adds missing library, fixes error LNK2019: unresolved external symbol __imp__DwmExtendFrameIntoClientArea
#pragma comment (lib,"user32.lib")

WinLessWindow::WinLessWindow()
{
    m_bBorder = false;
}
void WinLessWindow::setWidget(QWidget *widget)
{
    static bool s_initNative = false;
    if(!s_initNative)  {
        // 防止窗口 Native化
        QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QApplication::setAttribute(Qt::AA_NativeWindows,false);
        s_initNative = true;
    }

    widget->setAttribute(Qt::WA_DontCreateNativeAncestors);
    m_widget = widget;
    m_bJustMaximized = true;
    m_bDbClickTitleBarMax = true;
    m_bResizeable = true;
    m_moveEnable = true;
    m_borderWidth = 4;
    m_borderWidthTmp = m_borderWidth;
    m_titlebar = nullptr;
    m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setResizeEnable(m_bResizeable);
}

void WinLessWindow::resetWidget() {
    setResizeEnable(m_bResizeable);
}

void WinLessWindow::setResizeEnable(bool resizeable)
{
    bool visible = m_widget->isVisible();
    m_bResizeable = resizeable;
    if (m_bResizeable){
        m_borderWidth = m_borderWidthTmp;
        //
        m_widget->setWindowFlags(m_widget->windowFlags() | Qt::WindowMaximizeButtonHint);
        //此行代码可以带回Aero效果，同时也带回了标题栏和边框,在nativeEvent()会再次去掉标题栏
        HWND hwnd = (HWND)m_widget->winId();
        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
    }else{
        m_borderWidthTmp = m_borderWidth;
        m_borderWidth = 0;
//          这样设置界面阴影会消失掉
//        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
//        HWND hwnd = (HWND)this->winId();
//        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
//        ::SetWindowLong(hwnd, GWL_STYLE, style & ~WS_MAXIMIZEBOX & ~WS_CAPTION);
    }

    //保留一个像素的边框宽度，否则系统不会绘制边框阴影
    //we better left 1 piexl width of border untouch, so OS can draw nice shadow around it
    if(!m_bBorder) { // 不画边框时，绘制系统阴影
        const MARGINS shadow = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(HWND(m_widget->winId()), &shadow);
    }

    m_widget->setVisible(visible);
}

// 设置是否画边框
void WinLessWindow::setDrawBorder(bool bDrawBorder)
{
    m_bBorder = bDrawBorder;
    if(!m_bBorder) { // 不画边框时，绘制系统阴影
        const MARGINS shadow = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(HWND(m_widget->winId()), &shadow);
    } else {
        const MARGINS shadow = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(HWND(m_widget->winId()), &shadow);
    }
}

void WinLessWindow::setResizeableAreaWidth(int width)
{
    if (1 > width) width = 1;
    if(m_bResizeable){
        m_borderWidth = width;
    }else{
        m_borderWidthTmp = m_borderWidth;
    }
}

void WinLessWindow::settoolBox(QWidget *toolbox)
{
    m_toolbox = toolbox;
    if (!toolbox) return;
}

void WinLessWindow::setTitleBar(QWidget* titlebar)
{
    m_titlebar = titlebar;
    if (!titlebar) return;
    connect(titlebar, SIGNAL(destroyed(QObject*)), this, SLOT(onTitleBarDestroyed()));
}

void WinLessWindow::onTitleBarDestroyed()
{
    if (m_titlebar == QObject::sender()) {
        m_titlebar = Q_NULLPTR;
    }
}

void WinLessWindow::addIgnoreWidget(QWidget* widget)
{
    if (!widget) return;
    if (m_whiteList.contains(widget)) return;
    m_whiteList.append(widget);
}

bool WinLessWindow::NativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType)

    //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
    #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
    MSG* msg = *reinterpret_cast<MSG**>(message);
    #else
    MSG* msg = reinterpret_cast<MSG*>(message);
    #endif

    switch (msg->message)
    {
    case WM_NCACTIVATE:
    {
        if(msg->wParam  ==  FALSE)
        {
            *result = TRUE;
        }
        return true;
    }
    case WM_NCCALCSIZE:
    {
        NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
        if (params.rgrc[0].top != 0)
            params.rgrc[0].top -= 1;

        //this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
        *result = WVR_REDRAW;
        return true;
    }
    case WM_NCRBUTTONDBLCLK: {
        return false;
    }
    case WM_NCLBUTTONDBLCLK:{
        return dbClickTitleLab(msg);
    }
    case WM_LBUTTONDBLCLK:{
        return dbClickTitleLab(msg);
    }
    case WM_NCHITTEST:
    {
        *result = 0;

        const LONG border_width = m_borderWidth;
        RECT winrect;
        GetWindowRect(HWND(m_widget->winId()), &winrect);

        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);

        if(m_bResizeable && border_width > 0)
        {

            bool resizeWidth = m_widget->minimumWidth() != m_widget->maximumWidth();
            bool resizeHeight = m_widget->minimumHeight() != m_widget->maximumHeight();

            if(resizeWidth)
            {
                //left border
                if (x >= winrect.left && x < winrect.left + border_width)
                {
                    *result = HTLEFT;
                }
                //right border
                if (x < winrect.right && x >= winrect.right - border_width)
                {
                    *result = HTRIGHT;
                }
            }
            if(resizeHeight)
            {
                //bottom border
                if (y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOM;
                }
                //top border
                if (y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOP;
                }
            }
            if(resizeWidth && resizeHeight)
            {
                //bottom left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                        y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOMLEFT;
                }
                //bottom right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                        y < winrect.bottom && y >= winrect.bottom - border_width)
                {
                    *result = HTBOTTOMRIGHT;
                }
                //top left corner
                if (x >= winrect.left && x < winrect.left + border_width &&
                        y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOPLEFT;
                }
                //top right corner
                if (x < winrect.right && x >= winrect.right - border_width &&
                        y >= winrect.top && y < winrect.top + border_width)
                {
                    *result = HTTOPRIGHT;
                }
            }
        }
        if (0!=*result) return true;

        //*result still equals 0, that means the cursor locate OUTSIDE the frame area
        //but it may locate in titlebar area
        if (!m_titlebar || !m_moveEnable) return false;

        //support highdpi
        double dpr = m_widget->devicePixelRatioF();
        QPoint pos = m_titlebar->mapFromGlobal(QPoint(x/dpr,y/dpr));

        if (!m_titlebar->rect().contains(pos)) return false;
        QWidget* child = m_titlebar->childAt(pos);
        if (!child)
        {
            *result = HTCAPTION;
            return true;
        }else{
            if (m_whiteList.contains(child))
            {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    } //end case WM_NCHITTEST
    case WM_GETMINMAXINFO:
    {
        if (::IsZoomed(msg->hwnd)) {
            RECT frame = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);

            //record frame area data
            double dpr = m_widget->devicePixelRatioF();

            m_frames.setLeft(abs(frame.left)/dpr+0.5);
            m_frames.setTop(abs(frame.bottom)/dpr+0.5);
            m_frames.setRight(abs(frame.right)/dpr+0.5);
            m_frames.setBottom(abs(frame.bottom)/dpr+0.5);

            m_widget->setContentsMargins(m_frames.left()+m_margins.left(), \
                                            m_frames.top()+m_margins.top(), \
                                            m_frames.right()+m_margins.right(), \
                                            m_frames.bottom()+m_margins.bottom());
            m_bJustMaximized = true;
        }else {
            if (m_bJustMaximized)
            {
                m_widget->setContentsMargins(m_margins);
                m_frames = QMargins();
                m_bJustMaximized = false;
            }
        }
        return false;
    }
    case WM_NCLBUTTONUP:{
        return false;
    }
    case WM_LBUTTONUP:{
        return false;
    }
    default:
        return false;
        //return nativeEvent(eventType, message, result);
    }

}

//双击标题栏
bool WinLessWindow::dbClickTitleLab(MSG* msg)
{
    if(m_bDbClickTitleBarMax && m_titlebar) { //双击最大化/正常
        if (m_widget->isMaximized()) {
            if(!m_moveEnable){ // m_moveEnable = false时，最大化功能失效; true window 由系统实现最大化、最小化
                //support highdpi
                long x = GET_X_LPARAM(msg->lParam);
                long y = GET_Y_LPARAM(msg->lParam);
                if (m_titlebar->rect().contains(x, y)){
                    m_widget->showNormal();
                }
            }
            emit titleDblClick(false);
        } else {
            if(!m_moveEnable){ // m_moveEnable = false时，全屏功能失效; true window 由系统实现最大化、最小化
                //support highdpi
                long x = GET_X_LPARAM(msg->lParam);
                long y = GET_Y_LPARAM(msg->lParam);
                if (m_titlebar->rect().contains(x, y)){
                    m_widget->showMaximized();
                }
            }
            emit titleDblClick(true);
        }
        return false;
    }
    return true; //双击不全屏
}

void WinLessWindow::setContentsMargins(const QMargins &margins)
{
    m_widget->setContentsMargins(margins+m_frames);
    m_margins = margins;
}
void WinLessWindow::setContentsMargins(int left, int top, int right, int bottom)
{
    m_widget->setContentsMargins(left+m_frames.left(),\
                                    top+m_frames.top(), \
                                    right+m_frames.right(), \
                                    bottom+m_frames.bottom());
    m_margins.setLeft(left);
    m_margins.setTop(top);
    m_margins.setRight(right);
    m_margins.setBottom(bottom);
}
QMargins WinLessWindow::contentsMargins() const
{
    QMargins margins = m_widget->contentsMargins();
    margins -= m_frames;
    return margins;
}
void WinLessWindow::getContentsMargins(int *left, int *top, int *right, int *bottom) const
{
    m_widget->getContentsMargins(left,top,right,bottom);
    if (!(left&&top&&right&&bottom)) return;
    if (m_widget->isMaximized())
    {
        *left -= m_frames.left();
        *top -= m_frames.top();
        *right -= m_frames.right();
        *bottom -= m_frames.bottom();
    }
}
QRect WinLessWindow::contentsRect() const
{
    QRect rect = m_widget->contentsRect();
    int width = rect.width();
    int height = rect.height();
    rect.setLeft(rect.left() - m_frames.left());
    rect.setTop(rect.top() - m_frames.top());
    rect.setWidth(width);
    rect.setHeight(height);
    return rect;
}
void WinLessWindow::showFullScreen()
{
    if (m_widget->isMaximized()) {
        m_widget->setContentsMargins(m_margins);
        m_frames = QMargins();
    }
    m_widget->showFullScreen();
}
