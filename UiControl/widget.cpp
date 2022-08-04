#include "widget.h"
#include "ui_widget.h"
#include "dialog.h"
#include <QMessageBox>

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

void Widget::on_pushButton_clicked()
{
    Dialog dl(this);
    dl.setStyleSheet("background-color: rgb(48, 45, 153);");
    dl.Exec();
    //resetWidget();
}

void Widget::on_pushButton_2_clicked()
{
    QMessageBox::information(this, "test", "test");
}

void Widget::on_pushButton_3_clicked()
{
    static Widget *wd = nullptr;
    if(!wd) {
        wd = new Widget;
    }
    wd->showFullScreen();
}

void Widget::on_pushButton_4_clicked()
{
    Dialog dl;
    dl.setDrawBorder(true);
    dl.setStyleSheet("background-color: rgb(48, 45, 153);");
    dl.Exec();
}
