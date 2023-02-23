#include "networkserverwidget.h"
#include "ui_networkserverwidget.h"

NetworkServerWidget::NetworkServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkServerWidget)
{
    ui->setupUi(this);
    m_server = nullptr;
}

NetworkServerWidget::~NetworkServerWidget()
{
    if (m_server) {
        delete m_server;
        m_server = nullptr;
    }
    delete ui;
}

void NetworkServerWidget::onSigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<wkit::Session> session)
{
    if (!m_server)
        return;
    // 返回数据给服务端
    m_server->sendData("SR: " + (*data), session);
    ui->textBrowser->append(*data);
}

void NetworkServerWidget::on_pushButtonStart_clicked()
{
    if (!m_server) {
        m_server = new wkit::NetworkServer(wkit::NetworkTreatyType::TCP);
        connect(m_server, &wkit::NetworkServer::sigRecvData,
                this, &NetworkServerWidget::onSigRecvData);
    }

    if (m_server->start(ui->lineEditPort->text().toUInt())) {
        ui->textBrowser->setText("start success...");
    } else {
        ui->textBrowser->setText("start error...");
    }
}
