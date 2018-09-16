#ifndef SQLITE_GLOBAL_H
#define SQLITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SQLITE_LIBRARY)
#  define SQLITESHARED_EXPORT Q_DECL_EXPORT
#else
#  define SQLITESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SQLITE_GLOBAL_H
