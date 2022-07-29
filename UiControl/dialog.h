#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "framelesswindow/frameless.h"

namespace Ui {
class Dialog;
}

class Dialog : public DialogLessWindow
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void Exec();
private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
