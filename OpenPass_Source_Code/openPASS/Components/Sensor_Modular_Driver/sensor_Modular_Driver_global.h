/*********************************************************************
* Copyright (c) 2016 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/


//-----------------------------------------------------------------------------
//! @file  sensor_Modular_Driver_global.h
//! @brief This file contains DLL export declarations
//-----------------------------------------------------------------------------

#ifndef SENSOR_MODULAR_DRIVER_GLOBAL_H
#define SENSOR_MODULAR_DRIVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SENSOR_MODULAR_DRIVER_LIBRARY)
#  define SENSOR_MODULAR_DRIVER_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SENSOR_MODULAR_DRIVER_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SENSOR_MODULAR_DRIVER_GLOBAL_H
