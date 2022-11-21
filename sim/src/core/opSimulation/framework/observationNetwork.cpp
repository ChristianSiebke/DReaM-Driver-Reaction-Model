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

#include <stdexcept>
#include "common/log.h"
#include "observationNetwork.h"
#include "bindings/observationBinding.h"
#include "observationModule.h"
#include "scheduler/runResult.h"

namespace core {

ObservationNetwork::~ObservationNetwork()
{
    Clear();
}

void ObservationNetwork::Clear()
{
    for (auto& items : modules)
    {
        delete items.second;
    }

    modules.clear();

    for (auto& [library, binding] : *bindings)
    {
        binding.Unload();
    }
}

bool ObservationNetwork::Instantiate(const ObservationInstanceCollection& observationInstances,
                                     StochasticsInterface* stochastics,
                                     WorldInterface* world,
                                     EventNetworkInterface* eventNetwork,
                                     const std::string& sceneryPath,
                                     DataBufferReadInterface* dataBuffer)
{
    for (auto& observationInstance : observationInstances)
    {
        try
        {
            const auto bindingIter = bindings->find(observationInstance.libraryName);
            if (bindingIter == bindings->end())
            {
                return false;
            }

            auto& binding = bindingIter->second;

            openpass::parameter::ParameterSetLevel1 parameters{observationInstance.parameters};

            auto module = binding.Instantiate(observationInstance.libraryName,
                                              parameters,
                                              stochastics,
                                              world,
                                              eventNetwork,
                                              dataBuffer);

            modules.insert({observationInstance.id, module});
        }
        catch (const std::exception& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << observationInstance.libraryName << ", could not be initialized: " << ex.what();
            return false;
        }
    }

    return true;
}

const std::map<int, ObservationModule*>& ObservationNetwork::GetObservationModules()
{
    return modules;
}

bool ObservationNetwork::InitAll()
{
    for (auto& item : modules)
    {
        auto module = item.second;
        try
        {
            if (!module->GetLibrary()->SimulationPreHook(module->GetImplementation()))
            {
                LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre hook failed";
                return false;
            }
        }
        catch (const std::runtime_error& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre hook failed: " << ex.what();
            return false;
        }
        catch (...)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre hook failed";
            return false;
        }
    }
    return true;
}

bool ObservationNetwork::InitRun()
{
    for (auto& item : modules)
    {
        auto module = item.second;
        try
        {
            if (!module->GetLibrary()->SimulationPreRunHook(module->GetImplementation()))
            {
                LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre run hook failed";
                return false;
            }
        }
        catch (std::runtime_error const& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre run hook failed: " << ex.what();
            return false;
        }
        catch (...)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation pre run hook failed";
            return false;
        }
    }
    return true;
}

bool ObservationNetwork::UpdateTimeStep(int time, RunResult& runResult)
{
    for (auto& item : modules)
    {
        ObservationModule* module = item.second;
        try
        {
            if (!module->GetLibrary()->SimulationUpdateHook(module->GetImplementation(), time, runResult))
            {
                LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation update hook failed";
                return false;
            }
        }
        catch (std::runtime_error const& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation update hook failed: " << ex.what();
            return false;
        }
        catch (...)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation update hook failed";
            return false;
        }
    }
    return true;
}


bool ObservationNetwork::FinalizeRun(const RunResult& result)
{
    for (auto& item : modules)
    {
        auto module = item.second;
        try
        {
            if (!module->GetLibrary()->SimulationPostRunHook(module->GetImplementation(), result))
            {
                LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post run hook failed";
                return false;
            }
        }
        catch (std::runtime_error const& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post run hook failed: " << ex.what();
            return false;
        }
        catch (...)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post run hook failed";
            return false;
        }
    }
    return true;
}

bool ObservationNetwork::FinalizeAll()
{
    for (auto& item : modules)
    {
        auto module = item.second;
        try
        {
            if (!module->GetLibrary()->SimulationPostHook(module->GetImplementation()))
            {
                LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post hook failed";
                return false;
            }
        }
        catch (std::runtime_error const& ex)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post hook failed: " << ex.what();
            return false;
        }
        catch (...)
        {
            LOG_INTERN(LogLevel::Error) << "observation " << module->GetId() << ", simulation post hook failed";
            return false;
        }
    }
    return true;
}

} // namespace core
