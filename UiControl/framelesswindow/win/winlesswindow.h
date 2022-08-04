#ifndef WINLESSWINDOW_H
#define WINLESSWINDOW_H

#include <QObject>
#include <QWidget>
#include <QMargins>
#include <QPoint>
#include <QDialog>
#include <QPainter>
#include <QLayout>
#include <QEvent>
#include <QAbstractNativeEventFilter>
#include <windows.h>

/// 无边框窗口边框颜色
#define FL_WIDGET_BODER_COLOR "gray"

class WinLessWindow: public QObject
{
    Q_OBJECT
public:
    WinLessWindow();
    void setWidget(QWidget *widget);

    void setMax(bool bMax) { m_bMax = bMax; }
    bool isMax() { return m_bMax; }

    void resetWidget();

    //设置是否可以通过鼠标调整窗口大小
    void setResizeEnable(bool resizeable=true);
    bool isResizeable(){return m_bResizeable;}

    //设置是否可以通过鼠标移动窗口
    void setMoveEnable(bool moveEnable) { m_moveEnable = moveEnable; }
    bool isMoveEnable(){ return m_moveEnable; }

    //点击标题栏是否最大化
    void setDbClickTitlebarMax(bool bDbClickTitleBarMax = true){ m_bDbClickTitleBarMax = bDbClickTitleBarMax; }
    bool isDbClickTitlebarMax() { return m_bDbClickTitleBarMax; }

    //设置一个标题栏widget，此widget会被当做标题栏对待
    void setTitleBar(QWidget* titlebar);

    //设置可调整大小区域的宽度，在此区域内，可以使用鼠标调整窗口大小
    void setResizeableAreaWidth(int width = 5);

    //设置一个浮动栏widget，此widget会被当做浮动工具栏对待
    void settoolBox(QWidget* toolbox);

    // 设置是否画边框
    void setDrawBorder(bool bDrawBorder);
signals:
    //bMax: true（最大化） false(最小化)
    void titleDblClick(bool bMax = true);
public:
    //在标题栏控件内，也可以有子控件如标签控件“label1”，此label1遮盖了标题栏，导致不能通过label1拖动窗口
    //要解决此问题，使用addIgnoreWidget(label1)
    void addIgnoreWidget(QWidget* widget);

    bool NativeEvent(const QByteArray &eventType, void *message, long *result);

    //双击标题栏
    bool dbClickTitleLab(MSG* msg);
private slots:
    void onTitleBarDestroyed();
public:
    void setContentsMargins(const QMargins &margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    QMargins contentsMargins() const;
    QRect contentsRect() const;
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
public slots:
    void showFullScreen();
private:
    QWidget *m_widget;
    QWidget* m_titlebar;
    QList<QWidget*> m_whiteList;
    int m_borderWidth;
    int m_borderWidthTmp;

    QMargins m_margins;
    QMargins m_frames;
    bool m_bJustMaximized;
    bool m_moveEnable; //设置窗口是否可以移动
    bool m_bResizeable; //设置窗口是否可以拖动大小
    bool m_bDbClickTitleBarMax; //点击标题栏是否最大化
    QWidget* m_toolbox;
    QPoint m_pos;
    bool m_bMax;

    bool m_bBorder;
};

class WinLessBase
{
//    Q_OBJECT
public:
    WinLessBase(QWidget *widget) {
        m_bBorder = false;
        m_lessWin.setWidget(widget);
        m_lessWin.setDrawBorder(m_bBorder);
        m_widget = widget;
    }

    static void drawWidgetBorder(QWidget *widget) {
        /// 画边框
        QRect rt = widget->rect();
        rt.setWidth(widget->width() - 1);
        rt.setHeight(widget->height() - 2);
        QPainter painter(widget);
        painter.setPen(FL_WIDGET_BODER_COLOR);
        painter.drawRect(rt);
    }

    void resetWidget() { m_lessWin.resetWidget(); }

    void setMax(bool bMax) { m_lessWin.setMax(bMax); }
    bool isMax() { return m_lessWin.isMax(); }
    //设置是否可以通过鼠标调整窗口大小
    void setResizeEnable(bool resizeable) { m_lessWin.setResizeEnable(resizeable); };
    bool isResizeable(){ return m_lessWin.isResizeable(); }
    //设置是否可以通过鼠标移动窗口
    void setMoveEnable(bool moveEnable) { m_lessWin.setMoveEnable(moveEnable); }
    bool isMoveEnable(){ return m_lessWin.isMoveEnable(); }
    //点击标题栏是否最大化
    void setDbClickTitlebarMax(bool bClickFullScreen){ m_lessWin.setDbClickTitlebarMax(bClickFullScreen); }
    bool isDbClickTitlebarMax() { return m_lessWin.isDbClickTitlebarMax(); }
    //设置一个标题栏widget，此widget会被当做标题栏对待
    void setTitleBar(QWidget* titlebar) { m_lessWin.setTitleBar(titlebar); }
    //设置可调整大小区域的宽度，在此区域内，可以使用鼠标调整窗口大小
    void setResizeableAreaWidth(int width = 5) { m_lessWin.setResizeableAreaWidth(width); }
    //设置一个浮动栏widget，此widget会被当做浮动工具栏对待
    void settoolBox(QWidget* toolbox) { m_lessWin.settoolBox(toolbox); }
    //在标题栏控件内，也可以有子控件如标签控件“label1”，此label1遮盖了标题栏，导致不能通过label1拖动窗口
    //要解决此问题，使用addIgnoreWidget(label1)
    void addIgnoreWidget(QWidget* widget) { m_lessWin.addIgnoreWidget(widget); }
    // windows无效
    void setDrawBorder(bool bBorder) {
        m_lessWin.setDrawBorder(bBorder);
        m_bBorder = bBorder;
        // 设置边距，画边框
        if(bBorder) {
            int cMg = 1;
            QLayout *layout = m_widget->layout();
            if(layout) {
                layout->setContentsMargins(cMg, cMg, cMg, cMg + 1);
            } else {
                m_widget->setContentsMargins(cMg, cMg, cMg, cMg);
            }
        }
    }

    bool canDrawBorder() {
        if(!m_bBorder || isMax()/* || m_widget->windowState() == Qt::WindowFullScreen*/)
            return false;
        else
            return true;
    }
protected:
    bool NativeEvent(const QByteArray &eventType, void *message, long *result) {
        return m_lessWin.NativeEvent(eventType, message, result);
    }
    WinLessWindow m_lessWin;
    bool m_bBorder; // 是否设置边框（默认不设置）
    QWidget *m_widget;
};

class WidgetLessWindow: public QWidget, public WinLessBase
{
    Q_OBJECT
public:
    WidgetLessWindow(QWidget *parent = nullptr):
         QWidget(parent), WinLessBase(this) {
         connect(&m_lessWin, SIGNAL(titleDblClick(bool)), this, SIGNAL(titleDblClick(bool)));
    }
signals:
    //bMax: true（最大化） false(最小化)
    void titleDblClick(bool bMax = true);
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override {
        //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
        #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
        MSG* msg = *reinterpret_cast<MSG**>(message);
        #else
        MSG* msg = reinterpret_cast<MSG*>(message);
        #endif

        switch (msg->message)
        {
        case WM_NCACTIVATE:
        case WM_NCCALCSIZE:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCLBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
        case WM_NCHITTEST:
        case WM_GETMINMAXINFO:
        case WM_NCLBUTTONUP:
        case WM_LBUTTONUP:{
            return NativeEvent(eventType, message, result);
        }
        default:
            return QWidget::nativeEvent(eventType, message, result);
        }
    }

    void paintEvent(QPaintEvent *event) override {
        if(!canDrawBorder()) {
            // 不画边框
            return QWidget::paintEvent(event);
        }

        /// 画边框
        drawWidgetBorder(this);
        return QWidget::paintEvent(event);
    }
};

class DialogLessWindow: public QDialog, public WinLessBase
{
    Q_OBJECT
public:
    DialogLessWindow(QWidget *parent = nullptr):
         QDialog(parent), WinLessBase(this) {
        m_bActive = true;
        connect(&m_lessWin, SIGNAL(titleDblClick(bool)), this, SIGNAL(titleDblClick(bool)));
    }
signals:
    //bMax: true（最大化） false(最小化)
    void titleDblClick(bool bMax = true);
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override {
        //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
        #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
        MSG* msg = *reinterpret_cast<MSG**>(message);
        #else
        MSG* msg = reinterpret_cast<MSG*>(message);
        #endif

        switch (msg->message)
        {
        case WM_NCACTIVATE: {
//            if(m_widget && m_widget->windowType() == Qt::Dialog && m_widget->parent()) {
//                m_bActive = msg->wParam; // FALSE 模态窗口失去焦点
//                update();
//            }
        }
        case WM_NCCALCSIZE:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCLBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
        case WM_NCHITTEST:
        case WM_GETMINMAXINFO:
        case WM_NCLBUTTONUP:
        case WM_LBUTTONUP:{
            return NativeEvent(eventType, message, result);
        }
        default:
            return QDialog::nativeEvent(eventType, message, result);
        }
    }

    void paintEvent(QPaintEvent *event) override {
#if 0
        if(!m_bActive) {
//            QRect rt = rect();
//            rt.setY(1);
//            rt.setWidth(width() - 1);
//            rt.setHeight(height() - 2);
//            QPainter painter(this);
//            painter.setPen("red");
//            painter.drawRect(rt);

            PFLASHWINFO fInfo = new FLASHWINFO();
            fInfo->cbSize = sizeof (FLASHWINFO);// Convert.ToUInt32(Marshal.SizeOf(fInfo));
            fInfo->hwnd = (HWND)winId();//要闪烁的窗口的句柄，该窗口可以是打开的或最小化的
            fInfo->dwFlags = (uint)FLASHW_ALL;//闪烁的类型
            fInfo->uCount = 3;//闪烁窗口的次数
            fInfo->dwTimeout = 0; //窗口闪烁的频度，毫秒为单位；若该值为0，则为默认图标的闪烁频度
            FlashWindowEx(fInfo);

            // FlashWindow((HWND)winId(), true);
        }
#endif

        if(!canDrawBorder()) {
            // 不画边框
            return QDialog::paintEvent(event);
        }

        /// 画边框
        drawWidgetBorder(this);
        return QDialog::paintEvent(event);
    }

private:
    bool m_bActive;
};

#endif // WINLESSWINDOW_H
