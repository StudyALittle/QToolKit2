#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_fileOptUi = nullptr;
}

Widget::~Widget()
{
    if(m_fileOptUi) {
        m_fileOptUi->close();
        m_fileOptUi->deleteLater();
    }
    delete ui;
}


void Widget::on_toolButtonOpenFileOpt_clicked()
{
    if(!m_fileOptUi) {
        m_fileOptUi = new FileOptUi;
    }
    m_fileOptUi->show();
}
