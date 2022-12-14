#include "xmloperate.h"
#include <QDebug>
#include <QFile>

#pragma execution_character_set("utf-8")

XmlOperate::XmlOperate()
{
    m_p = new XmlOperatePrivate;
}

XmlOperate::~XmlOperate()
{
    if (m_p) {
        delete m_p;
    }
}

/**
 * @brief save: 保存数据到文件
 * @param filename
 */
void XmlOperate::save(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QFile::Text)) {
        QTextStream out(&file);
        m_p->domDoc.save(out, 4, QDomDocument::EncodingFromDocument);
        file.close();
    }
}

/**
 * @brief setXmlDataFromFile: 设置xml数据
 * @param filename: 文件名称
 * @return
 */
bool XmlOperate::setXmlDataFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    bool rt = setXmlData(&file);
    file.close();
    return rt;
}

/**
 * @brief setXmlData: 设置xml数据
 * @param xmlData
 * @param namespaceProcessing
 */
bool XmlOperate::setXmlData(const QByteArray &xmlData, bool namespaceProcessing,
                            QString *errorMsg, int *errorLine, int *errorColumn)
{
    return m_p->domDoc.setContent(xmlData, namespaceProcessing, errorMsg, errorLine, errorColumn);
}
bool XmlOperate::setXmlData(QIODevice *dev, bool namespaceProcessing,
                QString *errorMsg, int *errorLine, int *errorColumn)
{
    return m_p->domDoc.setContent(dev, namespaceProcessing, errorMsg, errorLine, errorColumn);
}

/**
 * @brief getValues: 获取值
 * @param path
 * @param subTagName
 * @return
 */
QList<QMap<QString, QString> > XmlOperate::getPathChild1LVals(const QString &path, bool bComment, int commentPos)
{
    QDomNode node = getPathEndDomNode(path);
    if (node.isNull())
        return QList<QMap<QString, QString> >();

    QList<QMap<QString, QString> > rt;
    auto funcGetChildNodeTextVals = [=](QDomNode &node) {
        QMap<QString, QString> rt;
        if (node.isNull())
            return rt;

        if (bComment) { // 处理注释节点
            QDomNode nodeComment;
            if (commentPos == 0) {
                nodeComment = node.previousSibling();
            } else {
                nodeComment = node.nextSibling();
            }
            if (nodeComment.nodeType() == QDomNode::CommentNode) {
                rt.insert("describe", nodeComment.toComment().data());
            }
        }

        QDomNodeList childs = node.childNodes();
        for (int m = 0; m < childs.size(); m ++) {
            QDomNode cNode = childs.at(m);
            if (cNode.isElement()) {
                rt.insert(cNode.nodeName(), cNode.toElement().text());
            } else { // 子节点不是文本节点
                rt.insert(cNode.nodeName(), "");
            }
        }
        return rt;
    };

    do {
        if (node.nodeType() != QDomNode::CommentNode) { // 不解析注释节点
            rt.append(funcGetChildNodeTextVals(node));
        }
        node = node.nextSibling();
    } while (!node.isNull());

    return rt;
}

/**
 * @brief getSingleValue: 获取路径下单个值
 * @param path
 * @return
 */
QString XmlOperate::getSingleValue(const QString &path)
{
    QDomNode node = getPathEndDomNode(path);
    if (node.isNull() || !node.isElement())
        return "";

    return node.toElement().text();
}

/**
 * @brief setSingleValue: 设置路径下的值
 * @param path
 * @param text
 */
void XmlOperate::setSingleValue(const QString &path, const QString &text)
{
    QDomNode node = getPathEndDomNode(path);
    changeTextNodeValue(node, text);
}

/**
 * @brief set2LsChildVal
 * @param path
 * @param index
 * @param child2NodeName
 * @param text
 * 示例：
 * 输入xml：
 * <aa>
 *  <bb>
 *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
 * 参数：
 *  path: "/aa/bb"
 *  index: 0
 *  child2NodeName: "period"
 *  text: "1"
 * 输出xml：
 * <aa>
 *  <bb>
 *      <cc><infoid>0x06080b22</infoid><period>1</period></cc>
 */
void XmlOperate::set2LsChildVal(const QString &path, int index, const QString child2NodeName, const QString &text)
{
    QDomNode node = getPathEndDomNode(path);
    auto childs = node.childNodes();
    int pos = 0;
    int cSize = childs.size();
    for (int n = 0; n < cSize; ++n) {
        QDomNode cNode = childs.at(n);
        if (cNode.nodeType() == QDomNode::CommentNode) // 跳过注释节点
            continue;
        if ((pos ++) == index) { // 主节点位置
            auto cChilds = cNode.childNodes();
            int ccSize = cChilds.size();
            for (int m = 0; m < ccSize; ++m) {
                auto ccNode = cChilds.at(m);
                if (ccNode.nodeName() == child2NodeName) {
                    // 修改节点值
                    changeTextNodeValue(ccNode, text);
                    break;
                }
            }
            break;
        }
    }
}

/**
 * @brief setComment: 设置节点注释
 * @param path: （同set2LsChildVal接口）
 * @param index: （同set2LsChildVal接口）
 * @param commentPos: 0: 表示注释在节点上面 1: 表示注释在节点下面
 */
void XmlOperate::set1LChildComment(const QString &path, const QString &text, int index, int commentPos)
{
    QDomNode node = getPathEndDomNode(path);
    auto childs = node.childNodes();
    int pos = 0;
    int cSize = childs.size();
    for (int n = 0; n < cSize; ++n) {
        QDomNode cNode = childs.at(n);
        if (cNode.nodeType() == QDomNode::CommentNode) // 跳过注释节点
            continue;
        if ((pos ++) == index) { // 主节点位置
            QDomNode nodeComment;
            if (commentPos == 0) {
                nodeComment = cNode.previousSibling();
            } else {
                nodeComment = cNode.nextSibling();
            }
            if (nodeComment.nodeType() == QDomNode::CommentNode) { // 注释节点，修改注释
                nodeComment.setNodeValue(text);
            }
            break;
        }
    }
}

/**
 * @brief insertSingleNodeAfter: 在路径之后插入新的节点
 * @param afterPath
 * @param nodeName
 * @param nodeVal
 */
void XmlOperate::insertSingleNodeAfter(const QString &afterPath, const QString &nodeName, const QString &nodeVal)
{
    QDomElement newEle = m_p->domDoc.createElement(nodeName);
    QDomText textEle = m_p->domDoc.createTextNode(nodeVal);
    newEle.appendChild(textEle);

    QDomNode node = getPathEndDomNode(afterPath);
    node.parentNode().insertAfter(newEle, node);
}

/**
 * @brief insert2LsChildEnd: 再路径下子节点末尾插入两级子节点节点
 * @param path：路径
 * @param nodeName：一级节点
 * @param childsNodes：二级节点和二级节点文本
 * 示例：
 * 参数：
 *  path: "/aa/bb"
 *  nodeName: cc
 *  childsNodes: [{infoid: 0x06080b22}, {period: 0}]
 * 输出：
 * <aa>
 *  <bb>
 *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
 */
void XmlOperate::insert2LsChildEnd(const QString &path, const QString &nodeName,
                                   const QList<QPair<QString, QString> > &childsNodes,
                                   bool bComment, bool bCommentPos, const QString &strComment)
{
    QDomNode node = getPathEndDomNode(path);

    QDomElement newEle = m_p->domDoc.createElement(nodeName);
    for (auto it = childsNodes.begin(); it != childsNodes.end(); ++it) {
        QDomElement cNewEle = m_p->domDoc.createElement(it->first);
        QDomText textEle = m_p->domDoc.createTextNode(it->second);
        cNewEle.appendChild(textEle);

        newEle.appendChild(cNewEle);
    }

    if (bComment) {
        QDomComment newComment = m_p->domDoc.createComment(strComment);
        if (bCommentPos == 0) { // 先添加注释再添加节点
            node.appendChild(newComment);
            node.appendChild(newEle);
        } else { // 先添加节点再添加注释
            node.appendChild(newEle);
            node.appendChild(newComment);
        }
    } else {
        node.appendChild(newEle);
    }
}

/**
 * @brief delChildNodeAt: 删除路径下的子节点
 * @param path: 路径
 * @param index: 节点索引（从0开始）
 */
void XmlOperate::delChildNodeAt(const QString &path, int index, bool bComment, bool bCommentPos)
{
    QDomNode node = getPathEndDomNode(path);
    QDomNodeList childs = node.childNodes();
    if (index < 0 || index >= childs.size())
        return;

    int pos = 0;
    int cSize = childs.size();
    for (int n = 0; n < cSize; ++n) {
        QDomNode cNode = childs.at(n);
        if (cNode.nodeType() == QDomNode::CommentNode) // 跳过注释节点
            continue;
        if ((pos ++) == index) {
            if (bComment) { // 删除注释节点
                QDomNode nodeComment;
                if (bCommentPos == 0) {
                    nodeComment = cNode.previousSibling();
                } else {
                    nodeComment = cNode.nextSibling();
                }
                if (nodeComment.nodeType() == QDomNode::CommentNode) {
                    if (bCommentPos == 0) {
                        node.removeChild(cNode);
                        node.removeChild(nodeComment);
                    } else {
                        node.removeChild(nodeComment);
                        node.removeChild(cNode);
                    }
                } else {
                    node.removeChild(cNode);
                }
            } else {
                node.removeChild(cNode);
            }
            break;
        }
    }
}

/**
 * @brief toByteArray
 * @param indent
 * @return
 */
QByteArray XmlOperate::toByteArray(int indent)
{
    return m_p->domDoc.toByteArray(indent);
}

/**
 * @brief isTextNode: 判断节点下一级(子节点)是不是文本节点
 * @param docElement
 * @return
 */
bool XmlOperate::isNodeSubText(QDomNode &docElement)
{
    QDomNode domNode = docElement.firstChild();
    if(!domNode.isNull() && domNode.nodeType() == QDomNode::TextNode) {
        return true;
    }
    return false;
}

/**
 * @brief getPathEndDomNode: 获取路径最后一个节点的DomNode
 * @param path:  路径（例："/aa/bb/cc"  存在多个节点只返回第一个节点）
 * @return
 */
QDomNode XmlOperate::getPathEndDomNode(const QString &path)
{
    QList<QMap<QString, QString> > rt;
    QStringList pathList = path.split("/", QString::SkipEmptyParts);
    if (pathList.size() <= 0)
        return QDomNode();

    QDomNode node = m_p->domDoc.documentElement();
    for (int n = 0; n < pathList.size(); n ++) {
        if (n) {
            node = getChildFirstDomNode(node, pathList.at(n));
        }
        if (node.isNull())
            return node;
    }
    return node;
}

/**
 * @brief getSubDomEle: 获取下一级的QDomElement
 * @param domEle: 当前QDomElement
 * @param tagName: 下一级TagName
 * @return
 */
QDomNode XmlOperate::getChildFirstDomNode(QDomNode &node, const QString &nodeName)
{
    QDomNodeList childs = node.childNodes();
    for (int m = 0; m < childs.size(); m ++) {
        QDomNode cNode = childs.at(m);
        // 找到当前节点
        if (cNode.nodeName() == nodeName)
            return cNode;
    }
    return QDomNode();
}

/**
 * @brief changeTextNodeValue: 修改文本节点的值
 * @param node
 */
void XmlOperate::changeTextNodeValue(QDomNode &node, const QString &text)
{
//    if (node.isNull() /*|| !isNodeSubText(node)*/)
//        return;
    QDomNode domNode = node.firstChild();
    if (domNode.isNull()) {
        QDomText textEle = m_p->domDoc.createTextNode(text);
        node.appendChild(textEle);
    } else {
        domNode.setNodeValue(text);
    }
}

void XmlOperate::test()
{
#if 0
    QDomElement docEle = m_p->domDoc.documentElement();
    docEle = docEle.firstChildElement("subscribeinfo");
    if (docEle.isNull()) {
        qDebug() << "firstChildElement is null: ";
    } else {
        qDebug() << "firstChildElement TagName: " << docEle.tagName();
    }

    QList<QMap<QString, QString> > data =
            getPathChild1LVals("/component/subscribeinfos/subscribeinfo");
    qDebug() << "data : " << data;
    qDebug() << "Single val : " << getSingleValue("/component/semcount");
    setSingleValue("/component/semcount", "66");
    qDebug() << "Single val : " << getSingleValue("/component/semcount");
    insertSingleNodeAfter("/component/name", "testnode", "123");

    set2LsChildVal("/component/subscribeinfos", 0, "period", "8");
    set1LChildComment("/component/subscribeinfos", "change notes", 0);
    delChildNodeAt("/component/subscribeinfos", 2, true);
    delChildNodeAt("/component/subscribeinfos", 1, true);

    QList<QPair<QString, QString> > subNodes;
    subNodes.append(QPair<QString, QString>("infoid", "0x66666666"));
    subNodes.append(QPair<QString, QString>("period", "01"));
    insert2LsChildEnd("/component/subscribeinfos", "subscribeinfo", subNodes, true, 0, "is a notes");

    qDebug() << m_p->domDoc.toString(4);

    QFile file("D:/xxx/testxml.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QFile::Text)) {
        //file.write(m_p->domDoc.toByteArray());

        QTextStream out(&file);
        m_p->domDoc.save(out, 4, QDomDocument::EncodingFromDocument);

        file.close();
    }
#endif
}
