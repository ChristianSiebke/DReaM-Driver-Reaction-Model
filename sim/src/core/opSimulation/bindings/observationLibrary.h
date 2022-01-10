/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ObservationLibrary.h
//! @brief This file contains the internal representation of the library of an
//!        observation module.
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <string>
#include <QLibrary>

#include "include/eventNetworkInterface.h"
#include "include/observationInterface.h"
#include "include/callbackInterface.h"
#include "common/runtimeInformation.h"

namespace core {

class ObservationModule;

class ObservationLibrary
{
public:
    typedef const std::string& (*ObservationInterface_GetVersion)();
    typedef ObservationInterface* (*ObservationInterface_CreateInstanceType)(StochasticsInterface* stochastics,
            WorldInterface* world,
            EventNetworkInterface* eventNetwork,
            const ParameterInterface* parameters,
            const CallbackInterface* callbacks,
            DataBufferReadInterface* const dataBuffer);
    typedef void (*ObservationInterface_DestroyInstanceType)(ObservationInterface* implementation);
    typedef bool (*ObservationInterface_OpSimulationManagerPreHook)(ObservationInterface* implementation);
    typedef bool (*ObservationInterface_OpSimulationManagerPostHook)(ObservationInterface* implementation,
            const std::string& filename);
    typedef bool (*ObservationInterface_OpSimulationPreHook)(ObservationInterface* implementation);
    typedef bool (*ObservationInterface_OpSimulationPreRunHook)(ObservationInterface* implementation);
    typedef bool (*ObservationInterface_OpSimulationUpdateHook)(ObservationInterface* implementation,
            int time,
            RunResultInterface& runResult);
    typedef bool (*ObservationInterface_OpSimulationPostRunHook)(ObservationInterface* implementation,
            const RunResultInterface& runResult);
    typedef bool (*ObservationInterface_OpSimulationPostHook)(ObservationInterface* implementation);
    typedef const std::string(*ObservationInterface_OpSimulationResultFile)(ObservationInterface* implementation);

    ObservationLibrary(const std::string libraryPath,
                       CallbackInterface* callbacks) :
        libraryPath(libraryPath),
        callbacks(callbacks)
    {}
    ObservationLibrary(const ObservationLibrary&) = delete;
    ObservationLibrary(ObservationLibrary&&) = delete;
    ObservationLibrary& operator=(const ObservationLibrary&) = delete;
    ObservationLibrary& operator=(ObservationLibrary&&) = delete;
    virtual ~ObservationLibrary();

    //-----------------------------------------------------------------------------
    //! Inits the observation library by obtaining the function pointers to all
    //! "Interface.h" functions defined via typedef.
    //!
    //! @return     Flag if init was successful
    //-----------------------------------------------------------------------------
    bool Init();

    //-----------------------------------------------------------------------------
    //! Releases an observation module instance by calling the destroyInstance method
    //! on the instance and removing the instance from the list of observation modules.
    //!
    //! @return     Flag if release was successful
    //-----------------------------------------------------------------------------
    bool ReleaseObservationModule(ObservationModule* observationModule);

    //-----------------------------------------------------------------------------
    //! Creates an observation module based on the observation instance in the run
    //! config and the observation library stored in the mapping for the library
    //! name of the observation instance.
    //!
    //! @param[in]     observationInstance  Observation instance from the run config
    //! @param[in]     stochastics          The stochastics interface
    //! @param[in]     world                The world interface
    //! @return                             Observation module created from the
    //!                                     observation instance
    //-----------------------------------------------------------------------------
    ObservationModule* CreateObservationModule(
            const openpass::common::RuntimeInformation& runtimeInformation,
            const openpass::parameter::ParameterSetLevel1 &parameters,
            StochasticsInterface* stochastics,
            WorldInterface* world,
            EventNetworkInterface* eventNetwork,
            DataBufferReadInterface* const dataBuffer);

    bool SimulationPreHook(ObservationInterface* implementation)
    {
        return simulationPreHookFunc(implementation);
    }


    bool SimulationPreRunHook(ObservationInterface* implementation)
    {
        return simulationPreRunHookFunc(implementation);
    }

    bool SimulationUpdateHook(ObservationInterface* implementation,
                         int time,
                         RunResultInterface& runResult)
    {
        return simulationUpdateHookFunc(implementation, time, runResult);
    }

    bool SimulationPostRunHook(ObservationInterface* implementation,
                          const RunResultInterface& runResult)
    {
        return simulationPostRunHookFunc(implementation, runResult);
    }

    bool SimulationPostHook(ObservationInterface* implementation)
    {
        return simulationPostHookFunc(implementation);
    }

    const std::string SimulationResultFile(ObservationInterface* implementation)
    {
        return simulationResultFileFunc(implementation);
    }

private:
    const std::string DllGetVersionId = "OpenPASS_GetVersion";
    const std::string DllCreateInstanceId = "OpenPASS_CreateInstance";
    const std::string DllDestroyInstanceId = "OpenPASS_DestroyInstance";
    const std::string DllOpSimulationManagerPreHookId = "OpenPASS_OpSimulationPreHook";
    const std::string DllOpSimulationManagerPostHookId = "OpenPASS_OpSimulationPostHook";
    const std::string DllOpSimulationPreHookId = "OpenPASS_OpSimulationPreHook";
    const std::string DllOpSimulationPreRunHookId = "OpenPASS_OpSimulationPreRunHook";
    const std::string DllOpSimulationUpdateHookId = "OpenPASS_OpSimulationUpdateHook";
    const std::string DllOpSimulationPostRunHookId = "OpenPASS_OpSimulationPostRunHook";
    const std::string DllOpSimulationPostHookId = "OpenPASS_OpSimulationPostHook";
    const std::string DllOpSimulationResultFileId = "OpenPASS_OpSimulationResultFile";

    const std::string libraryPath;
    std::vector<ObservationModule*> observationModules;
    QLibrary* library = nullptr;
    CallbackInterface* callbacks;

    ObservationInterface_GetVersion getVersionFunc{nullptr};
    ObservationInterface_CreateInstanceType createInstanceFunc{nullptr};
    ObservationInterface_DestroyInstanceType destroyInstanceFunc{nullptr};
    ObservationInterface_OpSimulationManagerPreHook opSimulationManagerPreHookFunc{nullptr};
    ObservationInterface_OpSimulationManagerPostHook opSimulationManagerPostHookFunc{nullptr};
    ObservationInterface_OpSimulationPreHook simulationPreHookFunc{nullptr};
    ObservationInterface_OpSimulationPreRunHook simulationPreRunHookFunc{nullptr};
    ObservationInterface_OpSimulationUpdateHook simulationUpdateHookFunc{nullptr};
    ObservationInterface_OpSimulationPostRunHook simulationPostRunHookFunc{nullptr};
    ObservationInterface_OpSimulationPostHook simulationPostHookFunc{nullptr};
    ObservationInterface_OpSimulationResultFile simulationResultFileFunc{nullptr};
};

} // namespace core


