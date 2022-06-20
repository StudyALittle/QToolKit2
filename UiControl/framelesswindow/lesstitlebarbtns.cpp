#include "lesstitlebarbtns.h"
#include "ui_lesstitlebarbtns.h"

LessTitleBarBtns::LessTitleBarBtns(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LessTitleBarBtns)
{
    ui->setupUi(this);
}

LessTitleBarBtns::~LessTitleBarBtns()
{
    delete ui;
}

void LessTitleBarBtns::setMainWidget(QWidget *mainWidget)
{
    m_mainWidget = mainWidget;
}

void LessTitleBarBtns::on_pushButtonTitlebarMin_clicked()
{
    m_mainWidget->showMinimized();
}

void LessTitleBarBtns::on_pushButtonTitlebarMax_clicked()
{
    if(m_mainWidget->isMaximized()) {
        m_mainWidget->showNormal();
    }else {
        m_mainWidget->showMaximized();
    }
}

void LessTitleBarBtns::on_pushButtonTitlebarClose_clicked()
{
    m_mainWidget->close();
}
