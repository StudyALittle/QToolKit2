#include "netdatareslice.h"

using namespace wkit;

NetDataReslice::NetDataReslice(UdpServer *udpServer):
    m_udpServer(udpServer)
{
    m_tcpServer = nullptr;
    m_tcpClient = nullptr;
}
NetDataReslice::NetDataReslice(TcpServer *tcpServer):
    m_tcpServer(tcpServer)
{
    m_udpServer = nullptr;
    m_tcpClient = nullptr;
}
NetDataReslice::NetDataReslice(TcpClient *tcpClient):
    m_tcpClient(tcpClient)
{
    m_udpServer = nullptr;
    m_tcpServer = nullptr;
}

///
/// \brief sendCut：发送数据
/// \param head：数据头
/// \param data：携带数据
/// \return
///
int NetDataReslice::sendCut(const QByteArray &data,
               const QString &ip, quint16 port,
             ushort frameType,
             uchar sendAdd,
            uchar recvAdd, uint *frameId, uint *unRest, bool bCut, const uchar *cFrameId)
{
    uint id = 0;
    int sendLen = 0;
    auto listData = m_reslice.splitFrame(data, frameType, sendAdd, recvAdd, id, unRest, bCut, cFrameId);
    for(std::shared_ptr<QByteArray> item: listData) {
        if(m_udpServer)
            sendLen += m_udpServer->writeDataToServer(*item, ip, port);
        else if(m_tcpClient)
            sendLen += m_tcpClient->writeData(*item);
        else if(m_tcpServer)
            sendLen += m_tcpServer->writeData(*item, ip, port);
    }
    if(frameId)
        *frameId = id;
    return sendLen;
}

///
/// \brief recomData: 重组数据
/// \param data: 需要重组的数据
/// \param recv: 重组完成的数据
/// \param bOk: 判断数据是否解析失败
/// \return true 表示数据重组完成返回, 否则返回nullptr
///
QList<FrameItem *> NetDataReslice::recomData(const QByteArray &data, quint32 ipv4, quint16 port, bool bBigLittleMode)
{
    return m_reslice.joinFrame(ipv4, port, data, bBigLittleMode);
}
