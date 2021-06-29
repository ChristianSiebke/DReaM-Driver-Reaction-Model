/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  algorithm_ECU_global.h
//! @brief This file contains DLL export declarations
//-----------------------------------------------------------------------------

#ifndef ALGORITHM_ECU_GLOBAL_H
#define ALGORITHM_ECU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ALGORITHM_ECU_LIBRARY)
#  define ALGORITHM_ECU_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ALGORITHM_ECU_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ALGORITHM_ECU_GLOBAL_H
