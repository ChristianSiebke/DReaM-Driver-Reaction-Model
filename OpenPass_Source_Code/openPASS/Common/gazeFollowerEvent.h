/*******************************************************************************
* Copyright (c) 2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  GazeFollowerEvent.h
* @brief This class implements all functionality of the GazeFollowerEvent.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include "vehicleComponentEvent.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the GazeFollowerEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class GazeFollowerEvent : public VehicleComponentEvent
{
public:
    GazeFollowerEvent(int time,
                      const std::string eventName,
                      std::string source,
                      int agentId,
                      const std::string gazeActivityState,
                      const std::string gazeFileName):
        VehicleComponentEvent(time,
                              eventName,
                              source,
                              agentId),
        gazeActivityState(gazeActivityState),
        gazeFileName(gazeFileName)

    {}
    GazeFollowerEvent(const GazeFollowerEvent&) = delete;
    GazeFollowerEvent(GazeFollowerEvent&&) = delete;
    GazeFollowerEvent& operator=(const GazeFollowerEvent&) = delete;
    GazeFollowerEvent& operator=(GazeFollowerEvent&&) = delete;
    virtual ~GazeFollowerEvent() override = default;

    /*!
    * \brief Returns the category of the event.
    *
    * @return	     EventCategory.
    */
    virtual EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::SetGazeFollower;
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

        eventParameters.push_back({"GazeActivityState", gazeActivityState});
        eventParameters.push_back({"GazeFileName", gazeFileName});

        return eventParameters;
    }

    const std::string gazeActivityState;
    const std::string gazeFileName;
};
