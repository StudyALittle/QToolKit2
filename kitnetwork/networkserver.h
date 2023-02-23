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
    /**
     * @brief NetworkServer: 网络服务
     * @param type: TCP或者UDP
     */
    NetworkServer(NetworkTreatyType type = NetworkTreatyType::UDP);
    ~NetworkServer();

    /**
     * @brief setBBigLittleMode: 是否判断数据大小端，默认不判断
     * @param bBigLittleMode
     */
    void setBBigLittleMode(bool bBigLittleMode);

    /**
     * @brief start：开启服务
     * @param port：端口
     * @param bRawData: true 返回接收到的原始数据 - false 返回不包含数据头的数据
     * @return
     */
    bool start(quint16 port, bool bRawData = false);

    /**
     * @brief close: 关闭服务
     */
    void close();

    /**
     * @brief sendData: 发送数据到客户端（UDP）
     * @param data: 发送数据
     * @param session: 客户端session
     * @param bCut: 是否分包（数据太大不分包会发送失败）
     * @param bAutoId: false, 使用session里面的gcInfo - true, 自动生成gcInfo (服务端发送数据时，可能需要把客户端携带的ID传回去)
     * @return
     */
    qint64 sendData(const QByteArray &data, std::shared_ptr<Session> session, bool bCut = true, bool bAutoId = false);

signals:
    /**
     * @brief sigRecvData: 接收到数据信号
     * @param data: 接收到的数据
     * @param session: 用户信息
     */
    void sigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<Session> session);

protected slots:
    /**
     * @brief onReadDataFinish: 接收数完成
     * @param data
     * @param ipV4
     * @param port
     */
    void onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);

private:
    NetworkTreatyType m_tType;
    bool m_bBigLittleMode;      // true: 判断数据大小端
    bool m_bRawData;            // true：未解析的数据， false: 解析过数据头的数据

    TcpServer *m_tcpserver;
    UdpServer *m_udpserver;
    NetDataReslice *m_netDataReslice;
};

} // end namespace
#endif // NETWORKSERVER_H
