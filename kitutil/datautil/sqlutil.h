#ifndef SQLUTIL_H
#define SQLUTIL_H

/// SqlUtil: sql语句组装工具类
/// SqlPackUtil: sql语句组装工具类

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QPair>

namespace wkit {

// 查询语句
#define WK_MYSQL_SELECT "select %1 from %2"
// 查询数据条数
#define WK_MYSQL_COUNT  "select count(1) from %2"
// 分页语句
#define WK_MYSQL_PAGES  " limit ?, ?"
// 查询时间范围
#define WK_MYSQL_TIMESPACE  " between ? and ?"

class SqlUtil
{
public:
    SqlUtil();
    static QString selectStr(const QStringList &fileds);
    static QString selectStr(const QStringList &aFileds, const QStringList &bFileds);
    static void selectStr(const QStringList &fileds, const QString &tableAlias, QString &str);

    static QVariantList whereStr(const QVariantMap &params, QString &strWhereSql, QString str, QString str2 = "");
    static QString whereJoinStr(const QString &aJoinFiledName, const QString &bJoinFiledName);
    static QVariantList whereStr(const QVariantMap &aParams, const QVariantMap &bParams, QString &strWhereSql, QString str);
    static QVariantList whereTimeSpaceAStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                                 const QDateTime &edDateTime, QString &strWhereSql);
    static QVariantList whereTimeSpaceBStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                                 const QDateTime &edDateTime, QString &strWhereSql);
    static QVariantList whereStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                                 const QDateTime &edDateTime, QString &strWhereSql, QString str = "");
    static QVariantList wherePageStr(int pageIndex, int pageSize, QString &strWhereSql);

    static QPair<QString, QVariantList> updateSet(const QVariantMap &updateDatas);
    static QPair<QString, QVariantList> insertStr(const QVariantMap &updateDatas);
};

class SqlPackUtilPrivate {
public:
    // 表A名称
    QString tableAName;
    // 表B名称
    QString tableBName;
    // 第几页（查询分页参数）
    int pageIndex;
    // 每页数量（查询分页参数）
    int pageSize;
    // 查询时间范围的key
    QString filedDateTime;
    // 时间范围
    QDateTime bgDateTime;
    QDateTime edDateTime;

    // a、b表查询字段
    QStringList aSelectFileds;
    QStringList bSelectFileds;

    // a、b表等于条件参数
    QVariantMap aWhereEqual;
    QVariantMap bWhereEqual;
    // a、b表不等于条件参数
    QVariantMap aWhereNotEqual;
    QVariantMap bWhereNotEqual;

    // 连表字段名称
    QString aJoinFiledName;
    QString bJoinFiledName;

    // 插入数据字段
    QVariantMap insertData;
    // 更新数据字段
    QVariantMap updateData;

    // 保存绑定结果的参数
    QVariantList bindParams;
};

class SqlPackUtil
{
public:
    enum Model {
        SingleTable, // 单表操作
        MultiTable   // 多表操作
    };
    enum DateTimeSpaceTable {
        DefaultTable,
        A_Table,
        B_Table
    };
public:
    SqlPackUtil(Model model = SingleTable);
    SqlPackUtil(const QString &tableAName, const QString &tableBName = QString(), Model model = SingleTable);

    void setTableAName(const QString &tableAName);
    void setTableBName(const QString &tableBName);

    ///
    /// \brief setSelectFileds: 设置查询字段
    /// \param aSelectFileds：a表查询字段
    /// \param bSelectFileds：b表查询字段
    ///
    void setSelectFileds(const QStringList &aSelectFileds, const QStringList &bSelectFileds = QStringList());
    /// 设置查询分页参数
    void setSelectPageParam(int pageIndex, int pageSize);
    /// 设置查询时间范围: dateTimeFileName: 查询字段名称
    void setSelectDateTimeSpace(const QString &dateTimeFileName, const QDateTime &bgDateTime,
                                const QDateTime &edDateTime, DateTimeSpaceTable timeSpaceTable = DefaultTable);

    /// 设置where等于条件参数: key:字段名 value: 字段值
    void setWhereEqual(const QVariantMap &aEqualParams, const QVariantMap &bEqualParams = QVariantMap());
    /// 设置where不等于条件参数: key:字段名 value: 字段值
    void setWhereNotEqual(const QVariantMap &aNotEqualParams, const QVariantMap &bNotEqualParams = QVariantMap());

    /// 设置连表的等于关联参数
    void setJoinFileds(const QString &aFiledName, const QString &bFiledName);

    /// 设置插入数据 key: 字段名 value: 值
    void setInsertDatas(const QVariantMap &insertDatas);
    /// 设置更新数据
    void setUpdateDatas(const QVariantMap &updateDatas);

    /// 获取组装结果的sql
    /// 查询语句
    QString sqlSelect();
    /// count语句
    QString sqlCount();
    /// 插入语句
    QString sqlInsert();
    /// 更新语句
    QString sqlUpdate();
    /// 获取绑定参数
    QVariantList bindParams();

protected:
    QPair<QString, QVariantList> conditionSql();
private:
    // 单表、多表操作
    Model m_model;
    DateTimeSpaceTable m_timeSpaceTable;
    SqlPackUtilPrivate m_p;
};

} // end namespace

#endif // SQLUTIL_H
