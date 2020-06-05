/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  SensorRecordStateImplementation.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>

#include "Interfaces/worldInterface.h"
#include "Interfaces/egoAgentInterface.h"

#include "sensor_recordStateImplementation.h"

SensorRecordStateImplementation::SensorRecordStateImplementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface * const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
    SensorInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent)
{
    if (GetPublisher() == nullptr)
    {
        const std::string msg = COMPONENTNAME + " invalid publisher module setup";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void SensorRecordStateImplementation::UpdateInput(int, const std::shared_ptr<SignalInterface const> &, int)
{
}

void SensorRecordStateImplementation::UpdateOutput(int ,std::shared_ptr<SignalInterface const> &, int)
{
}

void SensorRecordStateImplementation::Trigger([[maybe_unused]] int time)
{
}
