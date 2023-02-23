#ifndef NETFRAMEDATARESLICE_H
#define NETFRAMEDATARESLICE_H

///
/// \brief The NetFrameDataReslice class: 数据分包
///

#include <memory>
#include <QList>
#include <QMap>
#include <QByteArray>
#include <QDateTime>
#include "netframe.h"
#include "netsocket/udpserver.h"
#include "netsocket/tcpclient.h"
#include "netsocket/tcpserver.h"
#include "datautil/datautil.h"

// 分包大小
#define FRAME_MAX_SPLITE_SIZE 1024*50
// 组包超时数据处理(ms) , 丢弃时间段未重组完成的数据
#define FRAME_MAX_SPLITETIMESPACE_SIZE 1000*60
// 缓存最大数据大小60M
#define BUF_MAX_DATA_SIZE 1024*1024*60
// 丢弃5秒前的缓存数据(ms)
#define BUF_DEL_TIME_SPACE 5000

namespace wkit {

// 保存接收到的一帧数据
struct FrameItem {
    // 数据头
    NetFrameHead frameHead;
    // 携带的数据
    QByteArray data;
    // 收到数据的时间
    QDateTime recvTime;

    ///
    /// \brief analysis： 解析组包数据
    /// \param data
    /// \param len：数据总长度
    /// \param bBigLittleMode: true: 判断大小端
    /// \return > 0: 当前帧加数据的总长度 <0: 错误
    ///
    static FrameItem* analysis(const char *data, int len, bool bBigLittleMode = false) {
        if(len < static_cast<int>(sizeof (NetFrameHead))) {
            return nullptr;
        }

        int pos = 0;
        FrameItem *frame = new FrameItem;
        frame->recvTime = QDateTime::currentDateTime();

        if(!bBigLittleMode) {
            memcpy(&(frame->frameHead), data, sizeof (NetFrameHead));
            pos += sizeof (NetFrameHead);
        }else {
            // 判断字节模式
            bool isSameEndien = true;
            ushort iWordMode = 0;
            int nModePos = sizeof (NetFrameHead) - sizeof (uint) * 2 - sizeof (ushort);
            DataUtil::cBinaryToNumber<ushort>(iWordMode, data, nModePos);
            bool isDataBigEndien = (0 != iWordMode);
            isSameEndien = (isDataBigEndien == DataUtil::isBigEndien());
            if(!isSameEndien) { // 字节模式与本机不一样
                DataUtil::cBinaryToNumber<ushort>(frame->frameHead.usFrameH, data, pos, isSameEndien);
                DataUtil::cBinaryToChart<uchar>(frame->frameHead.condense, data, pos, 2, isSameEndien);
                DataUtil::cBinaryToNumber<uint>(frame->frameHead.unFrameLen, data, pos, isSameEndien);
                DataUtil::cBinaryToNumber<ushort>(frame->frameHead.usFrameN, data, pos, isSameEndien);
                DataUtil::cBinaryToNumber<ushort>(frame->frameHead.usFrameNumber, data, pos, isSameEndien);
                DataUtil::cBinaryToNumber<ushort>(frame->frameHead.usFrameID, data, pos, isSameEndien);
                DataUtil::cBinaryToChart<uchar>(frame->frameHead.gcInfo, data, pos, 8, isSameEndien);
                DataUtil::cBinaryToNumber<ushort>(frame->frameHead.usRest, data, pos, isSameEndien);
                DataUtil::cBinaryToNumber<uint>(frame->frameHead.unGroupLen, data, pos, isSameEndien);
                DataUtil::cBinaryToNumber<uint>(frame->frameHead.unFrameOffset, data, pos, isSameEndien);
            }else {
                memcpy(&(frame->frameHead), data, sizeof (NetFrameHead));
                pos += sizeof (NetFrameHead);
            }
        }

        int dataLen = frame->frameHead.unFrameLen - sizeof (NetFrameHead);
        if(pos + dataLen > len) {
            delete frame;
            return nullptr;
        }
        frame->data.append(data + pos, dataLen);
        return frame;
    }
};

/// 多组重组数据信息
struct FrameItems {
    /// 需要从组的数据
    struct Datas {
        /// unsigned int: 切片编号
        /// DataItem: 分片数据
        QMap<uint, FrameItem*> items;

        Datas() {
            lastTime = QDateTime::currentMSecsSinceEpoch();
        }
        /// 上一次组包时间
        quint64 lastTime;
        Datas(FrameItem *item) {
            lastTime = QDateTime::currentMSecsSinceEpoch();
            items.insert(item->frameHead.usFrameNumber, item);
        }
        ~Datas() {
            del();
        }
        void del() {
            for(auto it = items.begin(); it != items.end(); it ++) {
                delete it.value();
            }
            items.clear();
        }

        FrameItem *jionFrame(FrameItem *item) {
            quint64 ct = QDateTime::currentMSecsSinceEpoch();
            if(ct - lastTime > FRAME_MAX_SPLITETIMESPACE_SIZE) { // 丢弃超时的组包数据
                del();
            }
            lastTime = ct;

            QByteArray bFrameId(reinterpret_cast<char*>(item->frameHead.gcInfo), sizeof (8));
            if(items.contains(item->frameHead.usFrameNumber)) {
                // 分包数据已经存在
                return nullptr;
            }
            // 保存分包数据
            items.insert(item->frameHead.usFrameNumber, item);
            // 判断包是否已经全部找到
            if(items.size() < item->frameHead.usFrameN) {
                return nullptr;
            }

            auto it = items.begin();
            FrameItem *itemWhole = it.value();
            it ++;
            // 表示找到所有的数据包（组包）
            for(; it != items.end(); it ++) {
                itemWhole->data.append(it.value()->data);
                delete it.value(); // 释放内存
            }
            items.clear();
            return itemWhole;
        }
    };

    /// 保存重组信息
    /// QByteArray：数据标识
    /// Data：多组分包数据信息
    QMap<QByteArray, Datas*> datas;

    FrameItems() {
        lastTime = QDateTime::currentMSecsSinceEpoch();
    }
    /// 上一次组包时间
    quint64 lastTime;
    FrameItems(FrameItem *item) {
        lastTime = QDateTime::currentMSecsSinceEpoch();
        QByteArray bFrameId(reinterpret_cast<char*>(item->frameHead.gcInfo), sizeof (8)); // 帧标识号
        init(item, bFrameId);
    }
    ~FrameItems() {
        del();
    }
    void del() {
        for(auto it = datas.begin(); it != datas.end(); it ++) {
            delete it.value();
        }
        datas.clear();
    }

    // 重组数据（如果重组成功，返回重组后的数据，否则返回空）
    FrameItem *jionFrame(FrameItem *item) {
        quint64 ct = QDateTime::currentMSecsSinceEpoch();
        if(ct - lastTime > FRAME_MAX_SPLITETIMESPACE_SIZE) { // 丢弃超时的组包数据
            del();
        }
        lastTime = ct;

        QByteArray frameId(reinterpret_cast<char*>(item->frameHead.gcInfo), sizeof (8)); // 帧标识号
        auto itFd = datas.find(frameId);
        if(itFd != datas.end()) { // 存在部分分包数据
            auto itemWhole = itFd.value()->jionFrame(item);
            if(itemWhole) {
                delete itFd.value(); // 释放内存
                datas.remove(frameId);
            }
            return itemWhole;
        }else { // 不存在数据
            init(item, frameId);
            return nullptr;
        }
    }

    void init(FrameItem *item, const QByteArray &frameId) {
        Datas *data = new Datas(item);
        datas.insert(frameId, data);
    }

};

/// 保存端口的重组信息
struct PortsFrames {
    PortsFrames() {
        lastTime = QDateTime::currentMSecsSinceEpoch();
    }
    /// 上一次组包时间
    quint64 lastTime;

    /// key: 端口; value: 需要重组的数据
    QMap<quint16, FrameItems*> datas;

    PortsFrames(quint16 port, FrameItem *item) {
        lastTime = QDateTime::currentMSecsSinceEpoch();
        init(port, item);
    }
    ~PortsFrames() {
        del();
    }
    void del() {
        for(auto it = datas.begin(); it != datas.end(); it ++) {
            delete it.value();
        }
        datas.clear();
    }

    // 重组数据（如果重组成功，返回重组后的数据，否则返回空）
    FrameItem *jionFrame(quint16 port, FrameItem *item) {
        quint64 ct = QDateTime::currentMSecsSinceEpoch();
        if(ct - lastTime > FRAME_MAX_SPLITETIMESPACE_SIZE) { // 丢弃超时的组包数据
            del();
        }
        lastTime = ct;

        auto itFd = datas.find(port);
        if(itFd != datas.end()) {
            auto itemWhole = itFd.value()->jionFrame(item);
            if(itemWhole) { // 重组分包数据成功
                if(itFd.value()->datas.size() <= 0) {
                    delete itFd.value(); // 释放内存
                    datas.remove(port);
                }
            }
            return itemWhole;
        }else {
            init(port, item);
            return nullptr;
        }
    }

    void init(quint16 port, FrameItem *item) {
        FrameItems *fis = new FrameItems(item);
        datas.insert(port, fis);
    }
};

class NetFrameDataReslicePrivate {
public:
    NetFrameDataReslicePrivate() {
        lastTime = QDateTime::currentMSecsSinceEpoch();
    }
    /// 上一次组包时间
    quint64 lastTime;

    /// 保存重组的数据
    /// quint32: ip
    /// quint16: port
    /// RecombinationDataSt: 从组信息
    QMap<quint32, PortsFrames*> m_dataRecoms;

    FrameItem *jionFrame(quint32 ip, quint16 port, FrameItem *item) {
        if(item->frameHead.usFrameN == 1) { // 未分包
            return item;
        }

        quint64 ct = QDateTime::currentMSecsSinceEpoch();
        if(ct - lastTime > FRAME_MAX_SPLITETIMESPACE_SIZE) { // 丢弃超时的组包数据
            del();
        }
        lastTime = ct;

        auto itFd = m_dataRecoms.find(ip);
        if(itFd != m_dataRecoms.end()) {
            auto itemWhole = itFd.value()->jionFrame(port, item);
            if(itemWhole) {
                if(itFd.value()->datas.size() <= 0) {
                    delete itFd.value();
                    m_dataRecoms.remove(ip);
                }
            }
            return itemWhole;
        }else {
            PortsFrames *pits = new PortsFrames(port, item);
            m_dataRecoms.insert(ip, pits);
            return nullptr;
        }
    }

    void del() {
        for(auto it = m_dataRecoms.begin(); it != m_dataRecoms.end(); it ++) {
            delete it.value();
        }
        m_dataRecoms.clear();
    }
};

class NetFrameDataReslice
{
public:
    NetFrameDataReslice();
    ~NetFrameDataReslice() {
        m_private.del();
    }

    ///
    /// \brief splitFrame: 数据分包
    /// \param data：要分包的数据
    /// \param frameMaxSize：分包大小
    /// \param frameType: 信息类型
    /// \param sendAdd: 发送地址
    /// \param recvAdd: 接收地址
    /// \param frameId:
    /// \param unRest: 不使用此参数
    /// \return
    ///
    QList<std::shared_ptr<QByteArray> > splitFrame(const QByteArray &data,
                                                   ushort frameType,
                                                   uchar condense1,
                                                   uchar condense2,
                                                   uint &frameId,
                                                   uint *unRest = nullptr,
                                                   bool bCut = true,
                                                   const uchar *cFrameId = nullptr,
                                                   int frameMaxSize = FRAME_MAX_SPLITE_SIZE);

    ///
    /// \brief joinFrame: 组包数据
    /// \param data：
    /// \param bBigLittleMode: true: 判断大小端
    /// \return
    ///
    QList<FrameItem *> joinFrame(quint32 ip, quint16 port, const QByteArray &data, bool bBigLittleMode = false);

private:
    NetFrameDataReslicePrivate m_private;

    ushort m_fEnd; // 帧尾标识
    // 数据缓冲区
    QByteArray m_bigData;
    quint64 m_lastTime;
    uint m_cutIdentif;
};

} // end namespace wkit

#endif // NETFRAMEDATARESLICE_H
