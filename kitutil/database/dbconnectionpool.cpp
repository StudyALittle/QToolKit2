#include "dbconnectionpool.h"
#include "database.h"
#include <QDir>
#include <QCoreApplication>

using namespace wkit;

DbConItem::DbConItem(const QString &name, const QString &connectionName, const QString &dbType, const DataBaseConParam &conPars) {
    m_dbType = dbType;
    m_threadId = getThreadID();
    m_useNum = 0;
    m_database = new QSqlDatabase();
    m_conParam = conPars;
    m_name = name;
    m_connectionName = connectionName;
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
#if 0
    auto funcCon = [=](const QString &conName) {
        bool bConName = QSqlDatabase::contains(conName);
        if (!bConName)
            *m_database = QSqlDatabase::addDatabase(m_dbType, conName);
        else
            *m_database = QSqlDatabase::database(conName);

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
    };

    // 连接名称：[连接池名+线程地址+连接名]
    QString conName = QString("%1_%2").arg(m_name).arg(QString::number(m_threadId));

    bool bConName = QSqlDatabase::contains(conName);
    if(m_useNum == 0 || !(m_database->isOpen()) || !bConName) { // 不存在连接名或者数据库未打开（打开失败），或者数据库连接名第一次使用
        funcCon(conName);
    } else {
        // 当前QSqlDatabase正在被使用，如果连接参数被修改，需要重新连接
        if (checkDbParamsIsChanged(m_conParam)) {
            funcCon(conName);
        }
    }
#endif

    auto funcSetParam = [](const QString &dbType, QSqlDatabase *db, const DataBaseConParam &conParam) {
        if(dbType == "QSQLITE") {
            db->setDatabaseName(conParam.dbPath);
            db->setUserName(conParam.userName);
            db->setPassword(conParam.password);
        } else {
            db->setHostName(conParam.ip);
            db->setPort(conParam.port);
            db->setDatabaseName(conParam.databaseName);
            db->setUserName(conParam.userName);
            db->setPassword(conParam.password);
        }
    };

    // 连接名称：[连接池名+线程地址+连接名]
    QString conName = QString("%1_%2").arg(m_name).arg(QString::number(m_threadId)).arg(m_connectionName);
    bool bConName = QSqlDatabase::contains(conName);
    if (!bConName)
        *m_database = QSqlDatabase::addDatabase(m_dbType, conName);
    else
        *m_database = QSqlDatabase::database(conName);

    bool bOpen = m_database->isOpen();
    // 当前QSqlDatabase正在被使用，如果连接参数被修改，重新设置连接参数
    if (checkDbParamsIsChanged(m_conParam)) {
        funcSetParam(m_dbType, m_database, m_conParam);
        // 重新连接
        bOpen = m_database->open();
    }

    if (!bOpen) {
        m_database->open();
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
        QString conName = m_database->connectionName();
        if(QSqlDatabase::contains(conName))
            QSqlDatabase::removeDatabase(conName);
    }
    return --m_useNum;
}

//检查连接参数是否发生变化
bool DbConItem::checkDbParamsIsChanged(const DataBaseConParam &params)
{
    if(m_database->hostName() != params.ip ||
            m_database->port() != params.port ||
            m_database->userName() != params.userName ||
            m_database->password() != params.password ||
            m_database->databaseName() != params.databaseName) {
        return true;
    }
    return false;
}

///////////////////////////////////////////
QMap<QString, DbConnectionPool::Ptr> DbConnectionPool::s_dbCons;

DbConnectionPool::DbConnectionPool(const QString &name):
    m_name(name)
{
}

///
/// \brief initConPools: 初始化多个连接
/// \param conPools
///
void DbConnectionPool::initConPools(const QMap<QString, DataBaseConParam> &conPools)
{
    for(auto it = conPools.begin(); it != conPools.end(); it ++) {
        auto ptr = std::make_shared<DbConnectionPool>(it.key());
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
QSqlDatabase DbConnectionPool::getDatabase(const QString &type, const QString &connectName)
{
    return getDatabase(type, DbConItem::getThreadID(), connectName);
}
///
/// \brief getDatabase: 获取数据库(每个线程获取的连接一样)
/// \param threadId: 线程ID
/// \return
///
QSqlDatabase DbConnectionPool::getDatabase(const QString &type, qlonglong threadId, const QString &connectName)
{
    DbConItem::Ptr item = getConItem(threadId, connectName);
    if(!item) {
        item = std::make_shared<DbConItem>(m_name, connectName, type, m_conParam);
        QMutexLocker locker(&m_mutexCon);
        if (!m_connects.contains(threadId)) {
            m_connects.insert(threadId, QMap<QString, DbConItem::Ptr>());
        }
        m_connects[threadId].insert(connectName, item);
    }
    return item->database();
}

///
/// \brief revertDatabase: 归还数据库
/// \param database
///
void DbConnectionPool::revertDatabase(const QString &connectName)
{
    return revertDatabase(DbConItem::getThreadID(), connectName);
}
void DbConnectionPool::revertDatabase(qlonglong threadId, const QString &connectName)
{
    DbConItem::Ptr item = getConItem(threadId, connectName);
    if(!item)
        return;
    if(item->revert() == 0) { // 没有线程在使用连接，移除相关信息
        QMutexLocker locker(&m_mutexCon);
        if(m_connects.contains(threadId) && m_connects.value(threadId).contains(connectName)) {
            m_connects[threadId].remove(connectName);
            if (m_connects.value(threadId).size() <= 0)
                m_connects.remove(threadId);
        }
    }
}

/// 获取线程的连接信息
DbConItem::Ptr DbConnectionPool::getConItem(const QString &connectName)
{
    return getConItem(DbConItem::getThreadID(), connectName);
}
DbConItem::Ptr DbConnectionPool::getConItem(qlonglong threadId, const QString &connectName)
{
    QMutexLocker locker(&m_mutexCon);
    if(m_connects.contains(threadId) && m_connects.value(threadId).contains(connectName))
        return m_connects.value(threadId).value(connectName);
    return nullptr;
}
