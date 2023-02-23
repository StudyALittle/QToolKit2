#include "stylesheetexamplewidget.h"
#include "ui_stylesheetexamplewidget.h"

StyleSheetExampleWidget::StyleSheetExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StyleSheetExampleWidget)
{
    ui->setupUi(this);
}

StyleSheetExampleWidget::~StyleSheetExampleWidget()
{
    delete ui;
}

void StyleSheetExampleWidget::on_pushButtonUseStyleSheet_clicked()
{
    QString styleSheet = ui->textEdit->toPlainText();
    this->setStyleSheet(styleSheet);
}
