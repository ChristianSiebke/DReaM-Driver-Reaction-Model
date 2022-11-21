/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once

#include <QtCore/qglobal.h>

#if defined(SENSOR_PERCEPTION_LIBRARY)
#define SENSOR_PERCEPTION_SHARED_EXPORT Q_DECL_EXPORT
#else
#define SENSOR_PERCEPTION_SHARED_EXPORT Q_DECL_IMPORT
#endif
