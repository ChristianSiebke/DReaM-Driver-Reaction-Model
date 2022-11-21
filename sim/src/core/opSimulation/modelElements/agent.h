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

//-----------------------------------------------------------------------------
//! @file  Agent.h
//! @brief This file contains the internal representation of an agent instance
//!        during a simulation run.
//-----------------------------------------------------------------------------

#pragma once

#include <functional>
#include <algorithm>
#include <utility>
#include <map>
#include <list>

#include "include/stochasticsInterface.h"
#include "include/agentInterface.h"
#include "include/componentInterface.h"

class DataBufferWriteInterface;
class AgentBlueprintInterface;

namespace core
{

class Channel;
class ModelBinding;
class ModelParameters;
class SpawnPoint;
class ObservationNetworkInterface;
class SpawnItemParameter;

class Agent
{
public:
    Agent(WorldInterface *world, const AgentBlueprintInterface& agentBlueprint);
    Agent(const Agent&) = delete;
    Agent(Agent&&) = delete;
    Agent& operator=(const Agent&) = delete;
    Agent& operator=(Agent&&) = delete;
    virtual ~Agent();

    bool AddChannel(int id, Channel *channel);
    bool AddComponent(std::string name, core::ComponentInterface *component);
    Channel *GetChannel(int id) const;
    ComponentInterface *GetComponent(std::string name) const;
    const std::map<std::string, ComponentInterface*> &GetComponents() const;

    int GetId() const
    {
        return id;
    }

    bool Instantiate(const AgentBlueprintInterface& agentBlueprint,
                     ModelBinding *modelBinding,
                     StochasticsInterface *stochastics,
                     core::ObservationNetworkInterface *observationNetwork,
                     EventNetworkInterface *eventNetwork,
                     DataBufferWriteInterface* dataBuffer);

    AgentInterface* GetAgentAdapter() const;

    void SetAgentAdapter(AgentInterface *agentAdapt);

    void LinkSchedulerTime(int* const schedulerTime);

private:
    WorldInterface *world = nullptr;
    int id;

    std::vector<int> idsCollisionPartners;
    std::map<int, Channel*> channels;
    std::map<std::string, ComponentInterface*> components;

    AgentInterface* agentInterface = nullptr;
    std::unique_ptr<PublisherInterface> publisher;

    int* currentTime = nullptr;
};

} // namespace core


