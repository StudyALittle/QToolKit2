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
DataBaseConParam::DataBaseConParam(const QString &dbPath)
{
    setDbPath(dbPath);
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
void DataBaseConParam::setDbPath(const QString &dbPath)
{
    this->dbPath = dbPath;
}

DataBaseConParam& DataBaseConParam::operator = (const DataBaseConParam &that)
{
    if(&that != this) {
        setParam(that.ip, that.port, that.databaseName, that.userName, that.password);
        setDbPath(that.dbPath);
    }
    return *this;
}
