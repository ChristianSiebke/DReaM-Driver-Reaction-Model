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
/** \file  dynamics_trajectoryFollower.cpp */
//-----------------------------------------------------------------------------

#include <QCoreApplication>

#include "dynamics_trajectoryFollower.h"
#include "absoluteWorldCoordinateTrajectoryFollower.h"
#include "roadCoordinateTrajectoryFollower.h"
#include "CoreFramework/OpenPassSlave/importer/trajectoryImporter.h"

const std::string Version = "0.1.0";
static const CallbackInterface* Callbacks = nullptr;

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT const std::string& OpenPASS_GetVersion()
{
    return Version;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT ModelInterface* OpenPASS_CreateInstance(
    std::string componentName,
    bool isInit,
    int priority,
    int offsetTime,
    int responseTime,
    int cycleTime,
    StochasticsInterface* stochastics,
    WorldInterface* world,
    const ParameterInterface* parameters,
    const std::map<int, ObservationInterface*>* observations,
    AgentInterface* agent,
    const CallbackInterface* callbacks,
    SimulationSlave::EventNetworkInterface * const eventNetwork)
{
    Callbacks = callbacks;

    Trajectory trajectory;

    try
    {
        std::string trajectoryPath = QCoreApplication::applicationDirPath().toStdString()
                                     + "/configs/"
                                     + parameters->GetParametersString().at("TrajectoryFile");

        Importer::TrajectoryImporter::Import(trajectoryPath, &trajectory);
    }
    catch (...)
    {
        const std::string msg = componentName + " could not init trajectory parameters";
        Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, msg);
    }

    try
    {
        switch (trajectory.GetTrajectoryType())
        {
            case TrajectoryType::RoadCoordinatesAbsolute:
            case TrajectoryType::RoadCoordinatesRelative:
                return (ModelInterface*)(new (std::nothrow) RoadCoordinateTrajectoryFollower(
                                             componentName,
                                             isInit,
                                             priority,
                                             offsetTime,
                                             responseTime,
                                             cycleTime,
                                             stochastics,
                                             world,
                                             parameters,
                                             observations,
                                             callbacks,
                                             agent,
                                             &trajectory,
                                             eventNetwork));

            case TrajectoryType::WorldCoordinatesAbsolute:
                return (ModelInterface*)(new (std::nothrow) AbsoluteWorldCoordinateTrajectoryFollower(
                                             componentName,
                                             isInit,
                                             priority,
                                             offsetTime,
                                             responseTime,
                                             cycleTime,
                                             stochastics,
                                             world,
                                             parameters,
                                             observations,
                                             callbacks,
                                             agent,
                                             &trajectory,
                                             eventNetwork));

            default:
                throw std::runtime_error("Trajectory type could not be determined.");

        }
    }
    catch (const std::runtime_error& ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return nullptr;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return nullptr;
    }
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT void OpenPASS_DestroyInstance(ModelInterface* implementation)
{
    delete implementation;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_UpdateInput(
    ModelInterface* implementation,
    int localLinkId,
    const std::shared_ptr<SignalInterface const>& data,
    int time)
{
    try
    {
        implementation->UpdateInput(localLinkId, data, time);
    }
    catch (const std::runtime_error& ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_UpdateOutput(
    ModelInterface* implementation,
    int localLinkId,
    std::shared_ptr<SignalInterface const>& data,
    int time)
{
    try
    {
        implementation->UpdateOutput(localLinkId, data, time);
    }
    catch (const std::runtime_error& ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}

extern "C" DYNAMICS_TRAJECTORY_FOLLOWER_SHARED_EXPORT bool OpenPASS_Trigger(
    ModelInterface* implementation,
    int time)
{
    try
    {
        implementation->Trigger(time);
    }
    catch (const std::runtime_error& ex)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, ex.what());
        }

        return false;
    }
    catch (...)
    {
        if (Callbacks != nullptr)
        {
            Callbacks->Log(CbkLogLevel::Error, __FILE__, __LINE__, "unexpected exception");
        }

        return false;
    }

    return true;
}
