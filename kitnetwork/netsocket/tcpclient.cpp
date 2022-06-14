#include "tcpclient.h"
#include <QHostAddress>

TcpClient::TcpClient(QTcpSocket *tcpSocket, quint32 ipv4, quint16 port):
    m_tcpSocket(tcpSocket), m_ipV4(ipv4), m_port(port)
{
    m_cThread = QThread::currentThread();
    connect(this, &TcpClient::sigWriteNt, this, &TcpClient::onWriteData, Qt::QueuedConnection);
    if(m_tcpSocket) {
        m_tcpSocket->setReadBufferSize(TCP_READBUF_MAX_SIZE);
        m_bDelSocket = false;
        initConnectSlot();
    }else {
        m_bDelSocket = true;
    }
}
TcpClient::~TcpClient()
{
    if(m_bDelSocket && m_tcpSocket) {
        if(m_tcpSocket->isOpen()) {
            m_tcpSocket->close();
        }
        delete m_tcpSocket;
        m_tcpSocket = nullptr;
    }
}


///
/// \brief thisThread: 返回当前对象创建的线程
/// \return
///
QThread *TcpClient::thisThread()
{
    return m_cThread;
}

///
/// \brief start：开启客户端
/// \param ipv4
/// \param port
/// \return
///
bool TcpClient::start(quint32 ipv4, quint16 port)
{
    m_ipV4 = ipv4;
    m_port = port;
    if(!m_tcpSocket) {
        m_tcpSocket = new QTcpSocket;
        m_tcpSocket->setReadBufferSize(TCP_READBUF_MAX_SIZE);
        initConnectSlot();
    }
    m_tcpSocket->connectToHost(QHostAddress(ipv4), port);
    return m_tcpSocket->waitForConnected();
}
bool TcpClient::start(const QString &ipv4, quint16 port)
{
    QHostAddress hostAddr(ipv4);
    return start(hostAddr.toIPv4Address(), port);
}

/// 初始化连接槽
void TcpClient::initConnectSlot()
{
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &TcpClient::readyRead);
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_tcpSocket, &QTcpSocket::disconnected, [=]() {
        emit disconnected(m_ipV4, m_port);
    });
}

void TcpClient::onWriteData(const QByteArray &data)
{
    onWriteDataLt(data);
}

///
/// \brief writeData：发送数据到服务端
/// \param data
///
qint64 TcpClient::writeData(const QByteArray &data)
{
    if(m_cThread == QThread::currentThread()) {
        return onWriteDataLt(data);
    }else {
        emit sigWriteNt(data);
        return data.size();
    }
}
qint64 TcpClient::onWriteDataLt(const QByteArray &data)
{
    qint64 rt = m_tcpSocket->write(data);
    m_tcpSocket->flush();
    return rt;
}

///
/// \brief lastSocketError: 最新的错误
/// \return
///
QAbstractSocket::SocketError TcpClient::lastSocketError()
{
    return m_lastSocketError;
}

/// 收到数据后触发此槽函数
void TcpClient::readyRead()
{
    auto data = std::make_shared<QByteArray>(m_tcpSocket->readAll());
    emit readDataFinish(data, m_ipV4, m_port);
}

/// 错误槽函数
void TcpClient::onError(QAbstractSocket::SocketError socketError)
{
    if(socketError == QAbstractSocket::RemoteHostClosedError) {
        /// 连接被关闭
    }
    m_lastSocketError = socketError;
}
