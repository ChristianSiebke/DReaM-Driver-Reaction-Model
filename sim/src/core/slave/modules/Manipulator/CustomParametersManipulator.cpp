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
/** \file  CustomParametersManipulator.cpp */
//-----------------------------------------------------------------------------

#include "CustomParametersManipulator.h"

#include "common/events/customParametersEvent.h"
#include "common/commonTools.h"

CustomParametersManipulator::CustomParametersManipulator(WorldInterface *world,
                                                         SimulationSlave::EventNetworkInterface *eventNetwork,
                                                         const CallbackInterface *callbacks,
                                                         const openScenario::CustomCommandAction action,
                                                         const std::string &eventName) :
    ManipulatorCommonBase(world,
                          eventNetwork,
                          callbacks,
                          eventName)
{
    auto commandTokens = CommonHelper::TokenizeString(action.command, ' ');
    auto commandTokensSize = commandTokens.size();

    if (commandTokensSize < 2)
    {
        const std::string msg = COMPONENTNAME + ": Too less arguments for SetCustomParameters";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    if (commandTokens.at(0) != "SetCustomParameters")
    {
        const std::string msg = COMPONENTNAME + ": Wrong command token";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    const auto first = commandTokens.begin() + 1;
    const auto last = commandTokens.end();
    customParameters = std::vector<std::string>(first, last);
}

void CustomParametersManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(eventInterface);

        for (const auto actorId : triggeringEvent->actingAgents.entities)
        {
            auto trigger = std::make_unique<openpass::events::CustomParametersEvent>(time, eventName, COMPONENTNAME, actorId, customParameters);
            eventNetwork->InsertTrigger(openpass::events::CustomParametersEvent::TOPIC, std::move(trigger));
        }
    }
}

EventContainer CustomParametersManipulator::GetEvents()
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
