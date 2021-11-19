/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  TrajectoryEvent.h
* @brief This file contains all functions for agent based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------

#pragma once

#include "common/events/basicEvent.h"
#include "common/openScenarioDefinitions.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** This class implements all functionality of the Trajectory Event.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class TrajectoryEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"OpenSCENARIO/Trajectory"};

    TrajectoryEvent(int time, const std::string eventName, const std::string source, int agentId, const openScenario::Trajectory trajectory):
        OpenScenarioEvent{time, std::move(eventName), std::move(source), {}, {{agentId}}},
        trajectory{std::move(trajectory)}
    {}

    const openScenario::Trajectory trajectory;
};

} // namespace openpass::events

