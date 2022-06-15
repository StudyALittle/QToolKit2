#ifndef NETPARAM_H
#define NETPARAM_H

#include <QString>
#include <QByteArray>
#include <QVariantMap>
#include <QStringList>
#include "datautil/mapjsonutil.h"

namespace wkit {

enum NetResultCode {
    NetError = -5,      // 网络错误
    AnalysisError = -4, // 网络数据解析错误
    SendError = -3,     // 发送网络数据错误
    RecvError = -2,     // 接收网络数据错误
    Error = -1,         // 表示执行结果错误
    Success = 0         // 表示执行结果成功
};

/// 请求参数
struct RequestMapJson {
    using Ptr = std::shared_ptr<RequestMapJson>;

    QString api;    // 接口 格式 /aa/bb
    QVariant data;  // 输入参数 json格式
    QStringList apis; // 解析后的api

    /// 获取api
    QString apiAt(int index) {
        if(index < 0 || index >= apis.size())
            return QString();
        return apis.at(index);
    }

    /// 打包数据
    static QVariantMap packData(const QString &_api, const QVariant &_data = QVariant()) {
        QVariantMap dataMap;
        dataMap.insert("api", _api);
        dataMap.insert("data", _data);
        return dataMap;
    }

    /// 解析数据
    static RequestMapJson::Ptr unpackData(const QVariantMap &mapJson) {
        auto request = std::make_shared<RequestMapJson>();
        if(mapJson.contains("api")) {
            request->api = mapJson.value("api").toString();
            request->apis = request->api.split("/", QString::SkipEmptyParts);
        }
        if(mapJson.contains("data"))
            request->data = mapJson.value("data").toMap();
        return request;
    }
};

struct RequestPageMapJson: public RequestMapJson {
    using Ptr = std::shared_ptr<RequestPageMapJson>;

    int pageIndex;          // 第几页
    int pageSize;           // 每页数量
    QVariantMap pagePars; // 请求参数

    /// 打包分页数据
    static QVariantMap packData(const QString &_api, int pageIndex, int pageSize, const QVariant &_params = QVariantMap()) {
        QVariantMap pageMap;
        pageMap.insert("pageIndex", pageIndex);
        pageMap.insert("pageSize", pageSize);
        pageMap.insert("params", _params);
        return RequestMapJson::packData(_api, pageMap);
    }

    /// 解析分页数据
    static RequestPageMapJson::Ptr unpackPageData(const QVariantMap &mapJson) {
        auto req = RequestMapJson::unpackData(mapJson);

        auto request = std::make_shared<RequestPageMapJson>();
        request->api = req->api;
        request->apis = req->apis;

        QVariantMap vmap = req->data.toMap();
        if(vmap.contains("pageIndex"))
            request->pageIndex = vmap.value("pageIndex").toInt();
        if(vmap.contains("pageSize"))
            request->pageSize = vmap.value("pageSize").toInt();
        if(vmap.contains("params"))
            request->pagePars = vmap.value("params").toMap();
        return request;
    }
};

/// 返回参数
struct ResponseMapJson {
    using Ptr = std::shared_ptr<ResponseMapJson>;

    NetResultCode errCode;  // 执行错误编码
    QString error;          // 错误信息
    QString api;            // 调用的接口
    QStringList apis;       // 解析后的api
    QVariant data;          // 返回数据

    /// 获取api
    QString apiAt(int index) {
        if(index < 0 || index >= apis.size())
            return QString();
        return apis.at(index);
    }

    /// 打包数据（无数据返回）
    static QVariantMap packData(const QString &_api) {
        return ResponseMapJson::packData(_api, QVariant(), QString(), NetResultCode::Success);
    }

    /// 打包数据（有数据返回）
    static QVariantMap packData(const QString &_api, const QVariant &_data) {
        return ResponseMapJson::packData(_api, _data, QString(), NetResultCode::Success);
    }

    /// 打包数据（无数据返回，自定义错误）
    static QVariantMap packData(const QString &_api, NetResultCode errCod, const QString &_error = QString()) {
        return ResponseMapJson::packData(_api, QVariant(), _error, errCod);
    }

    /// 打包数据
    static QVariantMap packData(const QString &_api, const QVariant &_data,
                                const QString &_error, NetResultCode errCode) {
        QVariantMap dataMap;
        dataMap.insert("errCode", errCode);
        dataMap.insert("error", _error);
        dataMap.insert("api", _api);
        dataMap.insert("data", _data);
        return dataMap;
    }

    /// 解析数据
    static ResponseMapJson::Ptr unpackData(const QVariantMap &mapJson) {
        auto request = std::make_shared<ResponseMapJson>();
        if(mapJson.contains("api")) {
            request->api = mapJson.value("api").toString();
            request->apis = request->api.split("/", QString::SkipEmptyParts);
        }
        if(mapJson.contains("error"))
            request->error = mapJson.value("error").toString();
        if(mapJson.contains("errCode"))
            request->errCode = static_cast<NetResultCode>(mapJson.value("errCode").toInt());
        if(mapJson.contains("data"))
            request->data = mapJson.value("data").toMap();
        return request;
    }
};

struct ResponsePageMapJson: public ResponseMapJson {
    using Ptr = std::shared_ptr<ResponsePageMapJson>;

    int totalSize;
    int pageIndex;
    int pageSize;
    QVariantList listData;

    /// 打包分页数据
    static QVariantMap packData(const QString &_api, int totalSize, int pageIndex, int pageSize,
                                    const QVariantList &listData = QVariantList()) {
        QVariantMap pageMap;
        pageMap.insert("pageIndex", pageIndex);
        pageMap.insert("pageSize", pageSize);
        pageMap.insert("totalSize", totalSize);
        pageMap.insert("listData", listData);
        return ResponseMapJson::packData(_api, pageMap);
    }

    /// 解析分页数据
    static ResponsePageMapJson::Ptr unpackPageData(const QVariantMap &mapJson) {
        auto req = ResponseMapJson::unpackData(mapJson);

        auto request = std::make_shared<ResponsePageMapJson>();
        request->api = req->api;
        request->apis = req->apis;
        request->error = req->error;
        request->errCode = req->errCode;

        QVariantMap vmap = req->data.toMap();
        if(vmap.contains("pageIndex"))
            request->pageIndex = vmap.value("pageIndex").toInt();
        if(vmap.contains("pageSize"))
            request->pageSize = vmap.value("pageSize").toInt();
        if(vmap.contains("params"))
            request->totalSize = vmap.value("params").toInt();
        if(vmap.contains("listData"))
            request->listData = vmap.value("listData").toList();
        return request;
    }
};

} // end namespace
#endif // NETPARAM_H
