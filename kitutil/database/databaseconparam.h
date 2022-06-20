#ifndef DATABASECONPARAM_H
#define DATABASECONPARAM_H

#include <QString>
#include "kitutil_global.h"

namespace wkit {

///
/// \brief The DataBaseConParam class: 数据库连接参数配置
///
class KITUTIL_EXPORT DataBaseConParam {
public:
    static DataBaseConParam &instance();

    DataBaseConParam();
    DataBaseConParam(const DataBaseConParam &that);
    DataBaseConParam(const QString &dbPath);
    DataBaseConParam(const QString &ip, int port,
                      const QString &databaseName,
                      const QString &userName,
                      const QString &password);

    void setParam(const QString &ip, int port,
                  const QString &databaseName,
                  const QString &userName,
                  const QString &password);
    void setDbPath(const QString &dbPath);

    DataBaseConParam& operator = (const DataBaseConParam &that);
public:
    QString dbPath; // QSQLITE 时需要设置数据库路径
    QString ip;
    int port;
    QString databaseName;
    QString userName;
    QString password;
};

} // end namespace
#endif // DATABASECONPARAM_H
