/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  TrajectoryEvent.h
* @brief This file contains all functions for agent based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include <vector>

#include "vehicleComponentEvent.h"
#include "openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the AgentBasedEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class TrajectoryEvent : public VehicleComponentEvent
{
public:
    TrajectoryEvent(int time,
                    std::string source,
                    const std::string eventName,
                    int agentId,
                    const openScenario::Trajectory& trajectory):
        VehicleComponentEvent(time,
                   source,
                   eventName,
                   agentId),
        trajectory(trajectory)
    {}
    TrajectoryEvent(const TrajectoryEvent&) = delete;
    TrajectoryEvent(TrajectoryEvent&&) = delete;
    TrajectoryEvent& operator=(const TrajectoryEvent&) = delete;
    TrajectoryEvent& operator=(TrajectoryEvent&&) = delete;
    virtual ~TrajectoryEvent() = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::SetTrajectory;
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

        eventParameters.push_back({"TrajectoryName", trajectory.name});

        return eventParameters;
    }

    const openScenario::Trajectory trajectory;
};

