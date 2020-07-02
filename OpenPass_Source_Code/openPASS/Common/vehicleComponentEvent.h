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
                          const std::string &eventName,
                          std::string source,
                          int agentId) :
        BasicEvent(time,
                   eventName,
                   source,
                   {agentId},
                   {}),
        agentId(agentId)
    {
        parameter.emplace("Source", source);
        parameter.emplace("State", eventName);
    }

    VehicleComponentEvent(const VehicleComponentEvent&) = delete;
    VehicleComponentEvent(VehicleComponentEvent&&) = delete;
    VehicleComponentEvent& operator=(const VehicleComponentEvent&) = delete;
    VehicleComponentEvent& operator=(VehicleComponentEvent&&) = delete;
    virtual ~VehicleComponentEvent() override = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::VehicleComponent;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual EventParameters GetParametersAsString() override
    {
        return {{"AgentId", std::to_string(agentId)}};
    }

    int agentId;
};

