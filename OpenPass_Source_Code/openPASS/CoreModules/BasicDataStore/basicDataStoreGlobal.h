/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/**
 * \file  basicDataStoreGlobal.h
 * \brief This file contains DLL export declarations
 */

#pragma once

#include <QtCore/qglobal.h>

#if defined(BASIC_DATASTORE_LIBRARY)
#  define BASIC_DATASTORE_SHARED_EXPORT Q_DECL_EXPORT   //! Export of the dll-functions
#else
#  define BASIC_DATASTORE_SHARED_EXPORT Q_DECL_IMPORT   //! Import of the dll-functions
#endif


