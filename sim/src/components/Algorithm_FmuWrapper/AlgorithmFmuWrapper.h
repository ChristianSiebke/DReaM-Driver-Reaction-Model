/********************************************************************************
 * Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *               2017 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <QtCore/qglobal.h>

#include "include/modelInterface.h"

#if defined(ALGORITHM_FMUWRAPPER_LIBRARY)
#define ALGORITHM_FMUWRAPPER_SHARED_EXPORT Q_DECL_EXPORT
#else
#define ALGORITHM_FMUWRAPPER_SHARED_EXPORT Q_DECL_IMPORT
#endif

extern "C"
{
    ALGORITHM_FMUWRAPPER_SHARED_EXPORT const std::string &OpenPASS_GetVersion();

    ALGORITHM_FMUWRAPPER_SHARED_EXPORT ModelInterface *OpenPASS_CreateInstance(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface *const publisher,
        AgentInterface *agent,
        const CallbackInterface *callbacks);

    ALGORITHM_FMUWRAPPER_SHARED_EXPORT void OpenPASS_DestroyInstance(
        ModelInterface *implementation);

    ALGORITHM_FMUWRAPPER_SHARED_EXPORT bool OpenPASS_UpdateInput(
        ModelInterface *implementation,
        int localLinkId,
        const std::shared_ptr<SignalInterface const> &data,
        int time);

    ALGORITHM_FMUWRAPPER_SHARED_EXPORT bool OpenPASS_UpdateOutput(
        ModelInterface *implementation,
        int localLinkId,
        std::shared_ptr<SignalInterface const> &data,
        int time);

    ALGORITHM_FMUWRAPPER_SHARED_EXPORT bool OpenPASS_Trigger(
        ModelInterface *implementation,
        int time);
}
