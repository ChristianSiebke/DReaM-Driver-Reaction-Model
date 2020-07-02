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

#include "Common/Events/basicEvent.h"
#include "Common/openScenarioDefinitions.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** This class implements all functionality of the SpeedActionEvent.
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class SpeedActionEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"openSCENARIO/LongitudinalAction/SpeedAction"};

    SpeedActionEvent(int time, const std::string eventName, const std::string source, int agentId, const openScenario::SpeedAction speedAction) :
        OpenScenarioEvent(time, std::move(eventName), std::move(source), {}, {{agentId}}),
        speedAction(std::move(speedAction))
    {
    }

    const openScenario::SpeedAction speedAction;
};

} // namespace openpass::events
