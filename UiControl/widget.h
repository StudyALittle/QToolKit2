#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "framelesswindow/frameless.h"
#include "qstylesheet/stylesheetexamplewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public WidgetLessWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButtonStyleSheet_clicked();

private:
    Ui::Widget *ui;
    StyleSheetExampleWidget m_styleSheetWidget;
};
#endif // WIDGET_H
