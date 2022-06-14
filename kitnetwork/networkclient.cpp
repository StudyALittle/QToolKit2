#include "networkclient.h"
#include "netsocket/udpserver.h"
#include "netsocket/tcpclient.h"
#include "netframe/netdatareslice.h"
#include <QDateTime>

using namespace wkit;

NetworkClient::NetworkClient(NetworkTreatyType type)
{
    m_tType = type;
    m_bInitTimer = false;
    m_bHead = true;
    m_dataNoHead = std::make_shared<QByteArray>();
    m_netDataReslice = nullptr;
    m_udpserver = nullptr;
    m_tcpclient = nullptr;
    m_bBigLittleMode = false;
}
NetworkClient::~NetworkClient() {
    if(m_netDataReslice)
        delete m_netDataReslice;
    if(m_udpserver)
        delete m_udpserver;
    if(m_tcpclient)
        delete m_tcpclient;
    m_udpserver = nullptr;
    m_tcpclient = nullptr;
    m_netDataReslice = nullptr;
}

///
/// \brief setBBigLittleMode: true: 判断数据大小端
///
void NetworkClient::setBBigLittleMode(bool bBigLittleMode)
{
    m_bBigLittleMode = bBigLittleMode;
}

QString NetworkClient::sIp()
{
    return m_ip;
}
quint16 NetworkClient::port()
{
    return m_port;
}

///
/// \brief start: 连接服务
/// \param ip：服务端IP
/// \param port：端口
/// \return 连接情况
///
bool NetworkClient::start(const QString &ip, quint16 port, bool bHead)
{
    m_ip = ip;
    m_port = port;
    m_bHead = bHead;
    if(m_tType == NetworkTreatyType::UDP){
        if(!m_udpserver) {
            m_udpserver = new UdpServer();
            m_netDataReslice = new NetDataReslice(m_udpserver);
            connect(m_udpserver, SIGNAL(readDataFinish(std::shared_ptr<QByteArray>, quint32, int)),
                    this, SLOT(onReadDataFinish(std::shared_ptr<QByteArray>, quint32, int)));
        }
        return m_udpserver->startClient(port);
    }else if(m_tType == NetworkTreatyType::TCP){
        if(!m_tcpclient) {
            m_tcpclient = new TcpClient();
            m_netDataReslice = new NetDataReslice(m_tcpclient);
            connect(m_tcpclient, SIGNAL(readDataFinish(std::shared_ptr<QByteArray>, quint32, int)),
                    this, SLOT(onReadDataFinish(std::shared_ptr<QByteArray>, quint32, int)));
            connect(m_tcpclient, SIGNAL(disconnected(quint32, int)), this, SIGNAL(disconnected(quint32, int)));
        }
        return m_tcpclient->start(ip, port);
    }
    return true;
}
/// 重连
bool NetworkClient::restart()
{
    if(m_tType == NetworkTreatyType::UDP){
        // UDP无连接
    }else if(m_tType == NetworkTreatyType::TCP){
        return m_tcpclient->start(m_ip, m_port);
    }
    return true;
}

///
/// \brief close: 关闭连接
/// \return
///
void NetworkClient::close()
{
    if(m_tType == NetworkTreatyType::UDP){
        //UDP无连接，不做处理
    }else if(m_tType == NetworkTreatyType::TCP){

    }
}

///
/// \brief sendData: 发送数据到服务端(同步)
///
std::shared_ptr<QByteArray> NetworkClient::sendData(const QByteArray &param, ushort frameType, int timeOut)
{
    startTimerEx(timeOut);

    uint id = 0;
    qint64 nr = m_netDataReslice->sendCut(param, m_ip, m_port, frameType, 0x00, 0x00, &id);

    if(nr <= 0){ //发送数据错误
        closeTimer();
        return std::make_shared<QByteArray>();
    }

    m_eventLoop.exec();
    return getDataById(id);
}

///
/// \brief sendData: 发送数据到服务端(异步)
/// \param param
/// \return
///
qint64 NetworkClient::sendDataAsync(const QByteArray &param, ushort frameType)
{
    qint64 nr = m_netDataReslice->sendCut(param, m_ip, m_port, frameType, 0x00, 0x00, nullptr);
    return nr;
}
qint64 NetworkClient::sendDataAsyncNoHead(const QByteArray &param) {
    qint64 nr = -1;
    if(m_tType == NetworkTreatyType::TCP) {
        nr = m_tcpclient->writeData(param);
    }else if(m_tType == NetworkTreatyType::UDP) {
        nr = m_udpserver->writeDataToServer(param , m_ip, m_port);
    }
    return nr;
}

///
/// \brief getDataById: 获取数据
/// \param id
/// \return
///
std::shared_ptr<QByteArray> NetworkClient::getDataById(unsigned int id)
{
    auto result = std::make_shared<QByteArray>();
    QMutexLocker locker(&m_mutexSyncId);
    if(m_syncIds.find(id) != m_syncIds.end()){
        result = m_syncIds.value(id);
        m_syncIds.remove(id);
    }else { // 未获取到数据，说明数据超时了
        m_outtimeDataId.insert(id, QDateTime::currentDateTime().toSecsSinceEpoch());
    }
    return result;
}

///
/// \brief dataAnalysis: 数据解析
/// \param data
///
void NetworkClient::dataAnalysis(const QByteArray &data, quint32 ipV4, int port)
{
    auto result = m_netDataReslice->recomData(data, ipV4, port, m_bBigLittleMode);
    for(int n = 0; n < result.size(); n ++) {
        auto *item = result.at(n);
        int fid = item->frameHead.usFrameID & FT_MAnd;
        switch (fid) {
        case DataFrameType::FT_SyncYncData:
        case DataFrameType::FT_SyncData: { // 处理同步数据
            // 处理同步数据
            {
                uint dId = 0;
                memcpy(&dId, item->frameHead.gcInfo, sizeof (uint));

                QMutexLocker locker(&m_mutexSyncId);
                if(m_outtimeDataId.contains(dId)) { // 数据超时
                    m_outtimeDataId.remove(dId);
                    delete item;
                    return;
                }else {
                    m_syncIds.insert(dId, std::make_shared<QByteArray>(item->data));
                }

                /// 清除超时的状态
                QList<unsigned int> lRm;
                qint64 tm = QDateTime::currentDateTime().toSecsSinceEpoch();
                for(auto it = m_outtimeDataId.begin(); it != m_outtimeDataId.end(); it ++) {
                    /// 清除10分钟都没有接收到的超时情况
                    if(tm - it.value() > 60*10) {
                        lRm.append(it.key());
                    }
                }
                foreach (unsigned int id, lRm) {
                    m_outtimeDataId.remove(id);
                }
            }
            closeTimer();
            if(m_eventLoop.isRunning()) m_eventLoop.exit();

            if(fid == DataFrameType::FT_SyncData) {  // 只处理同步数据
                delete item;
                break;
            }
        }
        default: { // 处理异步数据
            auto session = std::make_shared<Session>();
            session->ip = ipV4;
            session->port = port;
            session->head = std::make_shared<NetFrameHead>();
            memcpy(session->head.get(), &(item->frameHead), sizeof (NetFrameHead));
            emit sigRecvData(std::make_shared<QByteArray>(item->data), session);
            delete item;
        }
        }
    }
}

void NetworkClient::onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port)
{
    if(!m_bHead) { // 返回原始数据
        m_dataNoHead = data;
        auto session = std::make_shared<Session>();
        session->ip = ipV4;
        session->port = port;
        emit sigRecvData(data, session);
        return;
    }
    dataAnalysis(*data, ipV4, port);
}

///
/// \brief startTimer：开启定时器
///
void NetworkClient::startTimerEx(int timeOut)
{
    QMutexLocker locker(&m_mutexTimer);
    if(!m_bInitTimer) {
        m_bInitTimer = true;
        m_timer.setSingleShot(true);
        m_timer.callOnTimeout([=]() {
            if(m_eventLoop.isRunning()) m_eventLoop.exit();
        });
    }
    m_timer.start(timeOut);
}
///
/// \brief closeTimer：关闭定时器
///
void NetworkClient::closeTimer()
{
    QMutexLocker locker(&m_mutexTimer);
    if(m_timer.isActive()) m_timer.stop();
}
