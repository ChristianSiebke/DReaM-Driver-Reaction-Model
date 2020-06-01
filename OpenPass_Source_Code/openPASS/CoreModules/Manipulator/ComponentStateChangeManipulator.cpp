/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ComponentStateChangeManipulator.cpp */
//-----------------------------------------------------------------------------

#include "ComponentStateChangeManipulator.h"
#include "Common/componentStateChangeEvent.h"
#include "Common/commonTools.h"


ComponentStateChangeManipulator::ComponentStateChangeManipulator(WorldInterface *world,
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

    if(commandTokensSize < 3)
    {
        const std::string msg = COMPONENTNAME + ": ComponentStateChangeManipulator provided invalid number of commands";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    if(commandTokens.at(0) != "SetComponentState")
    {
        const std::string msg = COMPONENTNAME + ": ComponentStateChangeManipulator provided invalid command for initialization";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    componentName = commandTokens.at(1);
    componentStateName = commandTokens.at(2);

    cycleTime = 100;
}

void ComponentStateChangeManipulator::Trigger([[maybe_unused]] int time)
{
    for (const auto& eventInterface : GetEvents())
    {
        const auto& triggeringEvent = std::dynamic_pointer_cast<ConditionalEvent>(eventInterface);

        std::shared_ptr<ComponentChangeEvent> event =
                std::make_shared<ComponentChangeEvent>(time,
                                                       eventName,
                                                       COMPONENTNAME,
                                                       triggeringEvent->triggeringAgents,
                                                       triggeringEvent->actingAgents,
                                                       componentName,
                                                       componentStateName);

         event->SetTriggeringEventId(triggeringEvent->GetId());

         eventNetwork->InsertEvent(event);
    }
}

EventContainer ComponentStateChangeManipulator::GetEvents()
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
