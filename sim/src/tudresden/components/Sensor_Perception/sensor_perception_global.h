
//-----------------------------------------------------------------------------/
// Copyright (c) TU Dresden.
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v1.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v10.html
//-----------------------------------------------------------------------------/

#pragma once

#include <QtCore/qglobal.h>

#if defined(SENSOR_PERCEPTION_LIBRARY)
#define SENSOR_PERCEPTION_SHARED_EXPORT Q_DECL_EXPORT
#else
#define SENSOR_PERCEPTION_SHARED_EXPORT Q_DECL_IMPORT
#endif
