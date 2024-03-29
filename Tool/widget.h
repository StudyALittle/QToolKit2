#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "ui/fileoptui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_toolButtonOpenFileOpt_clicked();

private:
    Ui::Widget *ui;
    FileOptUi *m_fileOptUi;
};
#endif // WIDGET_H
