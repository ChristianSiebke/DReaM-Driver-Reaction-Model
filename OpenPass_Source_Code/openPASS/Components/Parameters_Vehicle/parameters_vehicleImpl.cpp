/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*               2016, 2017 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  parameters_vehicleImpl.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>

#include "parameters_vehicleImpl.h"
#include "Common/parametersVehicleSignal.h"

ParametersVehicleImplementation::ParametersVehicleImplementation(
    std::string componentName,
    bool isInit,
    int priority,
    int offsetTime,
    int responseTime,
    int cycleTime,
    StochasticsInterface* stochastics,
    WorldInterface* world,
    const ParameterInterface* parameters,
    PublisherInterface * const publisher,
    const CallbackInterface* callbacks,
    AgentInterface* agent) :
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
}

void ParametersVehicleImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data,
        int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void ParametersVehicleImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data,
        int time)
{
    Q_UNUSED(time);

    try
    {
        switch (localLinkId)
        {
            case 1:
                // vehicle parameters
                data = std::make_shared<ParametersVehicleSignal const>(GetAgent()->GetVehicleModelParameters());
                break;

            default:
                const std::string msg = COMPONENTNAME + " invalid link";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
        }
    }
    catch (const std::bad_alloc&)
    {
        const std::string msg = COMPONENTNAME + " could not instantiate signal";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void ParametersVehicleImplementation::Trigger(int time)
{
    Q_UNUSED(time);
}
