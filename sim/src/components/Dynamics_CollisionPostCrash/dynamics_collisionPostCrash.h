/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2017-2019 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  dynamics_collisionPostCrash.h
*	@brief This file provides the exported methods.
*
*   This file provides the exported methods which are available outside of the library. */
//-----------------------------------------------------------------------------

#pragma once

#include <QtCore/qglobal.h>

#if defined(DYNAMICS_COLLISIONPOSTCRASH_LIBRARY)
#  define DYNAMICS_COLLISIONPOSTCRASH_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define DYNAMICS_COLLISIONPOSTCRASH_SHARED_EXPORT Q_DECL_IMPORT
#endif

#include "include/modelInterface.h"


