#ifndef CTYPEDEFSYNTAX_H
#define CTYPEDEFSYNTAX_H

/// 给类型重定义别名

#include <memory>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QVariantMap>

namespace wkit {

using QByteArrayPtr = std::shared_ptr<QByteArray>;
using QStringPtr = std::shared_ptr<QString>;
using QVariantMapPtr = std::shared_ptr<QVariantMap>;

} // end namespace
#endif // CTYPEDEFSYNTAX_H
