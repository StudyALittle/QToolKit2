#include "networkclientwidget.h"
#include "ui_networkclientwidget.h"

NetworkClientWidget::NetworkClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkClientWidget)
{
    ui->setupUi(this);
    m_client = nullptr;
}

NetworkClientWidget::~NetworkClientWidget()
{
    if (m_client) {
        delete m_client;
        m_client = nullptr;
    }
    delete ui;
}

void NetworkClientWidget::onSigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<wkit::Session> session)
{
    Q_UNUSED(session)

    if (!m_client)
        return;

    ui->textBrowser->append(*data);
}

void NetworkClientWidget::on_pushButton_clicked()
{
    if (!m_client) {
        m_client = new wkit::NetworkClient(wkit::NetworkTreatyType::TCP);
        connect(m_client, &wkit::NetworkClient::sigRecvData,
                this, &NetworkClientWidget::onSigRecvData);
    }
    if (m_client->start(ui->lineEditIP->text(), ui->lineEditPort->text().toUInt())) {
        ui->textBrowser->setText("con success...");
    } else {
        ui->textBrowser->setText("con error...");
    }
}

void NetworkClientWidget::on_pushButtonSend_clicked()
{
    if (!m_client)
        return;

    ui->pushButtonSend->setEnabled(false);
    QString text = ui->textEdit->toPlainText();
    auto data = m_client->sendData(text.toStdString().c_str(), wkit::FT_SyncData);
    if (data) {
        ui->textBrowser->append(*data);
    }
    ui->pushButtonSend->setEnabled(true);
}

void NetworkClientWidget::on_pushButtonYncSend_clicked()
{
    if (!m_client)
        return;

    QString text = ui->textEdit->toPlainText();
    m_client->sendDataAsync(text.toStdString().c_str(), wkit::FT_YncData);
}
