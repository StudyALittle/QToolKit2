#ifndef APISERVERJSONBASE_H
#define APISERVERJSONBASE_H

#include "netstruct/netparam.h"
#include "netstruct/netpagest.h"
#include "datautil/mapjsonutil.h"

/// 打包发送数据

namespace wkit {

template<typename T_ONEDATA>
class ApiServerJsonBase
{
public:
    ApiServerJsonBase();

    /// 打包数据
    QVariantMap PackData(const QString &_api, const QVariant &_data,
                        const QString &_error, NetResultCode errCode) {
        return ResponseMapJson::packData(_api, _data, _error, errCode);
    }

    /// 打包分页数据
    QVariantMap PackPageData(const QString &_api, int totalSize, int pageIndex, int pageSize,
                             const QVariantList &listData = QVariantList()) {
        return ResponsePageMapJson::packData(_api, totalSize, pageIndex, pageSize, listData);
    }

    /// 打包分页数据
    QVariantMap PackPageData(const QString &_api, int totalSize, int pageIndex, int pageSize,
                             const QList<T_ONEDATA> &datas) {
        QVariantList vlist;
        for(const auto &data: datas) {
            vlist.append(DataOneToVarianMap(data));
        }
        return PackPageData(_api, totalSize, pageIndex, pageSize, datas);
    }

    /// 打包错误数据
    QVariantMap PackErrorData(const QString &_api, NetResultCode errCode = NetResultCode::Error, const QString &_error = QString()) {
        return PackData(_api, QVariant(), _error, errCode);
    }

    /// 打包int类型数据
    QVariantMap PackIntData(const QString &_api, int data) {
        return PackData(_api, data, QString(), NetResultCode::Success);
    }

    /// 打包double类型数据
    QVariantMap PackDoubleData(const QString &_api, double data) {
        return PackData(_api, data, QString(), NetResultCode::Success);
    }

    /// 打包一条数据
    QVariantMap PackOneData(const QString &_api, const T_ONEDATA &data) {
        return PackData(_api, DataOneToVarianMap(data), QString(), NetResultCode::Success);
    }

protected:
    virtual QVariantMap DataOneToVarianMap(const T_ONEDATA &data) = 0;
};

}

#endif // APISERVERJSONBASE_H
