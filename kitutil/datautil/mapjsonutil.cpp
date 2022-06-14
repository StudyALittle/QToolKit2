#include "mapjsonutil.h"

using namespace wkit;

MapJsonUtil::MapJsonUtil()
{

}

QByteArray MapJsonUtil::variantMapToJson(const QVariantMap &map, QJsonDocument::JsonFormat format)
{
    return *(variantMapToJsonPtr(map, format));
}
QByteArrayPtr MapJsonUtil::variantMapToJsonPtr(const QVariantMap &map, QJsonDocument::JsonFormat format)
{
    auto jsonObj = QJsonObject::fromVariantMap(map);
    QJsonDocument jsonDom(jsonObj);
    return std::make_shared<QByteArray>(jsonDom.toJson(format));
}

///json字符串转map
QVariantMap MapJsonUtil::byteArrayToVariantMap(const QByteArray &data)
{
    return *(byteArrayToVariantMapPtr(data));
}
QVariantMapPtr MapJsonUtil::byteArrayToVariantMapPtr(const QByteArray &data)
{
    auto jsonDom = QJsonDocument::fromJson(data);
    auto jsonObj = jsonDom.object();
    return std::make_shared<QVariantMap>(jsonObj.toVariantMap());
}
