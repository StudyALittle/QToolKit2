#ifndef LESSTITLEBARBTNS_H
#define LESSTITLEBARBTNS_H

#include <QWidget>

namespace Ui {
class LessTitleBarBtns;
}

class LessTitleBarBtns : public QWidget
{
    Q_OBJECT

public:
    explicit LessTitleBarBtns(QWidget *parent = nullptr);
    ~LessTitleBarBtns();

    void setMainWidget(QWidget *mainWidget);
private slots:
    void on_pushButtonTitlebarMin_clicked();

    void on_pushButtonTitlebarMax_clicked();

    void on_pushButtonTitlebarClose_clicked();

private:
    Ui::LessTitleBarBtns *ui;
    QWidget *m_mainWidget;
};

#endif // LESSTITLEBARBTNS_H
