/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  RemoveAgentsManipulator.cpp */
//-----------------------------------------------------------------------------

#include "LaneChangeManipulator.h"
#include "Common/laneChangeEvent.h"

#include <QtGlobal>

LaneChangeManipulator::LaneChangeManipulator(WorldInterface *world,
                                             std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> action,
                                             SimulationSlave::EventNetworkInterface *eventNetwork,
                                             const CallbackInterface *callbacks):
    ManipulatorCommonBase(world,
                          action,
                          eventNetwork,
                          callbacks),
    action(action)
{
    cycleTime = 100;
    eventType = EventDefinitions::EventType::LaneChange;
}

void LaneChangeManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        std::shared_ptr<AgentBasedManipulationEvent> triggeringEvent = std::dynamic_pointer_cast<AgentBasedManipulationEvent>(eventInterface);

        for(const auto actorId : triggeringEvent->actingAgents)
        {
            std::shared_ptr<LaneChangeEvent> laneChangeEvent = std::make_shared<LaneChangeEvent>(time,
                                                                                                 COMPONENTNAME,
                                                                                                 sequenceName,
                                                                                                 eventType,
                                                                                                 actorId,
                                                                                                 action->GetValue());

            eventNetwork->InsertEvent(laneChangeEvent);
        }
    }
}

EventContainer LaneChangeManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    const auto &conditionalEvents = eventNetwork->GetActiveEventCategory(EventDefinitions::EventCategory::AgentBasedManipulation);

    for(const auto &event: conditionalEvents)
    {
        const auto conditionalEvent = std::static_pointer_cast<AgentBasedManipulationEvent>(event);

        if(conditionalEvent && conditionalEvent.get()->GetSequenceName() == sequenceName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
