/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <algorithm>
#include <string>
#include <QFile>
#include "AgentAdapter.h"
#include "AgentNetwork.h"

AgentNetwork::AgentNetwork(WorldInterface* world, const CallbackInterface* callbacks) :
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

bool AgentNetwork::AddAgent(int id, AgentInterface* agent)
{
    if (!agents.insert({id, agent}).second)
    {
        LOG(CbkLogLevel::Warning, "agents must be unique");
        return false;
    }

    return true;
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

const std::list<const AgentInterface*>& AgentNetwork::GetRemovedAgents() const
{
    return removedAgents;
}

const std::list<const AgentInterface*> AgentNetwork::GetRemovedAgentsInPreviousTimestep()
{
    auto agents = std::move(removedAgentsPrevious);
    removedAgentsPrevious.clear();
    return agents;
}

void AgentNetwork::QueueAgentUpdate(std::function<void()> func)
{
    updateQueue.push_back(func);
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
        publish(agentId, "YawRate", agent->GetYawRate());
        publish(agentId, "SteeringAngle", agent->GetSteeringWheelAngle());
        publish(agentId, "TotalDistanceTraveled", agent->GetDistanceTraveled());

        const auto& egoAgent = agent->GetEgoAgent();
        if (egoAgent.HasValidRoute())
        {
            const auto frontAgents = egoAgent.GetAgentsInRange(0, std::numeric_limits<double>::max(), 0);

            publish(agentId, "PositionRoute", egoAgent.GetMainLocatePosition().roadPosition.s);
            publish(agentId, "TCoordinate", egoAgent.GetPositionLateral());
            publish(agentId, "Lane", egoAgent.GetMainLocatePosition().laneId);
            publish(agentId, "Road", egoAgent.GetRoadId());
            publish(agentId, "SecondaryLanes", agent->GetObjectPosition().touchedRoads.at(egoAgent.GetRoadId()).lanes);
            publish(agentId, "AgentInFront", frontAgents.empty() ? -1 : frontAgents.front()->GetId());
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
    while (!updateQueue.empty())
    {
        const auto& func = updateQueue.front();
        func();
        updateQueue.pop_front();
    }

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
