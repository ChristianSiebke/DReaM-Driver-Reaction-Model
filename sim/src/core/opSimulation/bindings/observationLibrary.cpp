/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <iostream>
#include <algorithm>
#include <QLibrary>
#include <sstream>

#include "common/log.h"
#include "include/observationInterface.h"
#include "parameterbuilder.h"
#include "modelElements/parameters.h"
#include "bindings/observationBinding.h"
#include "bindings/observationLibrary.h"
#include "observationModule.h"

namespace core {

bool ObservationLibrary::Init()
{
    std::string suffix = DEBUG_POSTFIX;
    library = new (std::nothrow) QLibrary(QString::fromStdString(libraryPath+suffix));
    if (!library)
    {
        return false;
    }

    if (!library->load())
    {
        LOG_INTERN(LogLevel::Error) << library->errorString().toStdString();
        delete library;
        library = nullptr;
        return false;
    }

    getVersionFunc = (ObservationInterface_GetVersion)library->resolve(DllGetVersionId.c_str());
    if (!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    createInstanceFunc = (ObservationInterface_CreateInstanceType)library->resolve(DllCreateInstanceId.c_str());
    if (!createInstanceFunc)
    {
        return false;
    }

    destroyInstanceFunc = (ObservationInterface_DestroyInstanceType)library->resolve(DllDestroyInstanceId.c_str());
    if (!destroyInstanceFunc)
    {
        LOG_INTERN(LogLevel::Warning) << "observation module could not be released";
        return false;
    }

    opSimulationManagerPreHookFunc = (ObservationInterface_OpSimulationManagerPreHook)library->resolve(DllOpSimulationManagerPreHookId.c_str());
    if (!opSimulationManagerPreHookFunc)
    {
        return false;
    }

    opSimulationManagerPostHookFunc = (ObservationInterface_OpSimulationManagerPostHook)library->resolve(DllOpSimulationManagerPostHookId.c_str());
    if (!opSimulationManagerPostHookFunc)
    {
        return false;
    }

    simulationPreHookFunc = (ObservationInterface_OpSimulationPreHook)library->resolve(DllOpSimulationPreHookId.c_str());
    if (!simulationPreHookFunc)
    {
        return false;
    }

    simulationPreRunHookFunc = (ObservationInterface_OpSimulationPreRunHook)library->resolve(DllOpSimulationPreRunHookId.c_str());
    if (!simulationPreRunHookFunc)
    {
        return false;
    }

    simulationUpdateHookFunc = (ObservationInterface_OpSimulationUpdateHook)library->resolve(DllOpSimulationUpdateHookId.c_str());
    if (!simulationPreRunHookFunc)
    {
        return false;
    }

    simulationPostRunHookFunc = (ObservationInterface_OpSimulationPostRunHook)library->resolve(DllOpSimulationPostRunHookId.c_str());
    if (!simulationPostRunHookFunc)
    {
        return false;
    }

    simulationPostHookFunc = (ObservationInterface_OpSimulationPostHook)library->resolve(DllOpSimulationPostHookId.c_str());
    if (!simulationPostHookFunc)
    {
        return false;
    }

    simulationResultFileFunc = (ObservationInterface_OpSimulationResultFile)library->resolve(DllOpSimulationResultFileId.c_str());
    if (!simulationResultFileFunc)
    {
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded observation library " << library->fileName().toStdString()
                                        << ", version " << getVersionFunc();
    }
    catch (std::runtime_error const& ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL: " << ex.what();
        return false;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    return true;
}

ObservationLibrary::~ObservationLibrary()
{
    if (!(observationModules.empty()))
    {
        LOG_INTERN(LogLevel::Warning) << "unloading library which is still in use";
    }

    if (library)
    {
        if (library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading observation library";
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

bool ObservationLibrary::ReleaseObservationModule(ObservationModule* observationModule)
{
    if (!library)
    {
        return false;
    }

    auto findIter = std::find(observationModules.begin(), observationModules.end(),
            observationModule);
    if (observationModules.end() == findIter)
    {
        LOG_INTERN(LogLevel::Warning) << "observation module doesn't belong to library";
        return false;
    }

    try
    {
        destroyInstanceFunc(observationModule->GetImplementation());
    }
    catch (std::runtime_error const& ex)
    {
        LOG_INTERN(LogLevel::Error) << "observation could not be released: " << ex.what();
        return false;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "observation could not be released";
        return false;
    }

    observationModules.erase(findIter);

    return true;
}

ObservationModule* ObservationLibrary::CreateObservationModule(
        const openpass::common::RuntimeInformation& runtimeInformation,
        const openpass::parameter::ParameterSetLevel1& parameters,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        EventNetworkInterface* eventNetwork,
        DataBufferReadInterface* const dataBuffer)
{
    if (!library)
    {
        return nullptr;
    }

    if (!library->isLoaded())
    {
        if (!library->load())
        {
            return nullptr;
        }
    }

    auto module_parameters = openpass::parameter::make<SimulationCommon::Parameters>(runtimeInformation, parameters);

    ObservationInterface* observationInterface = nullptr;
    try
    {
        observationInterface = createInstanceFunc(stochastics,
                               world,
                               eventNetwork,
                               module_parameters.get(),
                               callbacks,
                               dataBuffer);
    }
    catch (std::runtime_error const& ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not create observation instance: " << ex.what();
        return nullptr;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "could not create observation instance";
        return nullptr;
    }

    if (!observationInterface)
    {
        return nullptr;
    }

    ObservationModule* observationModule = new (std::nothrow) ObservationModule(
                observationInterface,
                std::move(module_parameters),
                this);
    if (!observationModule)
    {
        return nullptr;
    }

    observationModules.push_back(observationModule);
    return observationModule;
}

} // namespace core
