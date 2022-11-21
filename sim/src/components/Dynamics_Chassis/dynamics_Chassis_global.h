/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dynamics_Chassis_global.h
//! @brief This file contains DLL export declarations
//-----------------------------------------------------------------------------

#ifndef DYNAMICS_CHASSIS_GLOBAL_H
#define DYNAMICS_CHASSIS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DYNAMICS_CHASSIS_LIBRARY)
#  define DYNAMICS_CHASSIS_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define DYNAMICS_CHASSIS_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DYNAMICS_CHASSIS_GLOBAL_H
