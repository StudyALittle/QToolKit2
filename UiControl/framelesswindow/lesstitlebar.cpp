#include "lesstitlebar.h"
#include "ui_lesstitlebar.h"
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>

LessTitleBar::LessTitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LessTitleBar)
{
    ui->setupUi(this);
}

LessTitleBar::~LessTitleBar()
{
    delete ui;
}

void LessTitleBar::setMainWidget(WidgetLessWindow *mainWidget)
{
    mainWidget->addIgnoreWidget(ui->label);
    ui->widgetBtns->setMainWidget(mainWidget);
}

void LessTitleBar::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return QWidget::paintEvent(event);
}
