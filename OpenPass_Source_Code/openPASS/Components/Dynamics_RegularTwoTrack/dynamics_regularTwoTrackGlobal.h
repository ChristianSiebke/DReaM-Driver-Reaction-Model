/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017, 2019, 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  dynamics_regularTwoTrackGlobal.h
*	@brief This file contains DLL export declarations
**/
//-----------------------------------------------------------------------------

#pragma once

#include <QtCore/qglobal.h>

#if defined(DYNAMICS_REGULAR_TWOTRACK_LIBRARY)
#  define DYNAMICS_REGULAR_TWOTRACK_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define DYNAMICS_REGULAR_TWOTRACK_SHARED_EXPORT Q_DECL_IMPORT
#endif


