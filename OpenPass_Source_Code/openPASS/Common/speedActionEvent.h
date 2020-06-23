/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "vehicleComponentEvent.h"
#include "openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the SpeedActionEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class SpeedActionEvent : public VehicleComponentEvent
{
public:
    SpeedActionEvent(int time,
                    const std::string eventName,
                    std::string source,
                    int agentId,
                    const openScenario::SpeedAction speedAction):
        VehicleComponentEvent(time,
                              eventName,
                              source,
                              agentId),
        speedAction(speedAction)
    {}
    virtual ~SpeedActionEvent() override = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::SpeedAction;
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

    const openScenario::SpeedAction speedAction;
};

