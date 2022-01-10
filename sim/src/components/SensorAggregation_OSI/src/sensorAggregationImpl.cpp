/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \brief sensorAggregationImpl.cpp */
//-----------------------------------------------------------------------------

#include "sensorAggregationImpl.h"
#include <qglobal.h>

SensorAggregationImplementation::SensorAggregationImplementation(
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
    UnrestrictedModelInterface(
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

void SensorAggregationImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    if(time != previousTimeStamp) {
        out_sensorData.Clear();
        previousTimeStamp = time;
    }

    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetId() << ", input data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());

    const std::shared_ptr<SensorDataSignal const> signal = std::dynamic_pointer_cast<SensorDataSignal const>(data);
    if(!signal)
    {
        const std::string msg = COMPONENTNAME + " invalid signaltype";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }

    out_sensorData.MergeFrom(signal->sensorData);
}

void SensorAggregationImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetId() << ", output data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());


    if(localLinkId == 0)
    {
        // to any ADAS
        try
        {
            data = std::make_shared<SensorDataSignal const>(out_sensorData);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void SensorAggregationImplementation::Trigger(int time)
{
    Q_UNUSED(time);
}
