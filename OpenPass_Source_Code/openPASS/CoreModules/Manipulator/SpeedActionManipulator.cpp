/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SpeedActionManipulator.cpp */
//-----------------------------------------------------------------------------

#include "SpeedActionManipulator.h"
#include "Common/speedActionEvent.h"

SpeedActionManipulator::SpeedActionManipulator(WorldInterface *world,
                                               SimulationSlave::EventNetworkInterface *eventNetwork,
                                               const CallbackInterface *callbacks,
                                               const openScenario::SpeedAction action,
                                               const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks,
                          eventName),
  action(action)
{
    cycleTime = 100;
}

void SpeedActionManipulator::Trigger([[maybe_unused]] int time)
{
    for (const auto& eventInterface : GetEvents())
    {
        const auto& triggeringEvent = std::dynamic_pointer_cast<ConditionalEvent>(eventInterface);

        for(const auto actorId : triggeringEvent->actingAgents)
        {
            auto event = std::make_shared<SpeedActionEvent>(time,
                                                            eventName,
                                                            COMPONENTNAME,
                                                            actorId,
                                                            action);
            eventNetwork->InsertEvent(event);
        }
    }
}

EventContainer SpeedActionManipulator::GetEvents()
{
    EventContainer manipulatorSpecificEvents{};

    const auto &conditionalEvents = eventNetwork->GetActiveEventCategory(EventDefinitions::EventCategory::OpenSCENARIO);

    for(const auto &event: conditionalEvents)
    {
        const auto conditionalEvent = std::static_pointer_cast<ConditionalEvent>(event);

        if(conditionalEvent && conditionalEvent.get()->GetName() == eventName)
        {
            manipulatorSpecificEvents.emplace_back(event);
        }
    }

    return manipulatorSpecificEvents;
}
