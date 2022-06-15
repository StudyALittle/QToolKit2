#ifndef APICLIENTJSONBASE_H
#define APICLIENTJSONBASE_H

///
/// \brief The ApiClientJsonBase class: json通信交互客户端基类
///

#include "netstruct/netparam.h"
#include "netstruct/netpagest.h"
#include "datautil/mapjsonutil.h"
#include "networkclient.h"

namespace wkit {

/// 客户端同步请求数据默认超时时间 ms
#define CLIENT_ASYNC_TIME_SPACE 5000

/// T_ONEDATA: 一条数据信息
template<typename T_ONEDATA>
class ApiClientJsonBase
{
public:
    ApiClientJsonBase(NetworkClient *client = nullptr): m_client(client) {
        m_fType = DataFrameType::FT_JSON;
        m_timeout = CLIENT_ASYNC_TIME_SPACE;
    }

    void SetClient(NetworkClient *client) {
        m_client = client;
    }

    ResponseMapJson::Ptr GetRMapJson(const QString &api, const QVariant &data = QVariant()) {
        auto rMap = RequestMapJson::packData(api, data);
        auto pData = m_client->sendData(MapJsonUtil::variantMapToJson(rMap), m_fType, m_timeout);
        auto ptr = ResponseMapJson::unpackData(MapJsonUtil::byteArrayToVariantMap(*pData));
        m_errCode = ptr->errCode;
        return ptr;
    }
    ResponsePageMapJson::Ptr GetRPageMapJson(const QString &api, int pageIndex, int pageSize, const QVariant &data = QVariant()) {
        auto rMap = RequestPageMapJson::packData(api, pageIndex, pageSize, data);
        auto pData = m_client->sendData(MapJsonUtil::variantMapToJson(rMap), m_fType, m_timeout);
        auto ptr = ResponsePageMapJson::unpackPageData(MapJsonUtil::byteArrayToVariantMap(*pData));
        m_errCode = ptr->errCode;
        return ptr;
    }

    ///
    /// \brief count: 获取int数据
    /// \param api
    /// \param data
    /// \return
    ///
    int IntRequest(const QString &api, const QVariant &data = QVariant()) {
        ResponseMapJson::Ptr rJson = GetRMapJson(api, data);
        return rJson->data.toInt();
    }
    /// 获取double数据
    double DoubleRequest(const QString &api, const QVariant &data = QVariant()) {
        ResponseMapJson::Ptr rJson = GetRMapJson(api, data);
        return rJson->data.toDouble();
    }

    ///
    /// \brief DataRequest: 获取数据
    /// \param api
    /// \param data
    /// \return
    ///
    std::shared_ptr<T_ONEDATA> DataRequest(const QString &api, const QVariant &data = QVariant()) {
        ResponseMapJson::Ptr rJson = GetRMapJson(api, data);
        return vMapToDataOne(rJson->data.toMap());
    }

    ///
    /// \brief PageRequest: 查询分页参数
    /// \param api
    /// \param data
    /// \return
    ///
    typename NetPageSt<T_ONEDATA>::Ptr
    PageRequest(const QString &api, int pageIndex, int pageSize, const QVariant &data = QVariant()) {
        ResponsePageMapJson::Ptr rJson = GetRPageMapJson(api, pageIndex, pageSize, data);

        auto result = std::make_shared<NetPageSt<T_ONEDATA> >();
        result->totalSize = rJson->totalSize;
        result->pageIndex = rJson->pageIndex;
        result->pageSize = rJson->pageSize;
        for(const auto &vo: rJson->listData) {
            result->datas.datas.append(vMapToDataOne(vo.toMap()));
        }
        return result;
    }

protected:
    virtual std::shared_ptr<T_ONEDATA> vMapToDataOne(const QVariantMap &mapData) = 0;

private:
    /// 保存最新的错误
    NetResultCode m_errCode;
    /// 网络通信客户端
    NetworkClient *m_client;
    /// 数据类型
    DataFrameType m_fType;
    /// 请求数据超时时间 ms
    int m_timeout;
};

}

#endif // APICLIENTJSONBASE_H
