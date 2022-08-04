#ifndef FILEOPTUI_H
#define FILEOPTUI_H

#include <QWidget>

namespace Ui {
class FileOptUi;
}

class FileOptUi : public QWidget
{
    Q_OBJECT

public:
    explicit FileOptUi(QWidget *parent = nullptr);
    ~FileOptUi();

private:
    Ui::FileOptUi *ui;
};

#endif // FILEOPTUI_H
