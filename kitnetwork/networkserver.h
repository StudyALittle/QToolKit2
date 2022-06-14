#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include "kitnetwork_global.h"
#include "netframe/netframe.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QEventLoop>
#include <QTimer>
#include <memory>

namespace wkit {

class TcpServer;
class UdpServer;
class NetDataReslice;

class KITNETWORK_EXPORT NetworkServer: public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief NetworkServer
    /// \param type
    ///
    NetworkServer(NetworkTreatyType type = NetworkTreatyType::UDP);

    ///
    ~NetworkServer();

    ///
    /// \brief setBBigLittleMode: true: 判断数据大小端
    ///
    void setBBigLittleMode(bool bBigLittleMode);

    ///
    /// \brief start：开启服务
    /// \param port：端口
    /// \param bRawData：true: 返回为解析头的数据
    /// \return
    ///
    bool start(quint16 port, bool bRawData = false);

    ///
    /// \brief close: 关闭服务
    /// \return
    ///
    void close();

    ///
    /// \brief sendData: 发送数据到客户端（UDP）
    /// \param param
    /// \param ipV4
    /// \param port
    /// \param bCut: true, 分包 false: 不分包
    /// \param bAutoId: false: 使用session 里面的gcInfo, true, 自动生成gcInfo
    /// \return
    ///
    qint64 sendData(const QByteArray &param, std::shared_ptr<Session> session, bool bCut = true, bool bAutoId = false);
signals:
    ///
    /// \brief sigRecvData
    /// \param data
    ///
    void sigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<Session> session);
protected slots:
    void onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);
private:
    NetworkTreatyType m_tType;
    bool m_bBigLittleMode; // true: 判断数据大小端
    bool m_bRawData; // true：未解析的数据， false: 解析过数据头的数据

    TcpServer *m_tcpserver;
    UdpServer *m_udpserver;
    NetDataReslice *m_netDataReslice;
};

} // end namespace
#endif // NETWORKSERVER_H
