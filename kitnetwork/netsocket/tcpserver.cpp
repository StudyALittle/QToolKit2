#include "tcpserver.h"

using namespace wkit;

TcpServer::TcpServer()
{
    m_cThread = QThread::currentThread();
    m_tcpServer = nullptr;
}
TcpServer::~TcpServer()
{
    if(m_tcpServer && m_tcpServer->isListening()) {
        m_tcpServer->close();
        delete m_tcpServer;
        m_tcpServer = nullptr;
    }
}

///
/// \brief start：开启服务
/// \param port
/// \return
///
bool TcpServer::start(quint16 port)
{
    if(!m_tcpServer) {
        m_tcpServer = new QTcpServer();

        connect(m_tcpServer, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
        // connect(m_tcpServer, &QTcpServer::acceptError, this, &TcpServer::displayError);
    }
    return m_tcpServer->listen(QHostAddress::Any, port);
}

/**
 * @brief close
 * @return
 */
void TcpServer::close()
{
    if (m_tcpServer) {
        m_tcpServer->close();
    }
}

///
/// \brief writeData：发送数据
/// \param data
/// \param ipv4
/// \param port
/// \return
///
qint64 TcpServer::writeData(const QByteArray &data, quint32 ipv4, quint16 port)
{
    TcpClient::Ptr client = getClient(ipv4, port);
    if(!client)
        return 0;
    /// 同一个线程直接发送数据（qtsocket 不支持跨线程发送数据）
    // if(client->thisThread() == QThread::currentThread())
    return client->writeData(data);
    // emit sigWriteToClient(data);
    // return data.size();
}
qint64 TcpServer::writeData(const QByteArray &data, const QString &ipv4, quint16 port)
{
    QHostAddress hostAddr(ipv4);
    return writeData(data, hostAddr.toIPv4Address(), port);
}

///
/// \brief getClient: 获取客户端的引用
/// \return
///
TcpClient::Ptr TcpServer::getClient(quint32 ipV4, int port)
{
    QMutexLocker locker(&m_mutexClients);
    if(!m_clients.contains(ipV4)) return nullptr;
    if(!m_clients[ipV4].contains(port)) return nullptr;
    return m_clients[ipV4].value(port);
}

/// 新的连接
void TcpServer::onNewConnection()
{
    QTcpSocket *socket = m_tcpServer->nextPendingConnection();
    if(!socket) return;

    quint32 ipv4 = socket->peerAddress().toIPv4Address();
    quint16 port = socket->peerPort();
    auto client = std::make_shared<TcpClient>(socket, ipv4, port);
    connect(client.get(), &TcpClient::readDataFinish, this, &TcpServer::readDataFinish);
    connect(client.get(), &TcpClient::disconnected, this, &TcpServer::onDisconnected);
    // connect(this, &TcpServer::sigWriteToClient, client.get(), &TcpClient::onWriteData, Qt::QueuedConnection);

    QMutexLocker locker(&m_mutexClients);
    if(m_clients.contains(ipv4)) {
        m_clients[ipv4].insert(port, client);
    }else {
        QMap<int, TcpClient::Ptr> m;
        m.insert(port, client);
        m_clients.insert(ipv4, m);
    }
}

/// 客户端断开连接
void TcpServer::onDisconnected(quint32 ipV4, int port)
{
    QMutexLocker locker(&m_mutexClients);
    if(!m_clients.contains(ipV4)) return;

    if(m_clients[ipV4].contains(port))
        m_clients[ipV4].remove(port);
    if(m_clients[ipV4].size() == 0)
        m_clients.remove(ipV4);
}
