#ifndef NETFRAME_H
#define NETFRAME_H

#include <QByteArray>
#include <memory>
#include "datautil/datautil.h"

namespace wkit {

#pragma pack(1)
/** 数据帧信息头结构 */
struct NetFrameHead
{
    unsigned short usFrameH;      /** 帧头标识**/
    unsigned char  condense[2];   /** 压缩[0]: 数据是否压缩; [1]: 压缩算法 **/
    unsigned int   unFrameLen;    /** 帧长度（头+数据长度）**/
    unsigned short usFrameN;      /** 数据包数**/
    unsigned short usFrameNumber; /** 包序号**/
    unsigned short usFrameID;     /** 信息标识(标识数据类型) **/
    unsigned char  gcInfo[8];     /** 帧标识号（帧标识ID）**/
    unsigned short usRest;        /** 大小端**/
    unsigned int   unGroupLen;    /** 帧体长度(携带数据的长度)**/
    unsigned int   unFrameOffset; /** 偏移量**/

    NetFrameHead() {
        memset(gcInfo, 0, 8);
        usRest = DataUtil::isBigEndien();
    }
};
#pragma pack()

/// 网络协议类型
enum NetworkTreatyType {
    UDP = 0, //目前只支持 UDP
    TCP
};

enum DataFrameType {
    FT_MAnd         = 0x0FF0,    // 无效类型 FT_MAnd & 其它数据类型 == FT_SyncData 表示是同步数据
    FT_SyncData     = 0x0110,    // 同步数据 0x1FF1 0x2FF1 0x~FF~ 都会同步处理接收数据
    FT_SyncYncData  = 0x0220,    // 同步/异步数据

    FT_JSON         = 0x0221,    // 携带json格式数据
};

typedef struct Session{
    quint32 ip;
    quint16 port;
    std::shared_ptr<NetFrameHead> head;
} Session;

} // end namespace wkit

#endif // NETFRAME_H
