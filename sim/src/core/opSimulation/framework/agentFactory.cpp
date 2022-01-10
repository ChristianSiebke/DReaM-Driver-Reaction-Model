/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <algorithm>
#include <list>
#include <sstream>

#include "agent.h"
#include "agentFactory.h"
#include "agentType.h"
#include "channel.h"
#include "channelBuffer.h"

#include "agentDataPublisher.h"
#include "bindings/modelBinding.h"
#include "spawnPoint.h"
#include "bindings/stochastics.h"

#include "common/log.h"
#include "modelElements/parameters.h"
#include "include/componentInterface.h"
#include "include/observationNetworkInterface.h"
#include "include/worldInterface.h"

const std::string AgentCategoryStrings[] =
{
    "Ego",
    "Scenario",
    "Common"
};

class DataBufferInterface;

namespace core
{

AgentFactory::AgentFactory(ModelBinding *modelBinding,
                           WorldInterface *world,
                           Stochastics *stochastics,
                           ObservationNetworkInterface *observationNetwork,
                           EventNetworkInterface *eventNetwork,
                           DataBufferWriteInterface* dataBuffer) :
    modelBinding(modelBinding),
    world(world),
    stochastics(stochastics),
    observationNetwork(observationNetwork),
    eventNetwork(eventNetwork),
    dataBuffer(dataBuffer)
{
}

void AgentFactory::Clear()
{
    agentList.clear();
}

Agent* AgentFactory::AddAgent(AgentBlueprintInterface* agentBlueprint)
{
    try
    {
        auto agent = CreateAgent(*agentBlueprint);
        world->RegisterAgent(agent->GetAgentAdapter());
        PublishProperties(*agent);
        agentList.push_back(std::move(agent));
        return agentList.back().get();
    }
    catch (const std::runtime_error& e)
    {
        LOG_INTERN(LogLevel::Error) << "could not create agent: " << e.what();
        return nullptr;
    }
}

std::unique_ptr<Agent> AgentFactory::CreateAgent(const AgentBlueprintInterface& agentBlueprint)
{
    LOG_INTERN(LogLevel::DebugCore) << "create new agent";

    auto agent = std::make_unique<Agent>(world, agentBlueprint);

    if (!agent)
    {
        LOG_INTERN(LogLevel::Error) << "agent creation failed";
        return nullptr;
    }
    else
    {
        LOG_INTERN(LogLevel::DebugCore) << "agent created (" << agent->GetId() << ")";
    }

    if (!agent->Instantiate(agentBlueprint,
                            modelBinding,
                            stochastics,
                            observationNetwork,
                            eventNetwork,
                            dataBuffer))
    {
        LOG_INTERN(LogLevel::Error) << "agent could not be instantiated";
        return nullptr;
    }

    // link agent internal components
    if (!ConnectAgentLinks(agent.get()))
    {
        LOG_INTERN(LogLevel::Error) << "agent channels could not be created";
        return nullptr;
    }

    return agent;
}

bool AgentFactory::ConnectAgentLinks(Agent *agent)
{
    for(const std::pair<const std::string, ComponentInterface*> &itemComponent : agent->GetComponents())
    {
        if(!itemComponent.second)
        {
            return false;
        }

        for(const std::pair<const int, Channel*> &itemChannel : itemComponent.second->GetOutputLinks())
        {
            int outputLinkId = itemChannel.first;
            Channel *channel = itemChannel.second;
            if(!channel)
            {
                return false;
            }

            ComponentInterface *source = channel->GetSource();
            if(!source)
            {
                return false;
            }

            ChannelBuffer *buffer = source->CreateOutputBuffer(outputLinkId);
            if(!buffer || !(channel->AttachSourceBuffer(buffer)))
            {
                return false;
            }

            // channel buffer is now attached to channel and will be released when deleting the agent
            for(const std::tuple<int, ComponentInterface*> &item : channel->GetTargets())
            {
                int targetLinkId = std::get<static_cast<size_t>(Channel::TargetLinkType::LinkId)>(item);
                ComponentInterface *targetComponent = std::get<static_cast<size_t>(Channel::TargetLinkType::Component)>(item);
                targetComponent->SetInputBuffer(targetLinkId, buffer);
            }
        }
    }

    return true;
}

void AgentFactory::PublishProperties(const Agent& agent)
{
    const auto adapter = agent.GetAgentAdapter();
    const std::string keyPrefix = "Agents/" + std::to_string(agent.GetId()) + "/";
    dataBuffer->PutStatic(keyPrefix + "AgentTypeGroupName", AgentCategoryStrings[static_cast<int>(adapter->GetAgentCategory())]);
    dataBuffer->PutStatic(keyPrefix + "AgentTypeName", adapter->GetAgentTypeName());
    dataBuffer->PutStatic(keyPrefix + "VehicleModelType", adapter->GetVehicleModelType());
    dataBuffer->PutStatic(keyPrefix + "DriverProfileName", adapter->GetDriverProfileName());
    dataBuffer->PutStatic(keyPrefix + "AgentType", std::string(openpass::utils::to_cstr(adapter->GetVehicleModelParameters().vehicleType)));   // std::string for compatibility with gcc-9 std::ariant

    const auto& vehicleModelParameters = adapter->GetVehicleModelParameters();
    dataBuffer->PutStatic(keyPrefix + "Vehicle/Width", vehicleModelParameters.boundingBoxDimensions.width);
    dataBuffer->PutStatic(keyPrefix + "Vehicle/Length", vehicleModelParameters.boundingBoxDimensions.length);
    dataBuffer->PutStatic(keyPrefix + "Vehicle/Height", vehicleModelParameters.boundingBoxDimensions.height);
    dataBuffer->PutStatic(keyPrefix + "Vehicle/LongitudinalPivotOffset", -vehicleModelParameters.boundingBoxCenter.x);

    for (const auto& sensor : adapter->GetSensorParameters())
    {
        const std::string sensorKeyPrefix = keyPrefix + "Vehicle/Sensors/" + std::to_string(sensor.id) + "/";
        dataBuffer->PutStatic(sensorKeyPrefix + "Type", sensor.profile.type);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Position/Longitudinal", sensor.position.longitudinal);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Position/Lateral", sensor.position.lateral);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Position/Height", sensor.position.height);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Orientation/Yaw", sensor.position.yaw);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Orientation/Pitch", sensor.position.pitch);
        dataBuffer->PutStatic(sensorKeyPrefix + "Mounting/Orientation/Roll", sensor.position.roll);

        const auto& parameters = sensor.profile.parameter;

        if (auto latency = openpass::parameter::Get<double>(parameters, "Latency"))
        {
            dataBuffer->PutStatic(sensorKeyPrefix + "Parameters/Latency", latency.value());
        }

        if (auto openingAngleH = openpass::parameter::Get<double>(parameters, "OpeningAngleH"))
        {
            dataBuffer->PutStatic(sensorKeyPrefix + "Parameters/OpeningAngleH", openingAngleH.value());
        }

        if (auto openingAngleV = openpass::parameter::Get<double>(parameters, "OpeningAngleV"))
        {
            dataBuffer->PutStatic(sensorKeyPrefix + "Parameters/OpeningAngleV", openingAngleV.value());
        }

        if (auto detectionRange = openpass::parameter::Get<double>(parameters, "DetectionRange"))
        {
            dataBuffer->PutStatic(sensorKeyPrefix + "Parameters/Range", detectionRange.value());
        }
    }
}

} // namespace core
