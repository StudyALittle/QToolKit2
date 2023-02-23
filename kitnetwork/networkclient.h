#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QEventLoop>
#include <QTimer>
#include <QThread>
#include <memory>
#include "kitnetwork_global.h"
#include "netframe/netframe.h"

namespace wkit {

class TcpClient;
class UdpServer;
class NetDataReslice;

/**
 * @brief The NetworkClient class: 网络通讯客户端
 */
class KITNETWORK_EXPORT NetworkClient: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief NetworkClient
     * @param type: TCP或者UDP
     */
    NetworkClient(NetworkTreatyType type = NetworkTreatyType::UDP);
    virtual ~NetworkClient();

    /**
     * @brief setBBigLittleMode: 是否判断数据大小端，默认不判断
     * @param bBigLittleMode
     */
    void setBBigLittleMode(bool bBigLittleMode);

    /**
     * @brief sIp: 连接的IP地址
     * @return
     */
    QString sIp();

    /**
     * @brief port: 连接的端口
     * @return
     */
    quint16 port();

    /**
     * @brief start: 连接服务端
     * @param ip: 服务端IP
     * @param port: 服务端端口
     * @param bHead: 接收的数据是否判断携带数据头
     * @return
     */
    bool start(const QString &ip, quint16 port, bool bHead = true);

    /**
     * @brief restart: 重连
     * @return
     */
    bool restart();

    /**
     * @brief close: 关闭连接
     */
    void close();

    /**
     * @brief sendData: 发送数据到服务端（同步，线程不阻塞）
     * @param data: 发送的数据
     * @param frameType: 数据类型 (DataFrameType)
     * @param timeOut: 超时时间
     * @param error: 返回错误 (对应 NetResultCode)
     * @return
     */
    std::shared_ptr<QByteArray> sendData(const QByteArray &data, ushort frameType = DataFrameType::FT_SyncYncData,
                                         int timeOut = 3000, int *error = nullptr);

    /**
     * @brief sendDataAsync: 发送数据到服务端(异步)
     * @param param
     * @param frameType: 数据类型 (DataFrameType)
     * @return
     */
    qint64 sendDataAsync(const QByteArray &param, ushort frameType = DataFrameType::FT_SyncYncData);

    /**
     * @brief sendDataAsyncNoHead: 发送数据到服务端(异步) , 没有数据头，没有分包处理数据，接收到的数据也没有数据头
     * @param param
     * @return
     */
    qint64 sendDataAsyncNoHead(const QByteArray &param);

signals:
    /**
     * @brief sigRecvData: 接收到数据时触发（异步调用才会触发）
     * @param data: 接收到的数据
     * @param session:
     */
    void sigRecvData(std::shared_ptr<QByteArray> data, std::shared_ptr<Session> session);

    /**
     * @brief disconnected: 客户端断开连接信号
     * @param ipV4
     * @param port
     */
    void disconnected(quint32 ipV4, int port);

    /**
     * @brief writeError: 写入数据异常（可能是网络原因）
     * @param ipV4
     * @param port
     */
    void writeError(quint32 ipV4, int port);

protected:
    /**
     * @brief dataAnalysis: 数据解析
     * @param data
     * @param ipV4
     * @param port
     */
    void dataAnalysis(const QByteArray &data, quint32 ipV4, int port);

    /**
     * @brief getDataById: 获取数据
     * @param id
     * @param error
     * @return
     */
    std::shared_ptr<QByteArray> getDataById(unsigned int id, int *error = nullptr);

protected slots:
    /**
     * @brief onReadDataFinish: 接收到数据
     * @param data: 原始数据（未解析数据头）
     * @param ipV4
     * @param port
     */
    void onReadDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);

protected:
    /**
     * @brief startTimerEx: 开启定时器
     * @param timeOut
     */
    void startTimerEx(int timeOut);

    /**
     * @brief closeTimer: 关闭定时器
     */
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

    /// 是否判断数据大小端
    bool m_bBigLittleMode; // true: 判断数据大小端
};

} // end namespace
#endif // NETWORKCLIENT_H
