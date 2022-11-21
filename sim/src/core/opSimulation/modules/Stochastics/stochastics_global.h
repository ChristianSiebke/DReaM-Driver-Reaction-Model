/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  StochasticsGlobal.h
*	@brief This file contains DLL export declarations
**/
//-----------------------------------------------------------------------------

#pragma once

#include <QtCore/qglobal.h>

#if defined(STOCHASTICS_LIBRARY)
#  define STOCHASTICS_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define STOCHASTICS_SHARED_EXPORT Q_DECL_IMPORT
#endif


