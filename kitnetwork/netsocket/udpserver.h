#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QUdpSocket>
#include <QByteArray>
#include <memory>

class UdpServer: public QObject
{
    Q_OBJECT
public:
    UdpServer();
    ~UdpServer();
    static UdpServer &instance();
    static void delServer();
    ///
    /// \brief start: 开启UDP服务
    /// \param port：绑定端口
    /// \return
    ///
    bool startServer(quint16 port);

    ///
    /// \brief startClient: 开启UDP客户端
    /// \param port
    /// \return
    ///
    bool startClient(quint16 port);

    void closeServer();

    ///
    /// \brief writeDataToServer: 发送数据到UDP服务
    /// \param datagram
    /// \param host
    /// \param port
    /// \return
    ///
    qint64 writeDataToServer(const QByteArray &datagram, const QString &address, quint16 port);
    qint64 writeDataToServer(const QByteArray &datagram, quint32 ipv4, quint16 port);
signals:
    ///收到UDP数据后触发此槽函数
    void readDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);
protected slots:
    ///接收数据槽函数
    void readPendingDatagrams();
private:

    QUdpSocket m_udp;
};

#endif // UDPSERVER_H
