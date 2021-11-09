/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/// @file  DefaultCustomCommandAction.cpp

#include "DefaultCustomCommandAction.h"

#include "common/commonTools.h"
#include "common/events/defaultCustomCommandActionEvent.h"

DefaultCustomCommandAction::DefaultCustomCommandAction(WorldInterface *world,
                                           core::EventNetworkInterface *eventNetwork,
                                           const CallbackInterface *callbacks,
                                           const openScenario::CustomCommandAction action,
                                           const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks,
                          eventName),
    command{std::move(action.command)}
{
}

void DefaultCustomCommandAction::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(eventInterface);

        for (const auto actorId : triggeringEvent->actingAgents.entities)
        {
            auto trigger = std::make_unique<openpass::events::DefaultCustomCommandActionEvent>(time, eventName, COMPONENTNAME, actorId, command);
            eventNetwork->InsertTrigger(openpass::events::DefaultCustomCommandActionEvent::TOPIC, std::move(trigger));
        }
    }
}

EventContainer DefaultCustomCommandAction::GetEvents()
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
