#ifndef DATABASE_H
#define DATABASE_H

//数据库操作接口
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>
#include <QSqlQueryModel>
#include <memory>
#include "dbconnectionpool.h"
#include "kitutil_global.h"

namespace wkit {

///
/// \brief The DataBase class: 数据库操作对象
///
class KITUTIL_EXPORT DataBase
{
public:
    enum BindType{
        Bind = 0, //执行绑定语句
        NotBind   //不执行绑定语句
    };
public:
    DataBase(const QString &conPoolName);
    //析构函数，关闭数据库连接及移除连接名
    virtual ~DataBase();

    //获取数据所有表名
    QStringList tables(QSql::TableType type = QSql::Tables) const;

    //判断数据库是否打开
    bool isOpen();
    //关闭连接
    void close();

    //返回当前语句执行的错误
    QSqlError currentError();
    //返回最后一次执行的错误
    QSqlError lastError();
    //返回最后一次插入数据的ID
    QVariant lastInsertId() const;

    // 打开数据库
    bool openDatabase();

    //查询数据QVariantList QVariantMap
    std::shared_ptr<QList<QMap<QString, QVariant>> > queryList(const QString &sql);
    //查询数据QVariantList QVariantMap(绑定参数)
    std::shared_ptr<QList<QMap<QString, QVariant>> > queryList(const QString &sql, const QList<QVariant> &datas);
    //查询数据QVariantList QVariantMap(type == Bind 需要传递绑定参数)
    std::shared_ptr<QList<QMap<QString, QVariant>> > queryList(const QString &sql, BindType type, const QList<QVariant> &datas = QList<QVariant>());

    //查询数据
    std::shared_ptr<QSqlQuery> query(const QString &sql);
    //查询数据(绑定参数)
    std::shared_ptr<QSqlQuery> query(const QString &sql, const QList<QVariant> &datas);
    //查询数据(type == Bind 需要传递绑定参数)
    std::shared_ptr<QSqlQuery> query(const QString &sql, BindType type, const QList<QVariant> &datas = QList<QVariant>());

    //查询数据(type == Bind 需要传递绑定参数)
    std::shared_ptr<QSqlQueryModel> queryModel(const QString& sql, BindType type, const QList<QVariant> &datas = QList<QVariant>());

    //查询数据条数(返回-1表示语句执行错误)
    long queryCount(const QString& sql);
    //查询数据条数(返回-1表示语句执行错误)(绑定参数)
    long queryCount(const QString& sql, const QList<QVariant> &datas);
    //查询数据条数(返回-1表示语句执行错误)(type == Bind 需要传递绑定参数)
    long queryCount(const QString& sql, BindType type, const QList<QVariant> &datas = QList<QVariant>());

    //执行数据
    bool exec(const QString& sql);
    //执行数据(绑定参数)
    bool exec(const QString& sql, const QList<QVariant> &datas);

    //执行数据（返回成功条数）(bLastID == true 可查询最近插入数据ID)
    int execAffectedRows(const QString& sql, bool bLastID = false);
    //执行数据（返回成功条数）(绑定参数)(bLastID == true 可查询最近插入数据ID)
    int execAffectedRows(const QString& sql, const QList<QVariant> &datas, bool bLastID = false);
    //执行数据（返回成功条数）(type == Bind 需要传递绑定参数)(bLastID == true 可查询最近插入数据ID)
    int execAffectedRows(const QString& sql, BindType type, bool bLastID = false, const QList<QVariant> &datas = QList<QVariant>());

    //准备批量插入数据语句
    std::shared_ptr<QSqlQuery> prepare(const QString &sql);
    //批量插入数据执行（返回成功条数）
    int execBatch(std::shared_ptr<QSqlQuery> query);
private:
    bool queryExec(QSqlQuery &sqlQuery, const QString& sql);
    bool queryExecBatch(QSqlQuery& sqlQuery);
    bool queryExec(QSqlQuery& sqlQuery, bool bBatch = true, const QString& sql = "");
    bool queryExecBind(QSqlQuery& sqlQuery, const QList<QVariant> &datas, const QString& sql = "");
private:
    QSqlDatabase *m_database;
    QSqlError *m_sqlError;          // 当前错误
    int m_lastInsertId;             // 最后一次插入数据的ID
    QString m_conPoolName;          // 连接池名称
};

} // end namespace
#endif // DATABASE_H
