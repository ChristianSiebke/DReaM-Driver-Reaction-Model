/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020, 2021 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ManipulatorExport.cpp */
//-----------------------------------------------------------------------------

#include "ManipulatorExport.h"

#include "AcquirePositionManipulator.h"
#include "CollisionManipulator.h"
#include "CustomCommandFactory.h"
#include "DefaultCustomCommandAction.h"
#include "LaneChangeManipulator.h"
#include "RemoveAgentsManipulator.h"
#include "SpeedActionManipulator.h"
#include "TrajectoryManipulator.h"
#include "common/openScenarioDefinitions.h"
#include "include/callbackInterface.h"
#include "include/eventNetworkInterface.h"

class PublisherInterface;
namespace openpass::publisher {
class CoreDataPublisher;
}

const std::string version = "0.0.1";
static const CallbackInterface* Callbacks = nullptr;

extern "C" MANIPULATOR_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return version;
}

extern "C" MANIPULATOR_SHARED_EXPORT ManipulatorInterface* OpenPASS_CreateInstance(
    WorldInterface* world,
    openScenario::ManipulatorInformation manipulatorInformation,
    core::EventNetworkInterface* eventNetwork,
    const CallbackInterface* callbacks)
{
    Callbacks = callbacks;

    try
    {
        const auto &action = manipulatorInformation.action;

        if (std::holds_alternative<openScenario::UserDefinedAction>(action))
        {
            const auto userDefinedAction = std::get<openScenario::UserDefinedAction>(action);

            if (std::holds_alternative<openScenario::CustomCommandAction>(userDefinedAction))
            {
                const auto action = std::get<openScenario::CustomCommandAction>(userDefinedAction);
                const auto command = action.command;
                
                // the keyword is the first word in the command
                const auto keyword = command.substr(0, command.find(' '));
                const auto eventName = manipulatorInformation.eventName;
                
                // get the manipulator for the given keyword from the factory
                if (auto instance = CustomCommandFactory::Create(keyword,
                                                                 world,
                                                                 eventNetwork,
                                                                 callbacks,
                                                                 action,
                                                                 eventName))
                {
                    return instance;
                }
                // fallback: relay command without interpretation
                else
                {
                    return new DefaultCustomCommandAction(world,
                                                          eventNetwork,
                                                          callbacks,
                                                          action,
                                                          eventName);
                }
            }
            else
            {
                Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid UserDefinedAction as manipulator.");
                return nullptr;
            }
        }
        else if (std::holds_alternative<openScenario::GlobalAction>(action))
        {
            const auto &globalAction = std::get<openScenario::GlobalAction>(action);

            if (std::holds_alternative<openScenario::EntityAction>(globalAction))
            {
               const auto entityAction = std::get<openScenario::EntityAction>(globalAction);
               const auto actionType = entityAction.type;
               if (actionType == openScenario::EntityActionType::Delete)
               {
                   return static_cast<ManipulatorInterface*>(new (std::nothrow) RemoveAgentsManipulator(
                                                                 world,
                                                                 eventNetwork,
                                                                 callbacks,
                                                                 entityAction,
                                                                 manipulatorInformation.eventName));
               }
               else
               {
                   Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid EntityAction as manipulator.");
                   return nullptr;
               }
            }
            else
            {
                Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid GlobalAction as manipulator.");
                return nullptr;
            }
        }
        else if (std::holds_alternative<openScenario::PrivateAction>(action))
        {
            const auto &privateAction = std::get<openScenario::PrivateAction>(action);
            if (std::holds_alternative<openScenario::LateralAction>(privateAction))
            {
                const auto &lateralAction = std::get<openScenario::LateralAction>(privateAction);
                if (std::holds_alternative<openScenario::LaneChangeAction>(lateralAction))
                {
                    const auto &laneChangeAction = std::get<openScenario::LaneChangeAction>(lateralAction);
                    return static_cast<ManipulatorInterface*>(new (std::nothrow) LaneChangeManipulator(
                                                                  world,
                                                                  eventNetwork,
                                                                  callbacks,
                                                                  laneChangeAction,
                                                                  manipulatorInformation.eventName));
                }
                else
                {
                    Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid LateralAction as manipulator.");
                    return nullptr;
                }
            }
            else if (std::holds_alternative<openScenario::LongitudinalAction>(privateAction))
            {
                const auto &longitudinalAction = std::get<openScenario::LongitudinalAction>(privateAction);
                if (std::holds_alternative<openScenario::SpeedAction>(longitudinalAction))
                {
                        const auto &speedAction = std::get<openScenario::SpeedAction>(longitudinalAction);
                        return static_cast<ManipulatorInterface*>(new (std::nothrow) SpeedActionManipulator(
                                                                      world,
                                                                      eventNetwork,
                                                                      callbacks,
                                                                      speedAction,
                                                                      manipulatorInformation.eventName));
                }
            }
            else if (std::holds_alternative<openScenario::RoutingAction>(privateAction))
            {
                const auto &routingAction = std::get<openScenario::RoutingAction>(privateAction);
                if (std::holds_alternative<openScenario::FollowTrajectoryAction>(routingAction))
                {
                    const auto &followTrajectoryAction = std::get<openScenario::FollowTrajectoryAction>(routingAction);
                    return static_cast<ManipulatorInterface*>(new (std::nothrow) TrajectoryManipulator(
                                                                  world,
                                                                  eventNetwork,
                                                                  callbacks,
                                                                  followTrajectoryAction,
                                                                  manipulatorInformation.eventName));
                }
                else if (std::holds_alternative<openScenario::AcquirePositionAction>(routingAction))
                {
                    const auto &acquirePositionAction = std::get<openScenario::AcquirePositionAction>(routingAction);
                    return static_cast<ManipulatorInterface *>(new (std::nothrow) AcquirePositionManipulator(
                        world,
                        eventNetwork,
                        callbacks,
                        manipulatorInformation.eventName,
                        acquirePositionAction));
                }
                else
                {
                    Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid RoutingAction as manipulator.");
                    return nullptr;
                }
            }
            else
            {
                Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "Invalid PrivateAction as manipulator.");
                return nullptr;
            }
        }
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }

    throw std::runtime_error("Unable to instantiate unknown manipulator");
}

extern "C" MANIPULATOR_SHARED_EXPORT ManipulatorInterface* OpenPASS_CreateDefaultInstance(
    WorldInterface* world,
    std::string manipulatorType,
    core::EventNetworkInterface* eventNetwork,
    const CallbackInterface* callbacks,
    PublisherInterface *publisher)
{
    Callbacks = callbacks;

    try
    {
        if (manipulatorType == "CollisionManipulator")
        {
            auto coreDataPublisher = dynamic_cast<openpass::publisher::CoreDataPublisher*>(publisher);
            if(!coreDataPublisher)
            {
                throw std::runtime_error("Publisher must a CoreDataPublisher");
            }

            return static_cast<ManipulatorInterface*>(new (std::nothrow) CollisionManipulator(
                                                          world,
                                                          eventNetwork,
                                                          callbacks,
                                                          coreDataPublisher));
        }
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }

    throw std::runtime_error("Unable to instantiate unknown manipulator type '" + manipulatorType + "'");
}

extern "C" MANIPULATOR_SHARED_EXPORT void OpenPASS_DestroyInstance(ManipulatorInterface* implementation)
{
    delete implementation;
}
