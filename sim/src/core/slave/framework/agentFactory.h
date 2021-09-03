/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  AgentFactory.h
//! @brief This file contains the singleton which creates agent instances
//!
//! The agent instances are created based on the agent types which are given by
//! configuration. Agent instances will be created on the fly during the
//! simulation run by spawn points.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <list>
#include <memory>

#include "include/agentFactoryInterface.h"
#include "include/eventNetworkInterface.h"
#include "include/worldInterface.h"
#include "common/opExport.h"

class DataBufferWriteInterface;

namespace SimulationSlave
{
class Agent;
class AgentType;
class ModelBinding;
class Stochastics;
class SpawnItemParameter;
class ObservationNetworkInterface;

class CORESLAVEEXPORT AgentFactory : public AgentFactoryInterface
{
public:
    AgentFactory(ModelBinding *modelBinding,
                 WorldInterface *world,
                 Stochastics *stochastics,
                 ObservationNetworkInterface *observationNetwork,
                 SimulationSlave::EventNetworkInterface *eventNetwork,
                 DataBufferWriteInterface* dataBuffer);
    virtual ~AgentFactory() override = default;

    virtual void Clear() override;
    virtual Agent *AddAgent(AgentBlueprintInterface* agentBlueprint) override;

private:
    //-----------------------------------------------------------------------------
    //! @brief Links all channels of the agent components.
    //!
    //! Gets the channels of the agent from all its components' output links,
    //! creates for each a channel buffer using its source and attaches it to the
    //! channel.
    //!
    //!
    //! @param[in]  agent               The agent whose channels are connected
    //!
    //! @return                         Flag if the connect was successful
    //-----------------------------------------------------------------------------
    bool ConnectAgentLinks(Agent *agent);

    //-----------------------------------------------------------------------------
    //! @brief Creates a new agent based on the provided parameters,
    //!        initializes it, and links its agent channels.
    //!
    //! @param[in]  agentBlueprint      agentBlueprint contains all necessary
    //!                                 informations to create an agent
    //! @return                         The created agent
    //-----------------------------------------------------------------------------
    std::unique_ptr<Agent> CreateAgent(const AgentBlueprintInterface& agentBlueprint);

    void PublishProperties(const Agent& agent);

    ModelBinding *modelBinding;
    WorldInterface *world;
    Stochastics *stochastics;
    ObservationNetworkInterface *observationNetwork;
    EventNetworkInterface *eventNetwork;
    DataBufferWriteInterface *dataBuffer;

    std::vector<std::unique_ptr<Agent>> agentList;
};

} // namespace SimulationSlave


