#ifndef WINLESSWINDOW_H
#define WINLESSWINDOW_H

#include <QObject>
#include <QWidget>
#include <QMargins>
#include <QPoint>
#include <QDialog>
#include <windows.h>

class WinLessWindow: public QObject
{
    Q_OBJECT
public:
    WinLessWindow();
    void setWidget(QWidget *widget);

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
};

class WinLessBase
{
//    Q_OBJECT
public:
    WinLessBase(QWidget *widget) {
        m_lessWin.setWidget(widget);
    }

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
    void setDrawBorder(bool bBorder) { Q_UNUSED(bBorder) }
protected:
    bool NativeEvent(const QByteArray &eventType, void *message, long *result) {
        return m_lessWin.NativeEvent(eventType, message, result);
    }
    WinLessWindow m_lessWin;
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
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) {
        //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
        #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
        MSG* msg = *reinterpret_cast<MSG**>(message);
        #else
        MSG* msg = reinterpret_cast<MSG*>(message);
        #endif

        switch (msg->message)
        {
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
};

class DialogLessWindow: public QDialog, public WinLessBase
{
    Q_OBJECT
public:
    DialogLessWindow(QWidget *parent = nullptr):
         QDialog(parent), WinLessBase(this) {
        connect(&m_lessWin, SIGNAL(titleDblClick(bool)), this, SIGNAL(titleDblClick(bool)));
    }
signals:
    //bMax: true（最大化） false(最小化)
    void titleDblClick(bool bMax = true);
protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) {
        //Workaround for known bug -> check Qt forum : https://forum.qt.io/topic/93141/qtablewidget-itemselectionchanged/13
        #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
        MSG* msg = *reinterpret_cast<MSG**>(message);
        #else
        MSG* msg = reinterpret_cast<MSG*>(message);
        #endif

        switch (msg->message)
        {
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
};

#endif // WINLESSWINDOW_H
