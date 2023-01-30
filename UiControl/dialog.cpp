#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    DialogLessWindow(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setTitleBar(ui->frameTitlebar);
    setDrawBorder(false);

    setDbClickTitlebarMax(false);
    setResizeEnable(false);
    setMoveEnable(true);
    addIgnoreWidget(ui->label);
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
