#ifndef NETWORKCLIENTWIDGET_H
#define NETWORKCLIENTWIDGET_H

#include <QWidget>
#include "networkclient.h"
#include "netframe/netframe.h"

namespace Ui {
class NetworkClientWidget;
}

class NetworkClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkClientWidget(QWidget *parent = nullptr);
    ~NetworkClientWidget();

private slots:
    void onSigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<wkit::Session> session);

    void on_pushButton_clicked();

    void on_pushButtonSend_clicked();

    void on_pushButtonYncSend_clicked();

private:
    Ui::NetworkClientWidget *ui;

    wkit::NetworkClient *m_client;
};

#endif // NETWORKCLIENTWIDGET_H
