/*******************************************************************************
* Copyright (c) 2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "AcquirePositionManipulator.h"

#include <src/common/events/acquirePositionEvent.h>
#include <src/common/events/basicEvent.h>
#include <src/common/openScenarioDefinitions.h>
#include <utility>

AcquirePositionManipulator::AcquirePositionManipulator(WorldInterface *world,
                                                       core::EventNetworkInterface *eventNetwork,
                                                       const CallbackInterface *callbacks,
                                                       const std::string &eventName,
                                                       openScenario::AcquirePositionAction acquirePositionAction) :
    ManipulatorCommonBase(world, eventNetwork, callbacks, eventName), acquirePositionAction(std::move(acquirePositionAction))
{
    cycleTime = 100;
}

void AcquirePositionManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(eventInterface);

        for (const auto actorId : triggeringEvent->actingAgents.entities)
        {
            auto trigger = std::make_unique<openpass::events::AcquirePositionEvent>(time, eventName, COMPONENTNAME, actorId, acquirePositionAction.position);
            eventNetwork->InsertTrigger(openpass::events::AcquirePositionEvent::TOPIC, std::move(trigger));
        }
    }
}

EventContainer AcquirePositionManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    const auto &conditionalEvents = eventNetwork->GetEvents(EventDefinitions::EventCategory::OpenSCENARIO);

    for (const auto &event : conditionalEvents)
    {
        const auto oscEvent = std::static_pointer_cast<openpass::events::OpenScenarioEvent>(event);

        if (oscEvent && oscEvent->GetName() == eventName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
