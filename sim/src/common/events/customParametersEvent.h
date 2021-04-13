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

#include "common/events/basicEvent.h"
#include "include/signalInterface.h"

namespace openpass::events {

//-----------------------------------------------------------------------------
/** This class hold information about a Custom Parameters Event
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------
class CustomParametersEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"OpenSCENARIO/UserDefinedAction/CustomCommandAction/SetParameters"};

    CustomParametersEvent(int time, const std::string eventName, std::string source, const int agentId, std::vector<std::string> parameters):
        OpenScenarioEvent{time, eventName, source, {}, {{agentId}}},
        parameters(parameters)
    {}

    const std::vector<std::string> parameters;
};

} // namespace openpass::events
