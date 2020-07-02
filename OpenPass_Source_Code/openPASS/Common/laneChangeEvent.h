/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <vector>

#include "vehicleComponentEvent.h"
#include "openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the LaneChangeEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class LaneChangeEvent : public VehicleComponentEvent
{
public:
    LaneChangeEvent(int time,
                    const std::string eventName,
                    std::string source,
                    int agentId,
                    const openScenario::LaneChangeParameter laneChange):
        VehicleComponentEvent(time,
                              eventName,
                              source,
                              agentId),
        laneChange(laneChange)
    {}
    virtual ~LaneChangeEvent() = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::LaneChange;
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

        return eventParameters;
    }

    const openScenario::LaneChangeParameter laneChange;
};

