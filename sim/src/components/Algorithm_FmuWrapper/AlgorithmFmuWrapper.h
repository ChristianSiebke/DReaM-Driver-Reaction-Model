/*******************************************************************************
* Copyright (c) 2017 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <QtCore/qglobal.h>

#if defined(ALGORITHM_FMUWRAPPER_LIBRARY)
#  define ALGORITHM_FMUWRAPPER_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ALGORITHM_FMUWRAPPER_SHARED_EXPORT Q_DECL_IMPORT
#endif

#include "include/modelInterface.h"

class ALGORITHM_FMUWRAPPER_SHARED_EXPORT Algorithm_FmuWrapper {
public:
    ModelInterface *OpenPASS_CreateInstance(std::string componentName,
                                   bool isInit,
                                   int priority,
                                   int offsetTime,
                                   int responseTime,
                                   int cycleTime,
                                   StochasticsInterface *stochastics,
                                   WorldInterface *world,
                                   const ParameterInterface *parameters,
                                   PublisherInterface * const publisher,
                                   AgentInterface *agent,
                                   const CallbackInterface *callbacks);

    void OpenPASS_DestroyInstance(ModelInterface *implementation);
};


