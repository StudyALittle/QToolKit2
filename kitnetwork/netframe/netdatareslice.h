#ifndef NETDATARESLICE_H
#define NETDATARESLICE_H

///
/// \brief The NetDataReslice class: 数据分包发送、接收
///

#include <memory>
#include "netframedatareslice.h"

namespace wkit {

class NetDataReslice
{
public:
    NetDataReslice(UdpServer *udpServer = nullptr);
    NetDataReslice(TcpServer *tcpServer = nullptr);
    NetDataReslice(TcpClient *tcpClient = nullptr);

    ///
    /// \brief sendCut：发送数据
    /// \param head：数据头
    /// \param data：携带数据
    /// \return
    ///
    int sendCut(const QByteArray &data,
                   const QString &ip, quint16 port,
                 ushort frameType,
                 uchar condense1 = 0x00,
                 uchar condense2 = 0x00,
                 uint *frameId = nullptr,
                 uint *unRest = nullptr, bool bCut = true, const uchar *cFrameId = nullptr);

    ///
    /// \brief recomData: 重组数据
    /// \param data: 需要重组的数据
    /// \param recv: 重组完成的数据
    /// \param bOk: 判断数据是否解析失败
    /// \return true 表示数据重组完成返回, 否则返回nullptr
    ///
    QList<FrameItem *> recomData(const QByteArray &data, quint32 ipv4, quint16 port, bool bBigLittleMode = false);
private:
    NetFrameDataReslice m_reslice;

    UdpServer *m_udpServer;
    TcpClient *m_tcpClient;
    TcpServer *m_tcpServer;
};

}

#endif // NETDATARESLICE_H
