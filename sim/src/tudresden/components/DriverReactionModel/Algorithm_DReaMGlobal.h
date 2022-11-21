/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
