#ifndef NETPARAM_H
#define NETPARAM_H

#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include "ctypedefsyntax.h"
#include "globalutil.h"

enum NetResultCode {
    NetError = -5,      // 网络错误
    AnalysisError = -4, // 网络数据解析错误
    SendError = -3,     // 发送网络数据错误
    RecvError = -2,     // 接收网络数据错误
    Error = -1,         // 表示执行结果错误
    Success = 0         // 表示执行结果成功
};

///
/// \brief The PageParam struct : 分页数据
///
struct NetPageParam {
    bool bOk;
    int totalSize;
    int pageIndex;
    int pageSize;
    QVariantList listData;

    NetPageParam() {
        bOk = true;
        totalSize = 0;
        pageIndex = 0;
        pageSize = 0;
    }
    NetPageParam(int _totalSize, int _pageIndex, int _pageSize) {
        totalSize = _totalSize;
        pageIndex = _pageIndex;
        pageSize = _pageSize;
    }
    NetPageParam(const QByteArray &jsonData) {
        bOk = true;
        QVariantMap mapData = GlobalUtil::byteArrayToVariantMap(jsonData);
        jsonMapToThis(mapData);
    }
    NetPageParam(const QVariantMap &mapData) {
        jsonMapToThis(mapData);
    }

    void jsonMapToThis(const QVariantMap &mapData) {
        bOk = true;
        if(!mapData.contains("totalSize") || !mapData.contains("pageIndex") ||
                !mapData.contains("pageSize")) {
            bOk = false;
            return;
        }
        totalSize = mapData.value("totalSize").toInt();
        pageIndex = mapData.value("pageIndex").toInt();
        pageSize = mapData.value("pageSize").toInt();
        if(mapData.contains("listData")) {
            listData = mapData.value("listData").toList();
        }
    }

    void add(const QVariantMap &mapData) {
        listData.append(mapData);
    }

    QVariantMap toMap() const {
        QVariantMap dataMap;
        dataMap.insert("totalSize", totalSize);
        dataMap.insert("pageIndex", pageIndex);
        dataMap.insert("pageSize", pageSize);
        dataMap.insert("listData", listData);
        return dataMap;
    }
};

///
/// \brief The RequestJsonParam struct: 客户端调用后端接口输入参数
///
struct RequestJsonParam {
    bool bOk; // 数据解析结果
    QString apiA; // 接口A
    QString apiB; // 接口B
    QVariant params; // 输入参数 json 数据

    RequestJsonParam() { bOk = true; }
    RequestJsonParam(const QString &_apiA, const QString &_apiB, const QVariant &_params = QVariant()) {
        bOk = true;
        apiA = _apiA;
        apiB = _apiB;
        params = _params;
    }
    RequestJsonParam(const QByteArray &jsonData) {
        bOk = true;
        QVariantMap mapData = GlobalUtil::byteArrayToVariantMap(jsonData);
        if(!mapData.contains("apiA") || !mapData.contains("apiB")) {
            bOk = false;
            return;
        }
        apiA = mapData.value("apiA").toString();
        apiB = mapData.value("apiB").toString();
        if(mapData.contains("params")) {
            params = mapData.value("params");
        }
    }

    QVariantMap map() const {
        return params.toMap();
    }
    QVariantList list() const {
        return params.toList();
    }

    QByteArrayPtr toDataPtr() {
        QVariantMap dataMap;
        dataMap.insert("apiA", apiA);
        dataMap.insert("apiB", apiB);
        dataMap.insert("params", params);
        return std::make_shared<QByteArray>(GlobalUtil::variantMapToJson(dataMap));
    }
    void setMapData(const QVariantMap &mapData) {
        params = mapData;
    }
};
///
/// \brief The RequestJsonParam struct: 后端接口返回结果给前端参数
///
struct ResponseJsonParam {
    bool bOk;               // 数据解析结果
    int result;             // （bool, int）执行结果
    NetResultCode errCode;  // 执行错误编码
    QString error;          // 错误信息
    QVariant data;          // 返回数据
    QString apiB;           // 请求的接口传回去
    QString apiA;           // 请求的接口传回去

    ResponseJsonParam() { bOk = true; errCode = NetResultCode::Error; result = 0;}

    /// 设置返回参数
    ResponseJsonParam(NetResultCode _errCode, int _result, const QVariant &_data = QVariant(), const QString &_error = "") {
        bOk = true;
        result = _result;
        data = _data;
        error = _error;
        errCode = _errCode;
    }
    /// 设置返回参数
    ResponseJsonParam(NetResultCode _errCode, int _result, const QString &_error) {
        bOk = true;
        result = _result;
        error = _error;
        errCode = _errCode;
    }
    /// 解析json数据
    ResponseJsonParam(const QByteArray &jsonData) {
        bOk = true;
        QVariantMap mapData = GlobalUtil::byteArrayToVariantMap(jsonData);
        if(!mapData.contains("errCode")) {
            bOk = false;
            return;
        }
        errCode = static_cast<NetResultCode>(mapData.value("errCode").toInt());
        if(!mapData.contains("result")) {
            bOk = false;
            return;
        }
        result = mapData.value("result").toInt();

        if(mapData.contains("error"))
            error = mapData.value("error").toString();
        if(mapData.contains("data"))
            data = mapData.value("data");
        if(mapData.contains("apiA"))
            apiA = mapData.value("apiA").toString();
        if(mapData.contains("apiB"))
            apiB = mapData.value("apiB").toString();
    }

    void setApiName(const QString &_apiA, const QString &_apiB) {
        apiA = _apiA;
        apiB = _apiB;
    }

    QByteArrayPtr toDataPtr() {
        QVariantMap dataMap;
        dataMap.insert("result", result);
        dataMap.insert("data", data);
        dataMap.insert("error", error);
        dataMap.insert("errCode", errCode);
        dataMap.insert("apiA", apiA);
        dataMap.insert("apiB", apiB);
        return std::make_shared<QByteArray>(GlobalUtil::variantMapToJson(dataMap));
    }

    QVariantMap map() const {
        return data.toMap();
    }
    QVariantList list() const {
        return data.toList();
    }

    NetPageParam netPage() {
        return NetPageParam(map());
    }

    void setPageData(const NetPageParam &page) {
        data = page.toMap();
    }
    void setMapData(const QVariantMap &mapData) {
        data = mapData;
    }
};
#endif // NETPARAM_H
