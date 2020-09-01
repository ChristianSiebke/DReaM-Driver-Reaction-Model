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
/** This class hold information about a  Component State Change Event
 *
 * \ingroup Event */
//-----------------------------------------------------------------------------

class ComponentStateChangeEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"OpenSCENARIO/UserDefinedAction/CustomCommandAction/SetComponentState"};

    ComponentStateChangeEvent(int time, std::string eventName, std::string source,
                     TriggeringEntities triggeringAgents, AffectedEntities actingAgents,
                     const std::string componentName, ComponentState componentState) :
        OpenScenarioEvent{time, eventName, source, triggeringAgents, actingAgents},
        componentName{std::move(componentName)},
        componentState{std::move(componentState)}
    {
    }

    const std::string componentName;
    const ComponentState componentState;
};

} // namespace openpass::events
