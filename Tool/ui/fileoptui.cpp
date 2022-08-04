#include "fileoptui.h"
#include "ui_fileoptui.h"

FileOptUi::FileOptUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileOptUi)
{
    ui->setupUi(this);
}

FileOptUi::~FileOptUi()
{
    delete ui;
}
