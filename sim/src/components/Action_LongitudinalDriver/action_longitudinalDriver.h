/*******************************************************************************
* Copyright (c) 2017, 2019 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  actio_lLongitudinalDriver.h
*	@brief This file provides the exported methods.
*
*   This file provides the exported methods which are available outside of the library. */
//-----------------------------------------------------------------------------

#ifndef ACTION_LONGITUDINAL_DRIVER_H
#define ACTION_LONGITUDINAL_DRIVER_H

#include <QtCore/qglobal.h>

#if defined(ACTION_LONGITUDINAL_DRIVER_LIBRARY)
#  define ACTION_LONGITUDINAL_DRIVER_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ACTION_LONGITUDINAL_DRIVER_SHARED_EXPORT Q_DECL_IMPORT
#endif

#include "include/modelInterface.h"

#endif // ACTION_LONGITUDINAL_DRIVER_H
