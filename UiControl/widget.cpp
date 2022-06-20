#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : WidgetLessWindow(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    /// 无边框设置
    // 设置标题栏才可拖动
    setTitleBar(ui->widgetTitlebar);
    // 设置可移动
    setMoveEnable(true);
    // 设置可拉伸
    setResizeEnable(true);
    // 设置双击标题栏最大化/不最大化
    setDbClickTitlebarMax(true);
    // linux不需要设置（设置无效）
    //addIgnoreWidget(ui->label);
    // 在无边框的环境下，窗口无阴影，需要设置画边框
    setDrawBorder(true);

    // 标题栏设置
    ui->widgetTitlebar->setMainWidget(this);
}

Widget::~Widget()
{
    delete ui;
}
