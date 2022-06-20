#ifndef LESSTITLEBAR_H
#define LESSTITLEBAR_H

#include <QWidget>
#include "frameless.h"

namespace Ui {
class LessTitleBar;
}

class LessTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit LessTitleBar(QWidget *parent = nullptr);
    ~LessTitleBar();

    void setMainWidget(WidgetLessWindow *mainWidget);

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::LessTitleBar *ui;
};

#endif // LESSTITLEBAR_H
