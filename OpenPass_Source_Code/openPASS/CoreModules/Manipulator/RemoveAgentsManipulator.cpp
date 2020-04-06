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

#include "RemoveAgentsManipulator.h"

RemoveAgentsManipulator::RemoveAgentsManipulator(WorldInterface *world,
                                                 std::shared_ptr<openScenario::GlobalEntityAction> action,
                                                 SimulationSlave::EventNetworkInterface *eventNetwork,
                                                 const CallbackInterface *callbacks):
    ManipulatorCommonBase(world,
                          action,
                          eventNetwork,
                          callbacks)
{
    cycleTime = 100;
}

void RemoveAgentsManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<ConditionalEvent>(eventInterface);

        for(const auto actorId : triggeringEvent->actingAgents)
        {
            world->GetAgent(actorId)->RemoveAgent();
        }

        auto removeAgentsEvent = std::make_shared<ConditionalEvent>(time,
                                                                    eventName,
                                                                    COMPONENTNAME,
                                                                    triggeringEvent->triggeringAgents,
                                                                    triggeringEvent->actingAgents);

        removeAgentsEvent->SetTriggeringEventId(triggeringEvent->GetId());
        eventNetwork->InsertEvent(removeAgentsEvent);
    }
}

EventContainer RemoveAgentsManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    for (const auto &event : eventNetwork->GetActiveEventCategory(EventDefinitions::EventCategory::OpenSCENARIO))
    {
        const auto conditionalEvent = std::static_pointer_cast<ConditionalEvent>(event);

        if(conditionalEvent && conditionalEvent.get()->GetName() == eventName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
