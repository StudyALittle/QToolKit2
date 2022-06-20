#include "dbconnectionpool.h"
#include "database.h"
#include <QDir>
#include <QCoreApplication>

using namespace wkit;

DbConItem::DbConItem(const QString &dbType, const DataBaseConParam &conPars) {
    m_dbType = dbType;
    m_threadId = getThreadID();
    m_useNum = 0;
    m_database = new QSqlDatabase();
    m_conParam = conPars;
}
DbConItem::~DbConItem() {
    if(m_database && m_database->isOpen()) {
        m_database->close();
        delete m_database;
        m_database = nullptr;
    }
}

qlonglong DbConItem::getThreadID() {
    return reinterpret_cast<qlonglong>(QThread::currentThread());
}

/// 获取数据库（获取一次，使用次数加一）
QSqlDatabase DbConItem::database()
{
    if(m_useNum == 0) {
        QString conName = QString::number(m_threadId);
        *m_database = QSqlDatabase::addDatabase(m_dbType, conName);

        if(m_dbType == "QSQLITE") {
            m_database->setDatabaseName(m_conParam.dbPath);
            m_database->setUserName(m_conParam.userName);
            m_database->setPassword(m_conParam.password);
        } else {
            m_database->setHostName(m_conParam.ip);
            m_database->setPort(m_conParam.port);
            m_database->setDatabaseName(m_conParam.databaseName);
            m_database->setUserName(m_conParam.userName);
            m_database->setPassword(m_conParam.password);
        }

        if(!m_database->open()) {
            QThread::msleep(200);
            if(!m_database->open()) {
                /// open error
            }
        }
    }

    m_useNum ++;
    return *m_database;
}
/// 归还使用
int DbConItem::revert()
{
    if(m_useNum == 1) {
        // 移除连接
        if(m_database->isOpen())
            m_database->close();
        delete m_database;
        m_database = nullptr;
        // 移除连接名
        if(QSqlDatabase::contains(QString::number(m_threadId)))
            QSqlDatabase::removeDatabase(QString::number(m_threadId));
    }
    return --m_useNum;
}

///////////////////////////////////////////
QMap<QString, DbConnectionPool::Ptr> DbConnectionPool::s_dbCons;

DbConnectionPool::DbConnectionPool()
{
}

///
/// \brief initConPools: 初始化多个连接
/// \param conPools
///
void DbConnectionPool::initConPools(const QMap<QString, DataBaseConParam> &conPools)
{
    for(auto it = conPools.begin(); it != conPools.end(); it ++) {
        auto ptr = std::make_shared<DbConnectionPool>();
        ptr->setConParam(it.value());
        s_dbCons.insert(it.key(), ptr);
    }
}

///
/// \brief getConPool: 获取连接池
/// \param name：连接池名字
/// \return
///
DbConnectionPool::Ptr DbConnectionPool::getConPool(const QString &name)
{
    if(s_dbCons.contains(name))
        return s_dbCons.value(name);
    return nullptr;
}

///
/// \brief instance
/// \return
///
DbConnectionPool &DbConnectionPool::instance()
{
    static DbConnectionPool s_instance;
    return s_instance;
}

///
/// \brief setConParam: 设置连接参数
/// \param conPars
///
void DbConnectionPool::setConParam(const DataBaseConParam &conPars)
{
    m_conParam = conPars;
}

///
/// \brief getDatabase: 获取数据库(每个线程获取的连接一样)
/// \param type: 数据库类型
/// \return
///
QSqlDatabase DbConnectionPool::getDatabase(const QString &type)
{
    return getDatabase(type, DbConItem::getThreadID());
}
///
/// \brief getDatabase: 获取数据库(每个线程获取的连接一样)
/// \param threadId: 线程ID
/// \return
///
QSqlDatabase DbConnectionPool::getDatabase(const QString &type, qlonglong threadId)
{
    DbConItem::Ptr item = getConItem(threadId);
    if(!item) {
        item = std::make_shared<DbConItem>(type, m_conParam);
        QMutexLocker locker(&m_mutexCon);
        m_connects.insert(threadId, item);
    }
    return item->database();
}

///
/// \brief revertDatabase: 归还数据库
/// \param database
///
void DbConnectionPool::revertDatabase()
{
    return revertDatabase(DbConItem::getThreadID());
}
void DbConnectionPool::revertDatabase(qlonglong threadId)
{
    DbConItem::Ptr item = getConItem(threadId);
    if(!item)
        return;
    if(item->revert() == 0) { // 没有线程在使用连接，移除相关信息
        QMutexLocker locker(&m_mutexCon);
        if(m_connects.contains(threadId))
            m_connects.remove(threadId);
    }
}

/// 获取线程的连接信息
DbConItem::Ptr DbConnectionPool::getConItem()
{
    return getConItem(DbConItem::getThreadID());
}
DbConItem::Ptr DbConnectionPool::getConItem(qlonglong threadId)
{
    QMutexLocker locker(&m_mutexCon);
    if(m_connects.contains(threadId))
        return m_connects.value(threadId);
    return nullptr;
}
