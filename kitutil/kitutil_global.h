#ifndef KITUTIL_GLOBAL_H
#define KITUTIL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KITUTIL_LIBRARY)
#  define KITUTIL_EXPORT Q_DECL_EXPORT
#else
#  define KITUTIL_EXPORT Q_DECL_IMPORT
#endif

#endif // KITUTIL_GLOBAL_H
