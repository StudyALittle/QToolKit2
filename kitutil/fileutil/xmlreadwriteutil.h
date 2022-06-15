#ifndef XMLREADWRITEUTIL_H
#define XMLREADWRITEUTIL_H

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QVariantMap>
#include "kitutil_global.h"

namespace wkit {

class KITUTIL_EXPORT XmlReadWriteUtil: public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief XmlReadWriteUtil
    /// \param parent
    ///
    XmlReadWriteUtil(QObject *parent = nullptr);

    ///
    /// \brief XmlReadWriteUtil: 构造函数
    /// \param filename：不需要初始化（init调用）
    /// \param parent
    ///
    XmlReadWriteUtil(const QString &filename, QObject *parent = nullptr);

    ///
    /// \brief init: 初始化
    /// \param filename
    ///
    void init(const QString &filename);

    ///
    /// \brief getValue: 获取xml值
    /// \param path： /xxx/xxx/xxx
    ///
    QString getValue(const QString &path);
protected:

    ///
    /// \brief xmlToMap: xml转map
    /// \param doc
    /// \return
    ///
    QHash<QString, QVariant> xmlToMap(QDomDocument &doc);

    ///
    /// \brief xmlToMapEx
    /// \param domElement
    /// \param tagName
    /// \return
    ///
    QHash<QString, QVariant> xmlToMapEx(QDomElement &domElement, const QString &tagName);

    ///
    /// \brief isTextNode: 判断是不是文本节点
    /// \param docElement
    /// \return
    ///
    bool isTextNode(QDomElement &docElement);
private:
    QString m_filename;
    // 保存xml doc
    QDomDocument m_domDoc;
    //QDomElement m_docElem;
    // 保存xml值
    QHash<QString, QVariant> m_hashVal;
};

class KITUTIL_EXPORT XmlUtil
{
public:
    // xml操作对象
    static XmlReadWriteUtil g_xmlReadWriteUtil;
    // 设置xml文件路径
    static void setXmlFilePath(const QString &path);
    static void readXmlFileVal(QString &val, const QString &keyPath, const QString &defaulVal = "");
    static void readXmlFileVal(int &val, const QString &keyPath, const int &defaulVal = 0);
    static void readXmlFileVal(uint &val, const QString &keyPath, const uint &defaulVal = 0);
    static void readXmlFileVal(qint16 &val, const QString &keyPath, const qint16 &defaulVal = 0);
    static void readXmlFileVal(quint16 &val, const QString &keyPath, const quint16 &defaulVal = 0);
};

#define XML_BG(xmlpath) XmlUtil::setXmlFilePath(xmlpath)
#define XML_VALUE(val, keyPath) XmlUtil::readXmlFileVal(val, keyPath)
#define XML_VALUE_D(val, keyPath, defaultVal) XmlUtil::readXmlFileVal(val, keyPath, defaultVal)
#define XML_ED

} // end namespace

#endif // XMLREADWRITEUTIL_H
