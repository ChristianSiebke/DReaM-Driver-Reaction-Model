/******************************************************************************
 * Copyright (c) 2022 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once

#include <QtCore/qglobal.h>

#if defined(GLOBALOBSERVER_LIBRARY)
#define GLOBALOBSERVER_SHARED_EXPORT Q_DECL_EXPORT
#else
#define GLOBALOBSERVER_SHARED_EXPORT Q_DECL_IMPORT
#endif