#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include "kitnetwork_global.h"
#include "netframe/netframe.h"
#include <QObject>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <memory>

namespace wkit {

class TcpClient;
class UdpServer;
class NetDataReslice;

class KITNETWORK_EXPORT NetworkClient: public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief NetworkClient: 构造函数
    /// \param type：TCP/UDP
    ///
    NetworkClient(NetworkTreatyType type = NetworkTreatyType::UDP);

    ///
    /// \brief ~NetworkClient
    ///
    virtual ~NetworkClient() override;

    ///
    /// \brief setBBigLittleMode: true: 判断数据大小端
    ///
    void setBBigLittleMode(bool bBigLittleMode);

    ///
    /// \brief sIp
    /// \return
    ///
    QString sIp();

    ///
    /// \brief port
    /// \return
    ///
    quint16 port();

    ///
    /// \brief start: 连接服务
    /// \param ip：服务端IP
    /// \param port：端口
    /// \param bHead: 接收的数据是否判断携带数据头
    /// \return 连接情况
    ///
    bool start(const QString &ip, quint16 port, bool bHead = true);
    /// 重连
    bool restart();

    ///
    /// \brief close: 关闭连接
    /// \return
    ///
    void close();

    ///
    /// \brief sendData：发送数据到服务端
    /// \param param：发送的数据
    /// \param frameType：数据类型
    /// \param timeOut：超时时间
    /// \return
    ///
    std::shared_ptr<QByteArray> sendData(const QByteArray &param, ushort frameType, int timeOut = 3000);

    ///
    /// \brief sendData: 发送数据到服务端(异步)
    /// \param param
    /// \return
    ///
    qint64 sendDataAsync(const QByteArray &param, ushort frameType);
    /// 发送数据到服务端(异步) , 没有数据头，没有分包处理数据，接收到的数据也没有数据头
    qint64 sendDataAsyncNoHead(const QByteArray &param);
signals:
    ///
    /// \brief recvData：接收到数据时触发（异步调用才会触发）
    /// \param data：接收到的数据
    /// \param session：数据头信息
    ///
    void sigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<Session> session);
    /// 客户端断开连接信号
    void disconnected(quint32 ipV4, int port);
protected:
    ///
    /// \brief dataAnalysis: 数据解析
    /// \param data
    ///
    void dataAnalysis(const QByteArray &data, quint32 ipV4, int port);

    ///
    /// \brief getDataById: 获取数据
    /// \param id
    /// \return
    ///
    std::shared_ptr<QByteArray> getDataById(unsigned int id);
protected slots:
    ///
    /// \brief onReadDataFinish: 接收到数据
    /// \param data： 未解析头的数据
    /// \param ipV4
    /// \param port
    ///
    void onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);

protected:
    ///
    /// \brief startTimer：开启定时器
    ///
    void startTimerEx(int timeOut);

    ///
    /// \brief closeTimer：关闭定时器
    ///
    void closeTimer();
private:
    QString m_ip;
    quint16 m_port;
    NetworkTreatyType m_tType;
    QEventLoop m_eventLoop;
    QTimer m_timer;
    bool m_bInitTimer;
    QMutex m_mutexSyncId;
    QMutex m_mutexTimer;
    bool m_bHead;
    std::shared_ptr<QByteArray> m_dataNoHead;
    QMap<unsigned int, std::shared_ptr<QByteArray> > m_syncIds; //key: 数据编号标识, value: 返回的数据
    QMap<unsigned int, qint64> m_outtimeDataId; // 记录超时数据的ID key: 数据编号标识, value：数据发送时间

    TcpClient *m_tcpclient;
    UdpServer *m_udpserver;
    NetDataReslice *m_netDataReslice;

    /// 是否与第三方系统通信
    bool m_bBigLittleMode; // true: 判断数据大小端
};

} // end namespace
#endif // NETWORKCLIENT_H
