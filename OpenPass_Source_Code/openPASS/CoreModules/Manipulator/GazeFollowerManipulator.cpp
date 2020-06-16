/*******************************************************************************
* Copyright (c) 2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  GazeFollowerManipulator.cpp */
//-----------------------------------------------------------------------------

#include "GazeFollowerManipulator.h"
#include "Common/gazeFollowerEvent.h"
#include "Common/commonTools.h"

#include <QtGlobal>

GazeFollowerManipulator::GazeFollowerManipulator(WorldInterface *world,
                                                 std::shared_ptr<openScenario::UserDefinedCommandAction> action,
                                                 SimulationSlave::EventNetworkInterface *eventNetwork,
                                                 const CallbackInterface *callbacks):
    ManipulatorCommonBase(world,
                          action,
                          eventNetwork,
                          callbacks)
{
    auto commandTokens = CommonHelper::TokenizeString(action->GetCommand(), ' ');
    auto commandTokensSize = commandTokens.size();

    if(commandTokensSize < 2)
    {
        const std::string msg = COMPONENTNAME + ": GazeFollowerManipulator provided invalid number of commands";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    if(commandTokens.at(0) != "SetGazeFollower")
    {
        const std::string msg = COMPONENTNAME + ": GazeFollowerManipulator provided invalid command for initialization";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }

    gazeActivityState = commandTokens.at(1);
    if(commandTokensSize > 2)
    {
        gazeFileName = commandTokens.at(2);
    }

    cycleTime = 100;
}

void GazeFollowerManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<ConditionalEvent>(eventInterface);

        for(const auto actorId : triggeringEvent->actingAgents)
        {
            auto gazeFollowerEvent = std::make_shared<GazeFollowerEvent>(time,
                                                                         eventName,
                                                                         COMPONENTNAME,
                                                                         actorId,
                                                                         gazeActivityState,
                                                                         gazeFileName);

            eventNetwork->InsertEvent(gazeFollowerEvent);
        }
    }
}

EventContainer GazeFollowerManipulator::GetEvents()
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
