#ifndef GAS_CAM_GLOBAL_H
#define GAS_CAM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GAS_CAM_LIBRARY)
#  define GAS_CAM_EXPORT Q_DECL_EXPORT
#else
#  define GAS_CAM_EXPORT Q_DECL_IMPORT
#endif

#endif // GAS_CAM_GLOBAL_H
