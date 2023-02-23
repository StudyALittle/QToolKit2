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
