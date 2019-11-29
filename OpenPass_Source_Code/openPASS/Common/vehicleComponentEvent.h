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
/** @file  VehicleComponentEvent.h
* @brief This file contains all functions for vehicle component events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "basicEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the VehicleComponentEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class VehicleComponentEvent : public BasicEvent
{
public:
    VehicleComponentEvent(int time,
                    std::string source,
                    std::string sequenceName,
                    EventDefinitions::EventType eventType,
                    int agentId):
        BasicEvent(time,
                   source,
                   sequenceName,
                   eventType),
        agentId(agentId)
    {}
    VehicleComponentEvent(const VehicleComponentEvent&) = delete;
    VehicleComponentEvent(VehicleComponentEvent&&) = delete;
    VehicleComponentEvent& operator=(const VehicleComponentEvent&) = delete;
    VehicleComponentEvent& operator=(VehicleComponentEvent&&) = delete;
    virtual ~VehicleComponentEvent() = default;

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual std::list<std::pair<std::string, std::string>> GetEventParametersAsString()
    {
        std::list<std::pair<std::string, std::string>> eventParameters;

        eventParameters.push_back(std::pair<std::string, std::string>("AgentId", std::to_string(agentId)));

        return eventParameters;
    }

    int agentId;
};

