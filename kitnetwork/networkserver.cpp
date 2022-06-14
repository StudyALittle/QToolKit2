#include "networkserver.h"
#include "netsocket/udpserver.h"
#include "netsocket/tcpserver.h"
#include "netframe/netdatareslice.h"

NetworkServer::NetworkServer(NetworkTreatyType type)
{
    m_tType = type;
    m_bBigLittleMode = false;

    m_tcpserver = nullptr;
    m_udpserver = nullptr;
    m_netDataReslice = nullptr;
}
NetworkServer::~NetworkServer()
{
    if(m_netDataReslice)
        delete m_netDataReslice;
    if(m_udpserver)
        delete m_udpserver;
    if(m_tcpserver)
        delete m_tcpserver;
    m_netDataReslice = nullptr;
    m_udpserver = nullptr;
    m_tcpserver = nullptr;
}

///
/// \brief setBBigLittleMode: true: 判断数据大小端
///
void NetworkServer::setBBigLittleMode(bool bBigLittleMode)
{
    m_bBigLittleMode = bBigLittleMode;
}

///
/// \brief start：开启服务
/// \param port：端口
/// \return
///
bool NetworkServer::start(quint16 port, bool bRawData)
{
    m_bRawData = bRawData;

    if(m_tType == NetworkTreatyType::UDP){
        if(!m_udpserver) {
            m_udpserver = new UdpServer();
            m_netDataReslice = new NetDataReslice(m_udpserver);
            connect(m_udpserver, SIGNAL(readDataFinish(std::shared_ptr<QByteArray>, quint32, int)),
                    this, SLOT(onReadDataFinish(std::shared_ptr<QByteArray>, quint32, int)));
        }
        return m_udpserver->startClient(port);
    }else if(m_tType == NetworkTreatyType::TCP){
        if(!m_tcpserver) {
            m_tcpserver = new TcpServer();
            m_netDataReslice = new NetDataReslice(m_tcpserver);
            connect(m_tcpserver, SIGNAL(readDataFinish(std::shared_ptr<QByteArray>, quint32, int)),
                    this, SLOT(onReadDataFinish(std::shared_ptr<QByteArray>, quint32, int)));
        }
        return m_tcpserver->start(port);
    }
    return true;
}

///
/// \brief close: 关闭服务
/// \return
///
void NetworkServer::close()
{
    if(m_tType == NetworkTreatyType::UDP){
        m_udpserver->closeServer();
    }else if(m_tType == NetworkTreatyType::TCP){

    }
}

///
/// \brief sendData: 发送数据到客户端（UDP）
/// \param param
/// \param ipV4
/// \param port
/// \return
///
qint64 NetworkServer::sendData(const QByteArray &param, std::shared_ptr<Session> session, bool bCut, bool bAutoId)
{
    QHostAddress addr(session->ip);
    int nr = m_netDataReslice->sendCut(param, addr.toString(), session->port,
                                       session->head->usFrameID, session->head->ucRxAddr,
                                       session->head->ucTxAddr, nullptr, nullptr, bCut,
                                       bAutoId ? nullptr : session->head->gcInfo);
    return nr;
}

void NetworkServer::onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port)
{
    if(m_bRawData) { // 返回未解析的数据
        auto session = std::make_shared<Session>();
        session->ip = ipV4;
        session->port = port;
        emit sigRecvData(data, session);
        return;
    }

    /// 分片重组数据
    auto result = m_netDataReslice->recomData(*data, ipV4, port, m_bBigLittleMode);
    for(int n = 0; n < result.size(); n ++) {
        auto *item = result.at(n);
        auto session = std::make_shared<Session>();
        session->ip = ipV4;
        session->port = port;
        session->head = std::make_shared<NetFrameHead>();
        memcpy(session->head.get(), &(item->frameHead), sizeof (NetFrameHead));
        emit sigRecvData(std::make_shared<QByteArray>(item->data), session);
        delete item;
    }
}
