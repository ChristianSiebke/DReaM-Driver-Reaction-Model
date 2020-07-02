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
/** \file  CustomLaneChangeManipulator.cpp */
//-----------------------------------------------------------------------------

#include "CustomLaneChangeManipulator.h"
#include "Common/customLaneChangeEvent.h"
#include "Common/commonTools.h"

#include <QtGlobal>

CustomLaneChangeManipulator::CustomLaneChangeManipulator(WorldInterface *world,
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

        if(commandTokensSize < 2)
        {
            const std::string msg = COMPONENTNAME + ": CustomLaneChange provided invalid number of commands";
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }

        if(commandTokens.at(0) != "SetCustomLaneChange")
        {
            const std::string msg = COMPONENTNAME + ": CustomLaneChange provided invalid command for initialization";
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }

        try
        {
            deltaLaneID = std::stoi(commandTokens.at(1));
        }
        catch (std::invalid_argument const &e)
        {
            const std::string msg = COMPONENTNAME + ": CustomLaneChange could not perform conversion";
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }
        catch (std::out_of_range const &e)
        {
            const std::string msg = COMPONENTNAME + ": CustomLaneChange provided deltaLaneID that is out of range of representable values by int";
            LOG(CbkLogLevel::Error, msg);
            throw std::runtime_error(msg);
        }

        cycleTime = 100;
}

void CustomLaneChangeManipulator::Trigger(int time)
{
    for (const auto &eventInterface : GetEvents())
    {
        auto triggeringEvent = std::dynamic_pointer_cast<ConditionalEvent>(eventInterface);

        for(const auto actorId : triggeringEvent->actingAgents)
        {
            auto customLaneChangeEvent = std::make_shared<CustomLaneChangeEvent>(time,
                                                                     eventName,
                                                                     COMPONENTNAME,
                                                                     actorId,
                                                                     deltaLaneID);

            eventNetwork->InsertEvent(customLaneChangeEvent);
        }
    }
}

EventContainer CustomLaneChangeManipulator::GetEvents()
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
