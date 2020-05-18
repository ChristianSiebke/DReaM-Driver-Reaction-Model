/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  CustomLaneChangeEvent.h
* @brief This class implements all functionality of the CustomLaneChangeEvent
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
class CustomLaneChangeEvent : public VehicleComponentEvent
{
public:
    CustomLaneChangeEvent(int time,
                    const std::string eventName,
                    std::string source,
                    int agentId,
                    const int deltaLaneId):
        VehicleComponentEvent(time,
                              eventName,
                              source,
                              agentId),
        deltaLaneId(deltaLaneId)
    {}
    CustomLaneChangeEvent(const CustomLaneChangeEvent&) = delete;
    CustomLaneChangeEvent(CustomLaneChangeEvent&&) = delete;
    CustomLaneChangeEvent& operator=(const CustomLaneChangeEvent&) = delete;
    CustomLaneChangeEvent& operator=(CustomLaneChangeEvent&&) = delete;
    virtual ~CustomLaneChangeEvent() = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::CustomLaneChange;
    }

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the agentId as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual EventParameters GetParametersAsString() override
    {
        auto eventParameters = VehicleComponentEvent::GetParametersAsString();

        eventParameters.push_back({"DeltaLaneId", std::to_string(deltaLaneId)});

        return eventParameters;
    }

    const int deltaLaneId;
};
