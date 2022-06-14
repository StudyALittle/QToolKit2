#ifndef MAPJSONUTIL_H
#define MAPJSONUTIL_H

#include <QObject>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include "ctypedefsyntax.h"

namespace wkit {

class MapJsonUtil
{
public:
    MapJsonUtil();

    ///map转json字符串
    static QByteArray variantMapToJson(const QVariantMap &map, QJsonDocument::JsonFormat format = QJsonDocument::Compact);
    static QByteArrayPtr variantMapToJsonPtr(const QVariantMap &map, QJsonDocument::JsonFormat format = QJsonDocument::Compact);

    ///json字符串转map
    static QVariantMap byteArrayToVariantMap(const QByteArray &data);
    static QVariantMapPtr byteArrayToVariantMapPtr(const QByteArray &data);
};

} // end namespace

#endif // MAPJSONUTIL_H
