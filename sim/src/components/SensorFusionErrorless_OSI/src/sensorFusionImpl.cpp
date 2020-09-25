/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \brief sensorFusionImpl.cpp */
//-----------------------------------------------------------------------------

#include "sensorFusionImpl.h"
#include <qglobal.h>

SensorFusionErrorlessImplementation::SensorFusionErrorlessImplementation(
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

void SensorFusionErrorlessImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, [[maybe_unused]] int time)
{
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

    MergeSensorData(signal->sensorData);
}

void SensorFusionErrorlessImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, [[maybe_unused]] int time)
{
    std::stringstream log;
    log << COMPONENTNAME << " (component " << GetComponentName() << ", agent " << GetAgent()->GetId() << ", output data for local link " << localLinkId << ": ";
    LOG(CbkLogLevel::Debug, log.str());


    if(localLinkId == 0)
    {
        // to any ADAS
        try
        {
            data = std::make_shared<SensorDataSignal const>(
                        out_sensorData);
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

void SensorFusionErrorlessImplementation::Trigger(int)
{
}

void SensorFusionErrorlessImplementation::MergeSensorData(const osi3::SensorData& in_SensorData)
{
    out_sensorData = {};
    for (auto& movingObject : in_SensorData.moving_object())
    {
        auto existingObject = std::find_if(out_sensorData.mutable_moving_object()->begin(), out_sensorData.mutable_moving_object()->end(),
                                           [&](const auto& object){return movingObject.header().ground_truth_id(0).value() == object.header().ground_truth_id(0).value();});
        if (existingObject != out_sensorData.mutable_moving_object()->end())
        {
            existingObject->mutable_header()->mutable_sensor_id()->MergeFrom(movingObject.header().sensor_id());
        }
        else
        {
            out_sensorData.add_moving_object()->CopyFrom(movingObject);
        }
    }
    for (auto& stationaryObject : in_SensorData.stationary_object())
    {
        auto existingObject = std::find_if(out_sensorData.mutable_stationary_object()->begin(), out_sensorData.mutable_stationary_object()->end(),
                                           [&](const auto& object){return stationaryObject.header().ground_truth_id(0).value() == object.header().ground_truth_id(0).value();});
        if (existingObject != out_sensorData.mutable_stationary_object()->end())
        {
            existingObject->mutable_header()->mutable_sensor_id()->MergeFrom(stationaryObject.header().sensor_id());
        }
        else
        {
            out_sensorData.add_stationary_object()->CopyFrom(stationaryObject);
        }
    }
}
