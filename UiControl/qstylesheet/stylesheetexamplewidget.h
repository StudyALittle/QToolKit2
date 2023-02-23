#ifndef STYLESHEETEXAMPLEWIDGET_H
#define STYLESHEETEXAMPLEWIDGET_H

#include <QWidget>

namespace Ui {
class StyleSheetExampleWidget;
}

class StyleSheetExampleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StyleSheetExampleWidget(QWidget *parent = nullptr);
    ~StyleSheetExampleWidget();

private slots:
    void on_pushButtonUseStyleSheet_clicked();

private:
    Ui::StyleSheetExampleWidget *ui;
};

#endif // STYLESHEETEXAMPLEWIDGET_H
