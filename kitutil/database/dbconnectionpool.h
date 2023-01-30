#ifndef DBCONNECTIONPOOL_H
#define DBCONNECTIONPOOL_H

///
/// \brief The DbConnectionPool class: 数据库连接池
/// 每个线程的连接名字相同
///
///

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QThread>
#include <QMutex>
#include <QMap>
#include <memory>
#include "databaseconparam.h"
#include "kitutil_global.h"

namespace wkit {

class KITUTIL_EXPORT DbConItem
{
public:
    using Ptr = std::shared_ptr<DbConItem>;

    DbConItem(const QString &name, const QString &connectionName, const QString &dbType, const DataBaseConParam &conPars);
    ~DbConItem();

    static qlonglong getThreadID();

    /// 获取数据库（获取一次，使用次数加一）
    QSqlDatabase database();
    /// 归还使用
    int revert();

    //检查连接参数是否发生变化
    bool checkDbParamsIsChanged(const DataBaseConParam &params);
private:
    QString m_name;             // 连接池名称
    QString m_connectionName;   // 连接名称
    QString m_dbType;
    qlonglong m_threadId;       // 线程地址转ID
    int m_useNum;               // database 被使用次数
    QSqlDatabase *m_database;   // database 实例

    /// 连接参数
    DataBaseConParam m_conParam;
};

class KITUTIL_EXPORT DbConnectionPool
{
public:
    using Ptr = std::shared_ptr<DbConnectionPool>;

    /// 构造函数
    DbConnectionPool(const QString &name);

    ///
    /// \brief initConPools: 初始化多个连接池(每个连接池对应不同的数据库)
    /// \param conPools
    ///
    static void initConPools(const QMap<QString, DataBaseConParam> &conPools);

    ///
    /// \brief getConPool: 获取连接池
    /// \param name：连接池名字
    /// \return
    ///
    static DbConnectionPool::Ptr getConPool(const QString &name);

    ///
    /// \brief setConParam: 设置连接参数
    /// \param conPars
    ///
    void setConParam(const DataBaseConParam &conPars);

    ///
    /// \brief getDatabase: 获取数据库(每个线程获取的连接一样)
    /// \param type: 数据库类型
    /// \return
    ///
    QSqlDatabase getDatabase(const QString &type, const QString &connectName = "default");

    ///
    /// \brief getDatabase: 获取数据库(每个线程获取的连接一样)
    /// \param threadId: 线程ID （不同线程使用同一个连接名会抛异常，使用线程地址进行区分）
    /// \return
    ///
    QSqlDatabase getDatabase(const QString &type, qlonglong threadId, const QString &connectName = "default");

    ///
    /// \brief revertDatabase: 归还数据库
    ///
    void revertDatabase(const QString &connectName = "default");
    void revertDatabase(qlonglong threadId, const QString &connectName = "default");

protected:
    /// 获取线程的连接信息
    DbConItem::Ptr getConItem(const QString &connectName = "default");
    DbConItem::Ptr getConItem(qlonglong threadId, const QString &connectName = "default");
private:
    /// key： 线程ID； value：key:连接名 value：连接信息
    QMap<qlonglong, QMap<QString, DbConItem::Ptr> > m_connects;
    QMutex m_mutexCon;

    /// 连接池名称
    QString m_name;
    /// 连接参数
    DataBaseConParam m_conParam;

    /// 保存多个不同数据库的连接池
    static QMap<QString, DbConnectionPool::Ptr> s_dbCons;
};

} // end namespace
#endif // DBCONNECTIONPOOL_H
