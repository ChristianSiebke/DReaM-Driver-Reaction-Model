/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  algorithm_routecontrol_global.h
//! @brief This file defines dynamic library import/export.
//-----------------------------------------------------------------------------

#ifndef ALGORITHM_ROUTECONTROL_GLOBAL_H
#define ALGORITHM_ROUTECONTROL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ALGORITHM_ROUTECONTROL_LIBRARY)
#  define ALGORITHM_ROUTECONTROL_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ALGORITHM_ROUTECONTROL_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ALGORITHM_ROUTECONTROL_GLOBAL_H
