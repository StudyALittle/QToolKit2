#include "udpserver.h"
#include <QDebug>
#include <QNetworkDatagram>

using namespace wkit;

UdpServer::UdpServer()
{
    /// 设置接收缓冲区大小
    /// m_udp.setReadBufferSize(10*1024*1024);
}
UdpServer::~UdpServer()
{

}

static UdpServer *s_udp = nullptr;
UdpServer &UdpServer::instance()
{
    if(s_udp == nullptr){
        s_udp = new UdpServer(); //new 保证创建和调用都在同一个线程
    }
    return *s_udp;
//    static UdpServer s_udp;
//    return s_udp;
}
void UdpServer::delServer()
{
    if(s_udp != nullptr){
        delete s_udp;
        s_udp = nullptr;
    }
}

///
/// \brief start: 开启UDP服务
/// \param port：绑定端口
/// \return
///
bool UdpServer::startServer(quint16 port)
{
    connect(&m_udp, &QUdpSocket::readyRead,
                 this, &UdpServer::readPendingDatagrams);
    return m_udp.bind(QHostAddress::Any, port);
}

///
/// \brief startClient: 开启UDP客户端
/// \param port
/// \return
///
bool UdpServer::startClient(quint16 port)
{
    connect(&m_udp, &QUdpSocket::readyRead,
                 this, &UdpServer::readPendingDatagrams);
    return m_udp.bind(port, QAbstractSocket::ShareAddress);
}

void UdpServer::closeServer()
{
    m_udp.close();
}

///
/// \brief writeDataToServer: 发送数据到UDP服务
/// \param datagram
/// \param host
/// \param port
/// \return
///
qint64 UdpServer::writeDataToServer(const QByteArray &datagram, const QString &address, quint16 port)
{
    auto rt = m_udp.writeDatagram(datagram, QHostAddress(address), port);
    m_udp.flush();
    return rt;
}
qint64 UdpServer::writeDataToServer(const QByteArray &datagram, quint32 ipv4, quint16 port)
{
    auto rt = m_udp.writeDatagram(datagram, QHostAddress(ipv4), port);
    m_udp.flush();
    return rt;
}

///接收数据槽函数
void UdpServer::readPendingDatagrams()
{
    quint32 ipv4 = 0;
    int port = 0;
    int n = 0;
    auto data = std::make_shared<QByteArray>();
    while (m_udp.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udp.receiveDatagram();
        quint32 nIpV4 = datagram.senderAddress().toIPv4Address();
        int nPort = datagram.senderPort();
        if(n == 0){
            ipv4 = nIpV4;
            port = nPort;
        }else if(ipv4 != nIpV4){ //不是同一个IP的数据
            //把上一个得到得IP数据发送出去
            emit readDataFinish(data, ipv4, port);
            //重置参数
            ipv4 = nIpV4;
            port = nPort;
            //创建新的数据缓存
            data = std::make_shared<QByteArray>();
        }
        data->push_back(datagram.data());
        ++ n;
    }
//    qDebug() << "Recv Udp len: " << data->size();
    emit readDataFinish(data, ipv4, port);
}
