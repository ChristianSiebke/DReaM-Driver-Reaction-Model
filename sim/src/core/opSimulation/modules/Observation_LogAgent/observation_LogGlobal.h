/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/**
 * \file  observation_LogGlobal.h
 * \brief This file contains DLL export declarations
 */

#pragma once

#include <QtCore/qglobal.h>

#if defined(OBSERVATION_LOGAGENT_LIBRARY)
#  define OBSERVATION_LOGAGENT_SHARED_EXPORT Q_DECL_EXPORT   //! Export of the dll-functions
#else
#  define OBSERVATION_LOGAGENT_SHARED_EXPORT Q_DECL_IMPORT   //! Import of the dll-functions
#endif


