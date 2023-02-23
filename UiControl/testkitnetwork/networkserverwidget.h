#ifndef NETWORKSERVERWIDGET_H
#define NETWORKSERVERWIDGET_H

#include <QWidget>
#include "networkserver.h"
#include "netframe/netframe.h"

namespace Ui {
class NetworkServerWidget;
}

class NetworkServerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkServerWidget(QWidget *parent = nullptr);
    ~NetworkServerWidget();

private slots:
    void onSigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<wkit::Session> session);

    void on_pushButtonStart_clicked();

private:
    Ui::NetworkServerWidget *ui;

    wkit::NetworkServer *m_server;
};

#endif // NETWORKSERVERWIDGET_H
