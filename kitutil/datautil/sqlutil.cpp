#include "sqlutil.h"

using namespace wkit;

#define SQP_ESCAPE_CHAR "`"

SqlUtil::SqlUtil()
{

}

QString SqlUtil::selectStr(const QStringList &fileds)
{
    QString str;
    selectStr(fileds, "", str);
    return str;
}
QString SqlUtil::selectStr(const QStringList &aFileds, const QStringList &bFileds)
{
    QString str;
    selectStr(aFileds, "a.", str);
    selectStr(bFileds, "b.", str);
    return str;
}
void SqlUtil::selectStr(const QStringList &fileds, const QString &tableAlias, QString &str)
{
    for(const QString &filed: fileds) {
        if(str != "") {
            str.append(",");
        }
        str.append(tableAlias);
        str.append(SQP_ESCAPE_CHAR);
        str.append(filed);
        str.append(SQP_ESCAPE_CHAR);
    }
}

QVariantList SqlUtil::whereStr(const QVariantMap &params, QString &strWhereSql, QString str, QString str2)
{
    QVariantList bindPars;
    for(auto it = params.begin(); it != params.end(); it ++) {
        if(strWhereSql != "") {
            strWhereSql.append(" and ");
        }
        // `xx`=?
        QString strOne = QString("%1%2%1%3?").arg(SQP_ESCAPE_CHAR).arg(it.key()).arg(str);
        strWhereSql.append(str2);
        strWhereSql.append(strOne);
        bindPars.append(it.value());
    }
    return bindPars;
}
QString SqlUtil::whereJoinStr(const QString &aJoinFiledName, const QString &bJoinFiledName)
{
    // a.`file_id` = b.`id`
    QString strSql = QString("a.%1%2%1=b.%1%3%1").arg(SQP_ESCAPE_CHAR).arg(aJoinFiledName).arg(bJoinFiledName);
    return strSql;
}
QVariantList SqlUtil::whereStr(const QVariantMap &aParams, const QVariantMap &bParams, QString &strWhereSql, QString str)
{
    auto list = whereStr(aParams, strWhereSql, str, "a.");
    list.append(whereStr(bParams, strWhereSql, str, "b."));
    return list;
}
QVariantList SqlUtil::whereTimeSpaceAStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                                 const QDateTime &edDateTime, QString &strWhereSql)
{
    return whereStr(filedDateTime, bgDateTime, edDateTime, strWhereSql, "a.");
}
QVariantList SqlUtil::whereTimeSpaceBStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                                 const QDateTime &edDateTime, QString &strWhereSql)
{
    return whereStr(filedDateTime, bgDateTime, edDateTime, strWhereSql, "b.");
}
QVariantList SqlUtil::whereStr(const QString &filedDateTime, const QDateTime &bgDateTime,
                             const QDateTime &edDateTime, QString &strWhereSql, QString str)
{
    // #define WK_MYSQL_TIMESPACE  " between ? and ?"
    QString strFiled = QString("%1%2%3%2").arg(str).arg(SQP_ESCAPE_CHAR).arg(filedDateTime);
    if(strWhereSql != "") {
        strWhereSql += " and ";
    }
    strWhereSql += (strFiled + WK_MYSQL_TIMESPACE);
    return {bgDateTime, edDateTime};
}

QVariantList SqlUtil::wherePageStr(int pageIndex, int pageSize, QString &strWhereSql)
{
    strWhereSql.append(WK_MYSQL_PAGES);
    return {(pageIndex - 1) * pageSize, pageSize};
}

/// 更新
QPair<QString, QVariantList> SqlUtil::updateSet(const QVariantMap &updateDatas)
{
    QPair<QString, QVariantList> pair;
    QString strSql;
    QVariantList bindPars;
    //列名称 = 新值
    for(auto it = updateDatas.begin(); it != updateDatas.end(); it ++) {
        if(strSql != "") {
            strSql.append(",");
        }
        strSql.append(QString("%1%2%1=?").arg(SQP_ESCAPE_CHAR).arg(it.key()));
        bindPars.append(it.value());
    }
    pair.first = strSql;
    pair.second = bindPars;
    return pair;
}
/// 插入
QPair<QString, QVariantList> SqlUtil::insertStr(const QVariantMap &updateDatas)
{
    //(列1, 列2,...) VALUES (值1, 值2,....)
    QVariantList bindPars;
    QString strSqlFileds;
    QString strValuses;
    for(auto it = updateDatas.begin(); it != updateDatas.end(); it ++) {
        if(strSqlFileds != "") {
            strSqlFileds.append(",");
            strValuses.append(",");
        }
        strSqlFileds.append(SQP_ESCAPE_CHAR);
        strSqlFileds.append(it.key());
        strSqlFileds.append(SQP_ESCAPE_CHAR);
        strValuses.append("?");
        bindPars.append(it.value());
    }
    QString strSql = QString("(%1) VALUES (%2)").arg(strSqlFileds).arg(strValuses);
    return QPair<QString, QVariantList>(strSql, bindPars);
}

SqlPackUtil::SqlPackUtil(Model model)
{
    m_model = model;
}
SqlPackUtil::SqlPackUtil(const QString &tableAName, const QString &tableBName, Model model)
{
    m_model = model;
    m_p.tableAName = tableAName;
    m_p.tableBName = tableBName;
}

void SqlPackUtil::setTableAName(const QString &tableAName)
{
    m_p.tableAName = tableAName;
}
void SqlPackUtil::setTableBName(const QString &tableBName)
{
    m_p.tableBName = tableBName;
}

///
/// \brief setSelectFileds: 设置查询字段
/// \param aSelectFileds：a表查询字段
/// \param bSelectFileds：b表查询字段
///
void SqlPackUtil::setSelectFileds(const QStringList &aSelectFileds, const QStringList &bSelectFileds)
{
    m_p.aSelectFileds = aSelectFileds;
    m_p.bSelectFileds = bSelectFileds;
}
/// 设置查询分页参数
void SqlPackUtil::setSelectPageParam(int pageIndex, int pageSize)
{
    m_p.pageSize = pageSize;
    m_p.pageIndex = pageIndex;
}
/// 设置查询时间范围: dateTimeFileName: 查询字段名称
void SqlPackUtil::setSelectDateTimeSpace(const QString &dateTimeFileName, const QDateTime &bgDateTime,
                                         const QDateTime &edDateTime, DateTimeSpaceTable timeSpaceTable)
{
    m_p.bgDateTime = bgDateTime;
    m_p.edDateTime = edDateTime;
    m_p.filedDateTime = dateTimeFileName;
    m_timeSpaceTable = timeSpaceTable;
}

/// 设置where等于条件参数: key:字段名 value: 字段值
void SqlPackUtil::setWhereEqual(const QVariantMap &aEqualParams, const QVariantMap &bEqualParams)
{
    m_p.aWhereEqual = aEqualParams;
    m_p.bWhereEqual = bEqualParams;
}
/// 设置where不等于条件参数: key:字段名 value: 字段值
void SqlPackUtil::setWhereNotEqual(const QVariantMap &aNotEqualParams, const QVariantMap &bNotEqualParams)
{
    m_p.aWhereNotEqual = aNotEqualParams;
    m_p.bWhereNotEqual = bNotEqualParams;
}

/// 设置连表的等于关联参数
void SqlPackUtil::setJoinFileds(const QString &aFiledName, const QString &bFiledName)
{
    m_p.aJoinFiledName = aFiledName;
    m_p.bJoinFiledName = bFiledName;
}

/// 设置插入数据 key: 字段名 value: 值
void SqlPackUtil::setInsertDatas(const QVariantMap &insertDatas)
{
    m_p.insertData = insertDatas;
}
/// 设置更新数据
void SqlPackUtil::setUpdateDatas(const QVariantMap &updateDatas)
{
    m_p.updateData = updateDatas;
}

/// 获取组装结果的sql
/// 查询语句
QString SqlPackUtil::sqlSelect()
{
    auto pair = conditionSql();

    if(pair.first == "") {
        pair.first += " 1=1 ";
    }
    pair.second.append(SqlUtil::wherePageStr(m_p.pageIndex, m_p.pageSize, pair.first));

    m_p.bindParams = pair.second;
    QString strSelect;
    if(m_model == SingleTable) { // 单表操作
        strSelect = SqlUtil::selectStr(m_p.aSelectFileds);
    }else {
        strSelect = SqlUtil::selectStr(m_p.aSelectFileds, m_p.bSelectFileds);
    }

    QString strSql = QString("select %1 from %2 where %3")
            .arg(strSelect).arg(m_p.tableAName).arg(pair.first);
    return strSql;
}
/// count语句
QString SqlPackUtil::sqlCount()
{
    auto pair = conditionSql();
    m_p.bindParams = pair.second;
    QString strSql = QString("select count(1) from %1%2%3")
            .arg(m_p.tableAName).arg(pair.first == "" ? "": " where ").arg(pair.first);
    return strSql;
}
QPair<QString, QVariantList> SqlPackUtil::conditionSql()
{
    QVariantList list;
    QString strWhere;
    if(m_model == SingleTable) { // 单表操作
        // 普通 = <> where条件
        list.append(SqlUtil::whereStr(m_p.aWhereEqual, strWhere, "="));
        list.append(SqlUtil::whereStr(m_p.aWhereNotEqual, strWhere, "<>"));
    }else { // 多表操作
        // 连表条件
        strWhere = SqlUtil::whereJoinStr(m_p.aJoinFiledName, m_p.bJoinFiledName);
        // 普通 = <> where条件
        list = SqlUtil::whereStr(m_p.aWhereEqual, m_p.bWhereEqual, strWhere, "=");
        list.append(SqlUtil::whereStr(m_p.aWhereNotEqual, m_p.bWhereNotEqual, strWhere, "<>"));
    }
    // 时间范围
    if(m_p.filedDateTime != "") {
        if(m_timeSpaceTable == A_Table) {
            list.append(SqlUtil::whereTimeSpaceAStr(m_p.filedDateTime, m_p.bgDateTime, m_p.edDateTime, strWhere));
        }else if(m_timeSpaceTable == A_Table) {
            list.append(SqlUtil::whereTimeSpaceBStr(m_p.filedDateTime, m_p.bgDateTime, m_p.edDateTime, strWhere));
        }else {
            list.append(SqlUtil::whereStr(m_p.filedDateTime, m_p.bgDateTime, m_p.edDateTime, strWhere));
        }
    }
    return QPair<QString, QVariantList>(strWhere, list);
}

/// 插入语句
QString SqlPackUtil::sqlInsert()
{
    //INSERT INTO table_name (列1, 列2,...) VALUES (值1, 值2,....)
    auto pair = SqlUtil::insertStr(m_p.insertData);
    m_p.bindParams = pair.second;
    QString strSql = QString("INSERT INTO %1 %2").arg(m_p.tableAName).arg(pair.first);
    return strSql;
}
/// 更新语句
QString SqlPackUtil::sqlUpdate()
{
    //UPDATE 表名称 SET 列名称 = 新值 WHERE 列名称 = 某值
    auto pair = SqlUtil::updateSet(m_p.updateData);

    QString strWhere;
    auto list = SqlUtil::whereStr(m_p.aWhereEqual, strWhere, "=");
    auto listNot = SqlUtil::whereStr(m_p.aWhereNotEqual, strWhere, "<>");
    if(listNot.size() > 0)
        list.append(listNot);
    m_p.bindParams = pair.second + list;

    QString strSql = QString("UPDATE %1 SET %2 WHERE %1")
            .arg(m_p.tableAName)
            .arg(pair.first)
            .arg(strWhere);
    return strSql;
}
/// 获取绑定参数
QVariantList SqlPackUtil::bindParams()
{
    return m_p.bindParams;
}
