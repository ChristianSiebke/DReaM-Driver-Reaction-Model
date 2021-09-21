/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
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

#include "RemoveAgentsManipulator.h"

#include "common/events/basicEvent.h"

RemoveAgentsManipulator::RemoveAgentsManipulator(WorldInterface *world,
                                                 core::EventNetworkInterface *eventNetwork,
                                                 const CallbackInterface *callbacks,
                                                 [[maybe_unused]] const openScenario::EntityAction action,
                                                 const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks,
                          eventName)
{
    cycleTime = 100;
}

void RemoveAgentsManipulator::Trigger([[maybe_unused]] int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(eventInterface);

        for (const auto actorId : triggeringEvent->actingAgents.entities)
        {
            world->QueueAgentRemove(world->GetAgent(actorId));
        }
    }
}

EventContainer RemoveAgentsManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    for (const auto &event : eventNetwork->GetEvents(EventDefinitions::EventCategory::OpenSCENARIO))
    {
        const auto oscEvent = std::static_pointer_cast<openpass::events::OpenScenarioEvent>(event);

        if (oscEvent && oscEvent.get()->GetName() == eventName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
