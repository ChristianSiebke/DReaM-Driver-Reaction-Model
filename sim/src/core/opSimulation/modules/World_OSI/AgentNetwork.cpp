/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <algorithm>
#include <string>
#include <QFile>
#include "AgentAdapter.h"
#include "AgentNetwork.h"
#include "WorldImplementation.h"

AgentNetwork::AgentNetwork(WorldImplementation* world, const CallbackInterface* callbacks) :
    world(world),
    callbacks(callbacks)
{}

AgentNetwork::~AgentNetwork()
{
    Clear();
}

void AgentNetwork::Clear()
{
    updateQueue.clear();
    removeQueue.clear();

    for (const std::pair<const int, AgentInterface*>& item : agents)
    {
        delete item.second;
    }

    agents.clear();

    for (const auto& removedAgent : removedAgents)
    {
        delete removedAgent;
    }

    removedAgents.clear();
    removedAgentsPrevious.clear();
}

void AgentNetwork::AddAgent(AgentInterface* agent)
{
    if (!agents.insert({agent->GetId(), agent}).second)
    {

        LOG(CbkLogLevel::Error, "Agent Ids must be unique");
        throw std::runtime_error("Agent Ids must be unique");
    }
}

AgentInterface* AgentNetwork::GetAgent(int id) const
{
    if (agents.find(id) == agents.end())
    {
        return nullptr;
    }

    return agents.at(id);
}

const std::map<int, AgentInterface*>& AgentNetwork::GetAgents() const
{
    return agents;
}

const AgentInterfaces& AgentNetwork::GetRemovedAgents() const
{
    return removedAgents;
}

const AgentInterfaces AgentNetwork::GetRemovedAgentsInPreviousTimestep()
{
    auto agents = std::move(removedAgentsPrevious);
    removedAgentsPrevious.clear();
    return agents;
}

void AgentNetwork::QueueAgentUpdate(std::function<void()> func)
{
    updateQueue.push_back(func);
}

void AgentNetwork::QueueAgentRemove(const AgentInterface* agent)
{
    removeQueue.push_back(agent);
}

void AgentNetwork::RemoveAgent(const AgentInterface* agent)
{

    if (1 != agents.erase(agent->GetId()))
    {
        LOG(CbkLogLevel::Warning, "trying to remove non-existent agent");
    }

    removedAgents.push_back(agent);
    removedAgentsPrevious.push_back(agent);
    agent->Unregister();
}

void AgentNetwork::PublishGlobalData(Publisher publish)
{
    for (const auto& [_, agent] : agents)
    {
        const openpass::type::EntityId agentId = agent->GetId();

        publish(agentId, "XPosition", agent->GetPositionX());
        publish(agentId, "YPosition", agent->GetPositionY());
        publish(agentId, "VelocityEgo", agent->GetVelocity());
        publish(agentId, "AccelerationEgo", agent->GetAcceleration());
        publish(agentId, "YawAngle", agent->GetYaw());
        publish(agentId, "RollAngle", agent->GetRoll());
        publish(agentId, "YawRate", agent->GetYawRate());
        publish(agentId, "SteeringAngle", agent->GetSteeringWheelAngle());
        publish(agentId, "TotalDistanceTraveled", agent->GetDistanceTraveled());

        const auto& egoAgent = agent->GetEgoAgent();
        if (egoAgent.HasValidRoute())
        {
            publish(agentId, "PositionRoute", egoAgent.GetMainLocatePosition().roadPosition.s);
            publish(agentId, "TCoordinate", egoAgent.GetPositionLateral());
            publish(agentId, "Lane", egoAgent.GetMainLocatePosition().laneId);
            publish(agentId, "Road", egoAgent.GetRoadId());
            publish(agentId, "SecondaryLanes", agent->GetObjectPosition().touchedRoads.at(egoAgent.GetRoadId()).lanes);
        }
        else
        {
            publish(agentId, "PositionRoute", NAN );
            publish(agentId, "TCoordinate", NAN );
            publish(agentId, "Lane", NAN );
            publish(agentId, "Road", NAN);
            publish(agentId, "SecondaryLanes", std::vector<int>{});
            publish(agentId, "AgentInFront", NAN);
        }
    }
}

// udpate global data at occurrence of time step
void AgentNetwork::SyncGlobalData()
{
    for (const auto& func : updateQueue)
    {
        func();
    }
    updateQueue.clear();

    for (auto& agent : removeQueue)
    {
        world->RemoveAgent(agent);
    }
    removeQueue.clear();

    auto currentAgents = agents; //make a copy, because agents is manipulated inside the loop
    for (auto& item : currentAgents)
    {
        AgentInterface* agent = item.second;

        agent->Unlocate();

        if (!agent->Update())
        {
            LOG(CbkLogLevel::Warning, "Could not locate agent");
        }

        if (!agent->IsAgentInWorld())
        {
            world->RemoveAgent(agent);
            continue;
        }
    }
}
