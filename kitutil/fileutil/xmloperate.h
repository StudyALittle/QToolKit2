#ifndef XMLOPERATE_H
#define XMLOPERATE_H

/**
 * @brief The XmlOperate class: xml操作类
 */

#include <QByteArray>
#include <QString>
#include <QDomDocument>
#include <QDomNode>
#include <QVariant>
#include <QList>
#include "kitutil_global.h"

class XmlOperatePrivate {
public:
    QDomDocument domDoc;
};

class KITUTIL_EXPORT XmlOperate
{
public:
    XmlOperate();
    ~XmlOperate();

    /**
     * @brief save: 保存数据到文件
     * @param filename
     */
    void save(const QString &filename);

    /**
     * @brief setXmlDataFromFile: 设置xml数据
     * @param filename: 文件名称
     * @return
     */
    bool setXmlDataFromFile(const QString &filename);

    /**
     * @brief setXmlData: 设置xml数据
     * @param xmlData
     * @param namespaceProcessing
     */
    bool setXmlData(const QByteArray &xmlData, bool namespaceProcessing = false,
                    QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);
    bool setXmlData(QIODevice *dev, bool namespaceProcessing = false,
                    QString *errorMsg = nullptr, int *errorLine = nullptr, int *errorColumn = nullptr);

    /**
     * @brief getValues: 获取路径下的下一级的多个值
     * @param path: 路径（例："/aa/bb/cc"  存在多个cc节点）
     * 例子：
     * <aa>
     *  <bb>
     *      <!--注释1-->
     *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
     *      <!--注释2-->
            <cc><infoid>0x4d000100</infoid><period>0</period></cc>

     * 返回多个cc的 infoid 和 period
     * @param bComment: true 获取节点注释
     * @param commentPos: 注释在节点位置（0：节点上方 1：节点下方） （上面例子中的注释）
     * @return
     */
    QList<QMap<QString, QString> > getPathChild1LVals(const QString &path, bool bComment = false, int commentPos = 0);

    /**
     * @brief getSingleValue: 获取路径下单个值
     * @param path
     * @return
     */
    QString getSingleValue(const QString &path);

    /**
     * @brief setSingleValue: 设置路径下的值
     * @param path
     * @param text
     */
    void setSingleValue(const QString &path, const QString &text, bool bCreatePath = true);

    /**
     * @brief exchangeTowNode: 交换两个节点（路径下存在多个同级子节点）
     * @param path: 路径
     * @param index1: 子节点（位置）
     * @param index2: 子节点（位置）
     * @param commentPos: 0: 表示注释在节点上面 1: 表示注释在节点下面
     * 示例：
     * 输入xml：
     * <aa>
     *  <bb>
     *      <!--语音指令-->
     *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
     *      <!--其它-->
     *      <cc><infoid>0x06080b23</infoid><period>0</period></cc>
     * 参数：
     *  path: "/aa/bb"
     *  index1: 0
     *  index2: 1
     */
    void exchangeTowNode(const QString &path, int subIndex1, int subIndex2, int commentPos = 0);

    /**
     * @brief set2LsChildVal
     * @param path
     * @param index: (从0开始)
     * @param child2NodeName
     * @param text
     * 示例：
     * 输入xml：
     * <aa>
     *  <bb>
     *      <!--语音指令-->
     *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
     *      <!--其它-->
     *      <cc><infoid>0x06080b23</infoid><period>0</period></cc>
     * 参数：
     *  path: "/aa/bb"
     *  index: 0 （对应cc同级节点）
     *  child2NodeName: "period"
     *  text: "1"
     * 输出xml：
     * <aa>
     *  <bb>
     *      <!--语音指令-->
     *      <cc><infoid>0x06080b22</infoid><period>1</period></cc>
     */
    void set2LsChildVal(const QString &path, int index, const QString child2NodeName, const QString &text);

    /**
     * @brief setComment: 设置节点注释
     * @param path: （同set2LsChildVal接口）
     * @param index: （同set2LsChildVal接口）
     * @param commentPos: 0: 表示注释在节点上面 1: 表示注释在节点下面
     */
    void set1LChildComment(const QString &path, const QString &text, int index, int commentPos = 0);

    /**
     * @brief insertSingleNodeAfter: 在路径之后插入新的节点
     * @param afterPath
     * @param nodeName
     * @param nodeVal
     */
    void insertSingleNodeAfter(const QString &afterPath, const QString &nodeName, const QString &nodeVal);

    /**
     * @brief insert2LsChildEnd: 再路径下子节点末尾插入两级子节点节点
     * @param path：路径
     * @param nodeName：一级节点
     * @param childsNodes：二级节点和二级节点文本
     * @param bComment: true，插入注释
     * @param bCommentPos: 0在节点上方插入注释, 1在节点下方插入注释（如下示例，在cc节点的上/下方插入注释）
     * @param strComment: 注释内容
     * @param bCreatePath: 如果路径（path）不存在，就会创建路径
     * 示例：
     * 参数：
     *  path: "/aa/bb"
     *  nodeName: cc
     *  childsNodes: [{infoid: 0x06080b22}, {period: 0}]
     * 输出：
     * <aa>
     *  <bb>
     *      <!--这是一个注释-->
     *      <cc><infoid>0x06080b22</infoid><period>0</period></cc>
     */
    void insert2LsChildEnd(const QString &path, const QString &nodeName, const QList<QPair<QString, QString> > &childsNodes,
                           bool bComment = true, bool bCommentPos = 0, const QString &strComment = "", bool bCreatePath = true);

    /**
     * @brief delChildNodeAt: 删除路径下的子节点
     * @param path: 路径
     * @param index: 节点索引（从1开始）
     * @param bComment: 删除节点对应的注释
     * @param bCommentPos: 0删除的注释在删除节点的上方，1删除的注释在删除节点的下方
     */
    void delChildNodeAt(const QString &path, int index, bool bComment = true, bool bCommentPos = 0);

    /**
     * @brief toByteArray
     * @param indent
     * @return
     */
    QByteArray toByteArray(int indent = 4);

    void test();
protected:
    /**
     * @brief isTextNode: 判断节点下一级(子节点)是不是文本节点
     * @param docElement
     * @return
     */
    bool isNodeSubText(QDomNode &docElement);

    /**
     * @brief getPathEndDomNode: 获取路径最后一个节点的DomNode
     * @param path:  路径（例："/aa/bb/cc"  存在多个节点只返回第一个节点）
     * @param bCreatePath: true：不存在路径时创建路径
     * @return
     */
    QDomNode getPathEndDomNode(const QString &path, bool bCreatePath = false);

    /**
     * @brief getSubDomEle: 获取下一级的QDomNode, 不会获取下一级的下一级
     * @param domEle: 当前QDomElement
     * @param tagName: 下一级TagName
     * @return
     */
    QDomNode getChildFirstDomNode(QDomNode &node, const QString &nodeName);

    /**
     * @brief changeTextNodeValue: 修改文本节点的值
     * @param node
     */
    void changeTextNodeValue(QDomNode &node, const QString &text);
private:
    XmlOperatePrivate *m_p;
};

#endif // XMLOPERATE_H
