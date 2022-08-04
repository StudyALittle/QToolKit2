#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    DialogLessWindow(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setTitleBar(ui->frameTitlebar);
    setDrawBorder(false);
//    setMoveEnable(true);
//    setResizeEnable(false);
//    setDbClickTitlebarMax(false);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::Exec()
{
    exec();
}

void Dialog::on_pushButtonClose_clicked()
{
    reject();
}
