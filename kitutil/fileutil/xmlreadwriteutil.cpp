#include "xmlreadwriteutil.h"

using namespace wkit;

// 全局xml文件路径
XmlReadWriteUtil XmlUtil::g_xmlReadWriteUtil;
// 设置xml文件路径
void XmlUtil::setXmlFilePath(const QString &path)
{
    g_xmlReadWriteUtil.init(path);
}
// 读取xml文件的值
void XmlUtil::readXmlFileVal(QString &val, const QString &keyPath, const QString &defaulVal)
{
    val = g_xmlReadWriteUtil.getValue(keyPath);
    if(val == "") val = defaulVal;
}
// 读取xml文件的值
void XmlUtil::readXmlFileVal(int &val, const QString &keyPath, const int &defaulVal)
{
    val = g_xmlReadWriteUtil.getValue(keyPath).toInt();
    if(val == 0) val = defaulVal;
}
// 读取xml文件的值
void XmlUtil::readXmlFileVal(uint &val, const QString &keyPath, const uint &defaulVal)
{
    val = g_xmlReadWriteUtil.getValue(keyPath).toUInt();
    if(val == 0) val = defaulVal;
}
void XmlUtil::readXmlFileVal(qint16 &val, const QString &keyPath, const qint16 &defaulVal)
{
    val = g_xmlReadWriteUtil.getValue(keyPath).toShort();
    if(val == 0) val = defaulVal;
}
void XmlUtil::readXmlFileVal(quint16 &val, const QString &keyPath, const quint16 &defaulVal)
{
    val = g_xmlReadWriteUtil.getValue(keyPath).toUShort();
    if(val == 0) val = defaulVal;
}


XmlReadWriteUtil::XmlReadWriteUtil(QObject *parent):
    QObject(parent)
{

}
XmlReadWriteUtil::XmlReadWriteUtil(const QString &filename, QObject *parent):
    QObject(parent), m_filename(filename)
{
    init(filename);
}

///
/// \brief init: 初始化
/// \param filename
///
void XmlReadWriteUtil::init(const QString &filename)
{
    m_filename = filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!m_domDoc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();
    m_hashVal = xmlToMap(m_domDoc);
}

///
/// \brief getValue: 获取xml值
/// \param path： /xxx/xxx/xxx
///
QString XmlReadWriteUtil::getValue(const QString &path)
{
    QString val;
    QStringList strList = path.split("/", QString::SkipEmptyParts);
    QHash<QString, QVariant> hash = m_hashVal;
    for(int n = 0; n < strList.size(); n ++) {
        if(hash.find(strList.at(n)) == hash.end()) {
            return val;
        } else if(n + 1 == strList.size()) {
            QVariant vt = hash.value(strList.at(n));
            if(vt.type() != QVariant::String)
                return val;
            return vt.toString();
        }

        hash = hash.value(strList.at(n)).toHash();
    }
    return val;
}

bool XmlReadWriteUtil::isTextNode(QDomElement &docElement)
{
    QDomNode domNode = docElement.firstChild();
    if(!domNode.isNull() && domNode.nodeType() == QDomNode::TextNode) {
        return true;
    }
    return false;
}

QHash<QString, QVariant> XmlReadWriteUtil::xmlToMapEx(QDomElement &domElement, const QString &tagName) {
    QHash<QString, QVariant> result;
    if(isTextNode(domElement)) {
        result.insert(domElement.tagName(), domElement.text());
        return result;
    }

    QDomNodeList domNodeList = domElement.childNodes();
    for(int n = 0; n < domNodeList.size(); n ++) {
        if(domNodeList.at(n).nodeType() != QDomNode::ElementNode)
           continue;

        QDomElement domEle = domNodeList.at(n).toElement();
        QHash<QString, QVariant> vMapResult = xmlToMapEx(domEle, domEle.tagName());
        if(vMapResult.size() <= 0)
            continue;

        QHash<QString, QVariant> tmpMap = result[tagName].toHash();
        tmpMap.insert(vMapResult.begin().key(), vMapResult.begin().value());

        result[tagName] = tmpMap;
    }
    return result;
}

QHash<QString, QVariant> XmlReadWriteUtil::xmlToMap(QDomDocument &doc)
{
    QDomElement docElem = doc.documentElement();
    QString tagName = docElem.tagName();

    QHash<QString, QVariant> vmap = xmlToMapEx(docElem, tagName);
    if(vmap.begin().key() != tagName && vmap.size() > 0) {
        QHash<QString, QVariant> vmapNew;
        vmapNew.insert(tagName, vmap);
        return vmapNew;
    }
    return vmap;
}
