#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QByteArray>
#include <QTcpSocket>
#include <memory>
#include <QThread>

/// tcp最大接收缓冲区
#define TCP_READBUF_MAX_SIZE 1024*1024*10

namespace wkit {

class TcpClient: public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<TcpClient>;

    TcpClient(QTcpSocket *tcpSocket = nullptr, quint32 ipv4 = 0, quint16 port = 0);
    ~TcpClient();

    ///
    /// \brief thisThread: 返回当前对象创建的线程
    /// \return
    ///
    QThread *thisThread();

    ///
    /// \brief start：开启客户端
    /// \param ipv4
    /// \param port
    /// \return
    ///
    bool start(quint32 ipv4, quint16 port);
    bool start(const QString &ipv4, quint16 port);

    ///
    /// \brief writeData：发送数据
    /// \param data
    ///
    qint64 writeData(const QByteArray &data);

    ///
    /// \brief lastSocketError: 最新的错误
    /// \return
    ///
    QAbstractSocket::SocketError lastSocketError();

signals:
    /// 把收到的数据发送出去
    void readDataFinish(std::shared_ptr<QByteArray> data, quint32 ipV4, int port);
    /// 连接成功信号
    void connected();
    /// 客户端断开连接信号
    void disconnected(quint32 ipV4, int port);
    /// 写数据
    void sigWriteNt(const QByteArray &data);
protected:
    /// 初始化连接槽
    void initConnectSlot();
    /// 当前线程写数据
    qint64 onWriteDataLt(const QByteArray &data);
public slots:
    void onWriteData(const QByteArray &data);
protected slots:
    /// 收到数据后触发此槽函数
    void readyRead();
    /// 错误槽函数
    void onError(QAbstractSocket::SocketError socketError);
private:
    bool m_bDelSocket;
    QTcpSocket *m_tcpSocket;
    quint32 m_ipV4;
    int m_port;

    QThread *m_cThread; // 保存当前线程的地址

    bool m_bDisconnect; // 是否已经与对端断开了连接
    QAbstractSocket::SocketError m_lastSocketError;
};

} // end namespace wkit

#endif // TCPCLIENT_H
