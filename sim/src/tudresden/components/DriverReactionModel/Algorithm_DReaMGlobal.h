/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
//-----------------------------------------------------------------------------
//! @file  Algorithm_DReaMGlobal.h
//! @brief contains DLL export declarations
//---------------------------------------------------------

#ifndef ALGORITHM_DReaM_GLOBAL_H
#define ALGORITHM_DReaM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ALGORITHM_DReaM_LIBRARY)
#define ALGORITHM_DReaM_SHARED_EXPORT Q_DECL_EXPORT
#else
#define ALGORITHM_DReaM_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ALGORITHM_DReaM_GLOBAL_H
