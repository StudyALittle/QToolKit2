#include "netframedatareslice.h"

using namespace wkit;

NetFrameDataReslice::NetFrameDataReslice()
{
    m_fEnd = 0xFFFF;
    m_cutIdentif = 0;
    m_lastTime = QDateTime::currentMSecsSinceEpoch();
}

///
/// \brief splitFrame: 数据分包
/// \param data：要分包的数据
/// \param frameMaxSize：分包大小
/// \return
///
QList<std::shared_ptr<QByteArray> > NetFrameDataReslice::splitFrame(const QByteArray &data,
                                                                    ushort frameType,
                                                                    uchar sendAdd,
                                                                    uchar recvAdd,
                                                                    uint &frameId,
                                                                    uint *unRest,
                                                                    bool bCut,
                                                                    const uchar *cFrameId,
                                                                    int frameMaxSize)
{
    Q_UNUSED(unRest)

    m_cutIdentif ++;
    frameId = m_cutIdentif;

    QList<std::shared_ptr<QByteArray> > result;
    int dataContentSize = frameMaxSize - sizeof (NetFrameHead) - sizeof (ushort);

    NetFrameHead frame;
    frame.usFrameH = 0xAAAA; /** 帧头标识**/
    frame.ucRxAddr = recvAdd;      /** 接收方地址**/
    frame.ucTxAddr = sendAdd;      /** 发送方地址**/
    frame.usFrameID = frameType;     /** 信息标识(标识数据类型) **/
    // 服务端发送数据时，可能需要把客户端携带的ID传回去
    if(cFrameId) {
        memset(&(frame.gcInfo), 0, 8); /** 帧标识号（帧标识ID）**/
        memcpy(&(frame.gcInfo), cFrameId, 8);
    }else {
        memset(&(frame.gcInfo), 0, 8); /** 帧标识号（帧标识ID）**/
        memcpy(&(frame.gcInfo), &m_cutIdentif, sizeof (uint));
    }

    if(data.size() <= dataContentSize || !bCut) { // 不需要分包数据

        frame.unFrameLen = sizeof (NetFrameHead) + data.size() + sizeof (ushort); /** 帧长度  **/
        frame.usFrameN = 1;      /** 数据包数**/
        frame.usFrameNumber = 0; /** 包序号**/
        frame.unGroupLen = data.size();    /** 帧体长度**/
        frame.unFrameOffset = 0; /** 偏移量**/
        // frame.usRest = unRest ? *unRest : 0;

        auto spData = std::make_shared<QByteArray>();
        spData->append(reinterpret_cast<char*>(&frame), sizeof (NetFrameHead));
        spData->append(data);
        spData->append(reinterpret_cast<char*>(&m_fEnd), sizeof (ushort)); // 帧尾

        result.append(spData);
        return result;
    }else {
        int dataSize = data.size();
        int sendNum = dataSize/dataContentSize;
        if(dataSize % dataContentSize > 0) {
            sendNum += 1;
        }

        int pos = 0;
        /// 分多次发送数据
        for(int n = 0; n < sendNum; n ++) {
            int dataSize = n + 1 == sendNum ?  data.size() - pos : dataContentSize;

            frame.unFrameLen = sizeof (NetFrameHead) + dataSize + sizeof (ushort); /** 帧长度  **/
            frame.usFrameN = sendNum;      /** 数据包数**/
            frame.usFrameNumber = n; /** 包序号**/
            frame.unGroupLen = dataSize;    /** 帧体长度**/
            frame.unFrameOffset = dataContentSize * n; /** 偏移量**/
            //frame.usRest = unRest ? *unRest : 0;

            auto spData = std::make_shared<QByteArray>();
            spData->append(reinterpret_cast<char*>(&frame), sizeof (NetFrameHead));
            spData->append(data.data() + pos, dataSize);
            spData->append(reinterpret_cast<char*>(&m_fEnd), sizeof (ushort)); // 帧尾

            result.append(spData);

            pos += dataContentSize;
        }
        return result;
    }
}

///
/// \brief joinFrame: 组包数据
/// \param data：
/// \return
///
QList<FrameItem *> NetFrameDataReslice::joinFrame(quint32 ip, quint16 port, const QByteArray &data1, bool bBigLittleMode)
{
    QList<FrameItem *> items;
    if(data1.size() < static_cast<int>(sizeof (NetFrameHead))) {
        return items;
    }
    quint64 tg = QDateTime::currentMSecsSinceEpoch();
    if(tg - m_lastTime > BUF_DEL_TIME_SPACE) { // 丢弃超时数据
        m_bigData.clear();
    }else if(m_bigData.size() > BUF_MAX_DATA_SIZE) { // 数据太大丢弃
        m_bigData.clear();
    }
    m_lastTime = tg;

    m_bigData.append(data1); // 合并数据

    int len = m_bigData.size();
    int pos = 0;
    while (true) {
        auto *item = FrameItem::analysis(m_bigData.data() + pos, len, bBigLittleMode);
        if(item == nullptr) {
            if(len > 0) { // 还存在未解析完的数据（存储起来）
                m_bigData = QByteArray(m_bigData.data() + pos, len);
            }else {
                // 数据解析完了，清除数据
                m_bigData.clear();
            }
            break;
        }

        int dataLen = sizeof (NetFrameHead) + item->data.size();
        item->data.remove(item->data.size() - sizeof (ushort), sizeof (ushort)); // 去掉帧尾
        item->frameHead.unFrameLen -= sizeof (ushort);

        auto *itemWhole = m_private.jionFrame(ip, port, item);
        if(itemWhole) {
            items.append(itemWhole);
        }

        pos += dataLen;
        len -= dataLen;
        if(pos >= m_bigData.size() || len <= 0) {
            m_bigData.clear();
            break;
        }
    }
    return items;
}
