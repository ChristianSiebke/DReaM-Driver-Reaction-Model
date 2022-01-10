/********************************************************************************
 * Copyright (c) 2017 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  RemoveAgentsManipulator.cpp */
//-----------------------------------------------------------------------------

#include "LaneChangeManipulator.h"

#include "common/events/laneChangeEvent.h"

LaneChangeManipulator::LaneChangeManipulator(WorldInterface *world,
                                             core::EventNetworkInterface *eventNetwork,
                                             const CallbackInterface *callbacks,
                                             const openScenario::LaneChangeAction action,
                                             const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks,
                          eventName),
    action(action)
{
    cycleTime = 100;
}

void LaneChangeManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(eventInterface);

        for (const auto actorId : triggeringEvent->actingAgents.entities)
        {
            auto trigger = std::make_unique<openpass::events::LaneChangeEvent>(time, eventName, COMPONENTNAME, actorId, action.laneChangeParameter);
            eventNetwork->InsertTrigger(openpass::events::LaneChangeEvent::TOPIC, std::move(trigger));
        }
    }
}

EventContainer LaneChangeManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    const auto &conditionalEvents = eventNetwork->GetEvents(EventDefinitions::EventCategory::OpenSCENARIO);

    for (const auto &event : conditionalEvents)
    {
        const auto oscEvent = std::static_pointer_cast<openpass::events::OpenScenarioEvent>(event);

        if (oscEvent && oscEvent.get()->GetName() == eventName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
