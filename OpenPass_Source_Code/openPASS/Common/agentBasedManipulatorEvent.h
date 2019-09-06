/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  AgentBasedEvent.h
* @brief This file contains all functions for agent based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "basicEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the AgentBasedEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class AgentBasedManipulationEvent : public BasicEvent
{
public:
    AgentBasedManipulationEvent(int time,
                    std::string source,
                    std::string sequenceName,
                    EventDefinitions::EventType eventType,
                    std::vector<int> triggeringAgents,
                    std::vector<int> actingAgents):
        BasicEvent(time,
                   source,
                   sequenceName,
                   eventType),
        triggeringAgents(triggeringAgents),
        actingAgents(actingAgents)
    {}
    AgentBasedManipulationEvent(const AgentBasedManipulationEvent&) = delete;
    AgentBasedManipulationEvent(AgentBasedManipulationEvent&&) = delete;
    AgentBasedManipulationEvent& operator=(const AgentBasedManipulationEvent&) = delete;
    AgentBasedManipulationEvent& operator=(AgentBasedManipulationEvent&&) = delete;
    ~AgentBasedManipulationEvent() = default;

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    std::list<std::pair<std::string, std::string>> GetEventParametersAsString()
    {
        std::list<std::pair<std::string, std::string>> eventParameters;

        std::string triggeringAgentsAsString{};
        for (auto &triggeringAgentId : triggeringAgents) {
            triggeringAgentsAsString += std::to_string(triggeringAgentId) + ",";
        }

        std::string actingAgentsAsString;
        for (auto &actingAgentId : actingAgents) {
            actingAgentsAsString += std::to_string(actingAgentId) + ",";
        }

        if(triggeringAgentsAsString.size() > 0)
        {
            triggeringAgentsAsString.pop_back();
            eventParameters.push_back(std::pair<std::string, std::string>("TriggeringAgentIds", triggeringAgentsAsString));
        }
        if(actingAgentsAsString.size() > 0)
        {
            actingAgentsAsString.pop_back();
            eventParameters.push_back(std::pair<std::string, std::string>("ActingAgentIds", actingAgentsAsString));
        }

        return eventParameters;
    }

    const std::vector<int> triggeringAgents;
    const std::vector<int> actingAgents;
};

