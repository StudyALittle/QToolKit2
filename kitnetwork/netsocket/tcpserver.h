#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMap>
#include <QTcpServer>
#include <QMutex>
#include <memory>
#include <QThread>
#include "tcpclient.h"

namespace wkit {

class TcpServer: public QObject
{
    Q_OBJECT
public:
    TcpServer();
    ~TcpServer();

    ///
    /// \brief start：开启服务
    /// \param port
    /// \return
    ///
    bool start(quint16 port);

    /**
     * @brief close
     * @return
     */
    void close();

    ///
    /// \brief writeData：发送数据
    /// \param data
    /// \param ipv4
    /// \param port
    /// \return
    ///
    qint64 writeData(const QByteArray &data, quint32 ipv4, quint16 port);
    qint64 writeData(const QByteArray &data, const QString &ipv4, quint16 port);
signals:
    /// 把收到的数据发送出去
    void readDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);
    /// 写数据到客户端
    void sigWriteToClient(const QByteArray &data);
protected slots:

    /// 新的连接
    void onNewConnection();
    /// 客户端断开连接
    void onDisconnected(quint32 ipV4, int port);

protected:
    ///
    /// \brief getClient: 获取客户端的引用
    /// \return
    ///
    TcpClient::Ptr getClient(quint32 ipV4, int port);
private:
    QTcpServer *m_tcpServer;

    QThread *m_cThread; // 保存当前线程的地址

    /// key: ip  value: (key: port value: client)
    QMap<quint32, QMap<int, TcpClient::Ptr> > m_clients;
    QMutex m_mutexClients;
};

} // end namespace

#endif // TCPSERVER_H
