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
/** \file  ManipulatorExport.cpp */
//-----------------------------------------------------------------------------

#include "ManipulatorExport.h"
#include "Interfaces/callbackInterface.h"
#include "Interfaces/eventNetworkInterface.h"
#include "Interfaces/scenarioActionInterface.h"
#include "Common/openScenarioDefinitions.h"
#include "CollisionManipulator.h"
#include "ComponentStateChangeManipulator.h"
#include "LaneChangeManipulator.h"
#include "RemoveAgentsManipulator.h"
#include "NoOperationManipulator.h"
#include "TrajectoryManipulator.h"


const std::string version = "0.0.1";
static const CallbackInterface* Callbacks = nullptr;

extern "C" MANIPULATOR_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return version;
}

extern "C" MANIPULATOR_SHARED_EXPORT ManipulatorInterface* OpenPASS_CreateInstance(
    WorldInterface* world,
    std::shared_ptr<ScenarioActionInterface> action,
    std::string manipulatorType,
    SimulationSlave::EventNetworkInterface* eventNetwork,
    const CallbackInterface* callbacks)
{
    Callbacks = callbacks;

    try
    {
        if (manipulatorType == "CollisionManipulator")
        {
            return static_cast<ManipulatorInterface*>(new (std::nothrow) CollisionManipulator(
                                                          world,
                                                          eventNetwork,
                                                          callbacks));
        }
        else
        {
            if (auto userDefinedCommandAction = std::dynamic_pointer_cast<openScenario::UserDefinedCommandAction>(action))
            {
                const auto command = userDefinedCommandAction->GetCommand();
                const auto firstSplitInCommand = command.find(' ');
                const auto commandType = command.substr(0, firstSplitInCommand);

                if (commandType == "SetComponentState")
                {
                    return static_cast<ManipulatorInterface*>(new (std::nothrow) ComponentStateChangeManipulator(
                                                                  world,
                                                                  userDefinedCommandAction,
                                                                  eventNetwork,
                                                                  callbacks
                                                                  ));
                }

                if (commandType == "NoOperation")
                {
                    return static_cast<ManipulatorInterface*>(new (std::nothrow) NoOperationManipulator(
                                                                  world,
                                                                  eventNetwork,
                                                                  callbacks
                                                                  ));
                }
            }
            else if (auto globalAction = std::dynamic_pointer_cast<openScenario::GlobalEntityAction>(action))
            {
                const auto actionType = globalAction->GetType();
                if (actionType == openScenario::GlobalEntityActionType::Delete)
                {
                    return static_cast<ManipulatorInterface*>(new (std::nothrow) RemoveAgentsManipulator(
                                                                  world,
                                                                  globalAction,
                                                                  eventNetwork,
                                                                  callbacks
                                                                  ));
                }
            }
            else if (auto laneChangeAction = std::dynamic_pointer_cast<openScenario::PrivateLateralLaneChangeAction>(action))
            {
                return static_cast<ManipulatorInterface*>(new (std::nothrow) LaneChangeManipulator(
                                                              world,
                                                              laneChangeAction,
                                                              eventNetwork,
                                                              callbacks
                                                              ));
            }
            else if (auto trajectoryAction = std::dynamic_pointer_cast<openScenario::PrivateFollowTrajectoryAction>(action))
            {
                return static_cast<ManipulatorInterface*>(new (std::nothrow) TrajectoryManipulator(
                                                              world,
                                                              trajectoryAction,
                                                              eventNetwork,
                                                              callbacks
                                                              ));
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

    throw std::runtime_error("Unable to instantiate unknown manipulator type '" + manipulatorType + "'");
}

extern "C" MANIPULATOR_SHARED_EXPORT void OpenPASS_DestroyInstance(ManipulatorInterface* implementation)
{
    delete implementation;
}
