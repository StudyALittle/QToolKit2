#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QUuid>
#include <QStandardPaths>
#include <QDir>
#include <QThread>
#include <QCoreApplication>

#pragma execution_character_set("utf-8")

using namespace wkit;

DataBase::DataBase(const QString &conPoolName)
{
    m_conPoolName = conPoolName;
    m_database = new QSqlDatabase;
    m_sqlError = new QSqlError;
}

DataBase::~DataBase(){
    close();
}

//
QStringList DataBase::tables(QSql::TableType type/* = QSql::Tables*/) const
{
    return m_database->tables(type);
}

void DataBase::close() {
    delete m_sqlError;
    delete m_database;
    m_database = nullptr;
    m_sqlError = nullptr;
    auto conPool = DbConnectionPool::getConPool(m_conPoolName);
    if(conPool)
        conPool->revertDatabase();
}

bool DataBase::openDatabase()
{
#if defined(SQL_DRIVE_SQLITE)
    Q_UNUSED(connectionName)
    *m_database = DbConnectionPool::instance().getDatabase("QSQLITE");
    return m_database->isOpen();
#else
    *m_database = DbConnectionPool::getConPool(m_conPoolName)->getDatabase("QMYSQL");
    return m_database->isOpen();
#endif
}

//执行查询
std::shared_ptr<QSqlQuery> DataBase::query(const QString& sql) {
    return query(sql, NotBind);
}

//查询数据(绑定参数)
std::shared_ptr<QSqlQuery> DataBase::query(const QString &sql, const QList<QVariant> &datas){
    return query(sql, Bind, datas);
}
//查询数据(type == Bind 需要传递绑定参数)
std::shared_ptr<QSqlQuery> DataBase::query(const QString &sql, BindType type, const QList<QVariant> &datas){
    auto sqlQuery = std::make_shared<QSqlQuery>(*m_database);
    if (type == NotBind && !queryExec(*sqlQuery, sql)){ /*error*/ }
    else if (type == Bind && !queryExecBind(*sqlQuery, datas, sql)){ /*error*/ }
    return sqlQuery;
}

//查询数据QVariantList QVariantMap
std::shared_ptr<QList<QMap<QString, QVariant>>> DataBase::queryList(const QString &sql){
    return queryList(sql, NotBind);
}
//查询数据QVariantList QVariantMap(绑定参数)
std::shared_ptr<QList<QMap<QString, QVariant>> > DataBase::queryList(const QString &sql, const QList<QVariant> &datas){
    return queryList(sql, Bind, datas);
}
//查询数据QVariantList QVariantMap(type == Bind 需要传递绑定参数)
std::shared_ptr<QList<QMap<QString, QVariant>> > DataBase::queryList(const QString &sql, BindType type, const QList<QVariant> &datas){
    auto result
            = std::make_shared<QList<QMap<QString, QVariant> > >();

    std::shared_ptr<QSqlQueryModel> sqlQuery = queryModel(sql, type, datas);

    int count = sqlQuery->rowCount();
    for (int i = 0; i < count; i++)
    {
        auto rec = sqlQuery->record(i);
        QMap<QString, QVariant> mapOne;

        int countRec = rec.count();
        for(int j = 0; j < countRec; j ++){
            mapOne.insert(rec.field(j).name(), rec.value(j));
        }
        result->append(mapOne);
    }

    return result;
}

std::shared_ptr<QSqlQueryModel> DataBase::queryModel(const QString& sql, BindType type, const QList<QVariant> &datas) {
    auto model = std::make_shared<QSqlQueryModel>();
    QSqlQuery query(*m_database);
    if (type == NotBind && !queryExec(query, sql)){
        return model;
    }else if (type == Bind && !queryExecBind(query, datas, sql)){
        return model;
    }

    model->setQuery(query);
    while (model->canFetchMore()) {
        model->fetchMore();
    }
    return model;
}

//查询数据条数
long DataBase::queryCount(const QString& sql) {
    return queryCount(sql, NotBind);
}

//查询数据条数(返回-1表示语句执行错误)(绑定参数)
long DataBase::queryCount(const QString& sql, const QList<QVariant> &datas)
{
    return queryCount(sql, Bind, datas);
}
//查询数据条数(返回-1表示语句执行错误)(type == Bind 需要传递绑定参数)
long DataBase::queryCount(const QString& sql, BindType type, const QList<QVariant> &datas)
{
    long count = -1;
    QSqlQuery query(*m_database);
    if (type == Bind && !queryExecBind(query, datas, sql))
    {
        return count;
    }else if(type == NotBind && !queryExec(query, sql)){
        return count;
    }
    if (query.next()) {
        count = query.value(0).toInt();
    }
    return count;
}

//执行数据
bool DataBase::exec(const QString& sql) {
    QSqlQuery query(*m_database);
    return queryExec(query, sql);
}
bool DataBase::exec(const QString& sql, const QList<QVariant> &datas)
{
    QSqlQuery query(*m_database);
    return queryExecBind(query, datas, sql);
}

//执行数据（返回成功条数）
int DataBase::execAffectedRows(const QString& sql, bool bLastID) {
    return execAffectedRows(sql, NotBind, bLastID);
}
//执行数据（返回成功条数）(绑定参数)
int DataBase::execAffectedRows(const QString& sql, const QList<QVariant> &datas, bool bLastID){
    return execAffectedRows(sql, Bind, bLastID, datas);
}
//执行数据（返回成功条数）(type == Bind 需要传递绑定参数)
int DataBase::execAffectedRows(const QString& sql, BindType type, bool bLastID, const QList<QVariant> &datas){
    QSqlQuery query(*m_database);
    if (NotBind == type && !queryExec(query, sql)) return -1; //执行错误
    else if (Bind == type && !queryExecBind(query, datas, sql)) return -1; //执行错误
    if(bLastID){
        m_lastInsertId = query.lastInsertId().toInt();
    }
    return query.numRowsAffected();
}

//准备批量插入数据语句
std::shared_ptr<QSqlQuery> DataBase::prepare(const QString& sql) {
    std::shared_ptr<QSqlQuery> query = std::make_shared<QSqlQuery>(*m_database);
    query->prepare(sql);
    return query;
}
//批量插入数据执行（返回成功条数）
int DataBase::execBatch(std::shared_ptr<QSqlQuery> query) {
    if (!queryExecBatch(*query)) return -1; //执行错误
    return query->numRowsAffected();
}

//判断数据库是否打开
bool DataBase::isOpen() {
    return m_database->isOpen();
}

//返回当前语句执行的错误
QSqlError DataBase::currentError() {
    return *m_sqlError;
}

//返回执行的错误
QSqlError DataBase::lastError() {
    return m_database->lastError();
}

//返回最后一次插入数据的ID
QVariant DataBase::lastInsertId() const {
    return m_lastInsertId;
}

bool DataBase::queryExec(QSqlQuery& sqlQuery, const QString& sql) {
    return queryExec(sqlQuery, false, sql);
}

bool DataBase::queryExecBatch(QSqlQuery& sqlQuery) {
    return queryExec(sqlQuery);
}

bool DataBase::queryExec(QSqlQuery& sqlQuery, bool bBatch, const QString& sql) {
    bool result = false;
    if (!bBatch) result = sqlQuery.exec(sql);
    else result = sqlQuery.execBatch();
    //QString strCode = sqlQuery.lastError().nativeErrorCode();
    if (!result && ("2006" == sqlQuery.lastError().nativeErrorCode()
        || "2013" == sqlQuery.lastError().nativeErrorCode())) //连接错误,执行一次重连(连接可能被数据主动断开)
    {
        QString connectName = m_database->connectionName();
        if (connectName == "")
        {
            qlonglong id = reinterpret_cast<qlonglong>(QThread::currentThread());
            connectName = QString("QSQLDB_%1").arg(id);
        }
        //*m_database = connectAndOpen(connectName);
        if (m_database->open()) {
            sqlQuery = QSqlQuery(*m_database);
            //连接成功重新执行语句(目前不支持绑定语句)
            if (!bBatch) result = sqlQuery.exec(sql);
            //else result = sqlQuery.execBatch();
        }
    }
    if (!result) {
        *m_sqlError = sqlQuery.lastError();
    }
    else {
        *m_sqlError = QSqlError();
    }
    return result;
}

bool DataBase::queryExecBind(QSqlQuery& sqlQuery, const QList<QVariant> &datas,
                                    const QString& sql)
{
    bool result = false;
    ///////////////////////////////
    if (!sqlQuery.prepare(sql)) {
         *m_sqlError = sqlQuery.lastError();
         return result;
    }
    foreach (const QVariant &data, datas) {
        sqlQuery.addBindValue(data);
    }
    result = sqlQuery.exec();
    ///////////////////////////////

    if (!result && ("2006" == sqlQuery.lastError().nativeErrorCode()
        || "2013" == sqlQuery.lastError().nativeErrorCode())) //连接错误,执行一次重连(连接可能被数据主动断开)
    {
        QString connectName = m_database->connectionName();
        if (connectName == "")
        {
            qlonglong id = reinterpret_cast<qlonglong>(QThread::currentThread());
            connectName = QString("QSQLDB_%1").arg(id);
        }
        //*m_database = connectAndOpen(connectName);
        if (m_database->open()) {
            sqlQuery = QSqlQuery(*m_database);
            //连接成功重新执行语句
            ///////////////////////////////
            if (!sqlQuery.prepare(sql)) {
                 *m_sqlError = sqlQuery.lastError();
                 return result;
            }
            foreach (const QVariant &data, datas) {
                sqlQuery.addBindValue(data);
            }
            result = sqlQuery.exec();
            ///////////////////////////////
        }
    }
    if (!result) {
        *m_sqlError = sqlQuery.lastError();
    }
    else {
        *m_sqlError = QSqlError();
    }
    return result;
}
