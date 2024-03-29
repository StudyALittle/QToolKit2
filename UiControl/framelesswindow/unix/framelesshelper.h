﻿#ifndef FRAMELESS_HELPER_H
#define FRAMELESS_HELPER_H

#include <QObject>
#include <QDialog>
#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QApplication>
#include <QAbstractNativeEventFilter>

class QWidget;
class QDialog;
class FramelessHelperPrivate;

/// 无边框窗口边框颜色
#define FL_WIDGET_BODER_COLOR "gray"

///
/// Use x11 frameless widget(Linux下使用x11无边框方案)
/// .pro file add:
/// DEFINES += W_LESSWINDOW_X11
/// QT += x11extras
/// LIBS += -lX11 -lXext
///

/*****
* CursorPosCalculator
* 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
*****/
class CursorPosCalculator
{
public:
    explicit CursorPosCalculator();
    void reset();
    void recalculate(const QPoint &globalMousePos, const QRect &frameRect);

public:
    bool m_bOnEdges : true;
    bool m_bOnLeftEdge : true;
    bool m_bOnRightEdge : true;
    bool m_bOnTopEdge : true;
    bool m_bOnBottomEdge : true;
    bool m_bOnTopLeftEdge : true;
    bool m_bOnBottomLeftEdge : true;
    bool m_bOnTopRightEdge : true;
    bool m_bOnBottomRightEdge : true;

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};


/*****
* WidgetData
* 更新鼠标样式、移动窗体、缩放窗体
*****/
class WidgetData : public QObject
{
    Q_OBJECT

public:
    explicit WidgetData(FramelessHelperPrivate *d, QWidget *pTopLevelWidget, QObject *parent = 0);
    ~WidgetData();
    QWidget* widget();
    // 处理鼠标事件-划过、按下、释放、移动
    void handleWidgetEvent(QEvent *event);
    // 更新橡皮筋状态
    void updateRubberBandStatus();

    void setTitleBar(QWidget *titleBar);
    void setDbClickTitlebarMax(bool bDbClickTitlebarMax);

    void setMax();

    bool isX11Move();
    void x11MoveRelease();
private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint &gMousePos);
    // 重置窗体大小
    void resizeWidget(const QPoint &gMousePos);
    // 移动窗体
    void moveWidget(const QPoint &gMousePos);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    //处理鼠标双击
    void handleMouseDblClickEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

signals:
    void sigMax(bool bMax);

private:
    FramelessHelperPrivate *d;
    QRubberBand *m_pRubberBand;
    QWidget *m_pWidget;
    QPoint m_ptDragPos;
    CursorPosCalculator m_pressedMousePos;
    CursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTitlePressed;
    QRect m_rect;
    Qt::WindowFlags m_windowFlags;
    bool m_bDbClickTitlebarMax;

    bool m_x11Move; // 使用x11的无边框，保存窗口移动状态

    QWidget *m_titleBar;
};


class FramelessHelper : public QObject
{
    Q_OBJECT

public:
    explicit FramelessHelper(QObject *parent = 0);
    ~FramelessHelper();

    //
    void setTitleBar(QWidget *titleBar, QWidget *parent);
    void setDbClickTitlebarMax(bool bDbClickTitlebarMax);

    // 激活窗体
    void activateOn(QWidget *topLevelWidget);
    // 移除窗体
    void removeFrom(QWidget *topLevelWidget);
    // 设置窗体移动
    void setWidgetMovable(bool movable);
    // 设置窗体缩放
    void setWidgetResizable(bool resizable);
    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);
    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);
    // 设置边框的宽度
    void setBorderWidth(uint width);
    // 设置标题栏高度
    void setTitleHeight(uint height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();

    bool isX11Move();
    void x11MoveRelease();
protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void sigMax(bool bMax);

private:
    FramelessHelperPrivate *d;
    bool m_bDbClickTitlebarMax;
    QWidget *m_mainWidget;
};

class LessWindowBase
#if defined (W_LESSWINDOW_X11)
    : public QAbstractNativeEventFilter
#endif
{
public:
    LessWindowBase(QWidget *widget): m_widget(widget) {
#if defined (W_LESSWINDOW_X11)
    qApp->installNativeEventFilter(this);
#endif
        m_bMax = false;
        m_bMaxInit = false;
        m_maxChangeCallbackFunc = nullptr;
        m_frHelper = new FramelessHelper(widget);
        m_frHelper->activateOn(widget);
        m_bBorder = false;
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
    }
    virtual ~LessWindowBase() {
        m_frHelper->deleteLater();
        m_frHelper = 0;
    }

    void setMaxChangeCallback(std::function<void(bool)> maxChangeCallbackFunc) {
        m_maxChangeCallbackFunc = maxChangeCallbackFunc;
    }

    static void drawWidgetBorder(QWidget *widget) {
        /// 画边框
        QRect rt = widget->rect();
        rt.setWidth(widget->width() - 1);
        rt.setHeight(widget->height() - 1);
        QPainter painter(widget);
        painter.setPen(FL_WIDGET_BODER_COLOR);
        painter.drawRect(rt);
    }

    // 设置是否画边框，无边框在linux下无阴影，需要画边框
    void setDrawBorder(bool bBorder) {
        m_bBorder = bBorder;
        // 设置边距，画边框
        int cMg = 1;
        m_widget->setContentsMargins(cMg, cMg, cMg, cMg);
    }

    //设置是否可以通过鼠标调整窗口大小
    void setResizeEnable(bool resizeable) {
        m_frHelper->setWidgetResizable(resizeable);
    }

    //设置是否可以通过鼠标移动窗口
    void setMoveEnable(bool moveEnable) {
        m_frHelper->setWidgetMovable(moveEnable);
    }

    //点击标题栏是否全屏（默认全屏）
    void setDbClickTitlebarMax(bool bDbClickTitlebarMax){
        m_frHelper->setDbClickTitlebarMax(bDbClickTitlebarMax);
    }

    //设置一个标题栏widget，此widget会被当做标题栏对待
    void setTitleBar(QWidget* titlebar) {
        m_frHelper->setTitleBar(titlebar, m_widget);
    }

    //设置可调整大小区域的宽度，在此区域内，可以使用鼠标调整窗口大小
    void setResizeableAreaWidth(int width = 5) {
        m_frHelper->setBorderWidth(width);
    }

    //在标题栏控件内，也可以有子控件如标签控件“label1”，此label1遮盖了标题栏，导致不能通过label1拖动窗口
    //要解决此问题，使用addIgnoreWidget(label1)
    void addIgnoreWidget(QWidget* widget) {
        Q_UNUSED(widget)
    }
#if defined (W_LESSWINDOW_X11)
    void maxChangeProccess();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif
protected:
    bool m_bBorder; // 是否设置边框（默认不设置）
    QWidget *m_widget;
    FramelessHelper *m_frHelper;
    std::function<void(bool)> m_maxChangeCallbackFunc; // 最大化消息回调

    bool m_bMax; // X11有效
    bool m_bMaxInit; // X11有效
};

class WidgetLessWindow: public QWidget, public LessWindowBase
{
    Q_OBJECT
public:
    WidgetLessWindow(QWidget *parent = 0):
        QWidget(parent), LessWindowBase(this) {
        connect(m_frHelper, SIGNAL(sigMax(bool)), this, SIGNAL(maxChange(bool)));

        // 窗口最大化、恢复正常状态回调
        auto maxChangeCallback = [this](bool bMax) {
            emit maxChange(bMax);
        };
        setMaxChangeCallback(maxChangeCallback);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        if(!m_bBorder || isMaximized()) {
            return QWidget::paintEvent(event);
        }

        /// 画边框
        drawWidgetBorder(this);
        return QWidget::paintEvent(event);
    }
signals:
    //bMax: true（最大化） false(正常大小)
    void maxChange(bool bMax = true);
};

class DialogLessWindow: public QDialog, public LessWindowBase
{
    Q_OBJECT
public:
    DialogLessWindow(QWidget *parent = 0):
        QDialog(parent), LessWindowBase(this) {
        connect(m_frHelper, SIGNAL(sigMax(bool)), this, SIGNAL(maxChange(bool)));

        // 窗口最大化、恢复正常状态回调
        auto maxChangeCallback = [this](bool bMax) {
            emit maxChange(bMax);
        };
        setMaxChangeCallback(maxChangeCallback);
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        if(!m_bBorder || isMaximized()) {
            return QDialog::paintEvent(event);
        }

        /// 画边框
        drawWidgetBorder(this);
        return QDialog::paintEvent(event);
    }
signals:
    //bMax: true（最大化） false(正常大小)
    void maxChange(bool bMax = true);
};

#endif //FRAMELESS_HELPER_H
