#include "databaseconparam.h"

using namespace wkit;

DataBaseConParam &DataBaseConParam::instance()
{
    static DataBaseConParam s_instance;
    return s_instance;
}

DataBaseConParam::DataBaseConParam() {}
DataBaseConParam::DataBaseConParam(const DataBaseConParam &that)
{
    setParam(that.ip, that.port, that.databaseName, that.userName, that.password);
}
//数据库连接配置单例
DataBaseConParam::DataBaseConParam(const QString &ip, int port,
                                             const QString &databaseName,
                                             const QString &userName,
                                             const QString &password)
{
    setParam(ip, port, databaseName, userName, password);
}

void DataBaseConParam::setParam(const QString &ip, int port,
              const QString &databaseName,
              const QString &userName,
              const QString &password)
{
    this->ip = ip;
    this->port = port;
    this->databaseName = databaseName;
    this->userName = userName;
    this->password = password;
}

DataBaseConParam& DataBaseConParam::operator = (const DataBaseConParam &that)
{
    if(&that != this)
        setParam(ip, port, databaseName, userName, password);
    return *this;
}
