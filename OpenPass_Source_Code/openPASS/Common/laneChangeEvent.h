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
/** @file  LaneChangeEvent.h
* @brief This file contains all functions for agent based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "vehicleComponentEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the AgentBasedEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class LaneChangeEvent : public VehicleComponentEvent
{
public:
    LaneChangeEvent(int time,
                    std::string source,
                    std::string sequenceName,
                    EventDefinitions::EventType eventType,
                    int agentId,
                    const int deltaLaneId):
        VehicleComponentEvent(time,
                   source,
                   sequenceName,
                   eventType,
                   agentId),
        deltaLaneId(deltaLaneId)
    {}
    LaneChangeEvent(const LaneChangeEvent&) = delete;
    LaneChangeEvent(LaneChangeEvent&&) = delete;
    LaneChangeEvent& operator=(const LaneChangeEvent&) = delete;
    LaneChangeEvent& operator=(LaneChangeEvent&&) = delete;
    virtual ~LaneChangeEvent() = default;

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual std::list<std::pair<std::string, std::string>> GetEventParametersAsString()
    {
        auto eventParameters = VehicleComponentEvent::GetEventParametersAsString();

        eventParameters.push_back(std::pair<std::string, std::string>("DeltaLaneId", std::to_string(deltaLaneId)));

        return eventParameters;
    }

    const int deltaLaneId;
};

