/*******************************************************************************
* Copyright (c) 2017 - 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Common/Events/basicEvent.h"
#include "Common/openScenarioDefinitions.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** This class implements all functionality of the LaneChangeEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class LaneChangeEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"OpenSCENARIO/PrivateAction/LateralAction/LaneChangeAction"};

    LaneChangeEvent(int time, const std::string eventName, const std::string source, int agentId, const openScenario::LaneChangeParameter laneChange):
        OpenScenarioEvent{time, std::move(eventName), std::move(source), {}, {{agentId}}},
        laneChange{std::move(laneChange)}
    {}

    const openScenario::LaneChangeParameter laneChange;
};

} // namespace openpass::events
