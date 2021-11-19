/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
 *               2016-2018 ITK Engineering GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <QLibrary>

#include "agent.h"
#include "agentDataPublisher.h"
#include "component.h"
#include "componentType.h"
#include "common/log.h"
#include "bindings/modelBinding.h"
#include "bindings/modelLibrary.h"
#include "include/observationNetworkInterface.h"
#include "observationModule.h"
#include "parameterbuilder.h"
#include "modelElements/parameters.h"

namespace core {

bool ModelLibrary::Init()
{
#if defined(unix)
    QString path = QString(modelLibraryPath.c_str()) + QString("/lib") + QString(modelLibraryName.c_str());
#elif defined (WIN32)
    std::string suffix = DEBUG_POSTFIX;
    QString path = QString(modelLibraryPath.c_str()) + QString("/") + QString((modelLibraryName+suffix).c_str());
#else
    #error Unsupported target platform. Either unix or WIN32 have to be set.
#endif

    library = new (std::nothrow) QLibrary(path);
    if (!library)
    {
        return false;
    }
    try {
        if (!library->load())
        {
            LOG_INTERN(LogLevel::Error) << library->errorString().toStdString();
            delete library;
            library = nullptr;
            return false;
        }
    }
    catch (std::exception e)
    {
        // ignore exceptions during dublicate initialization of libprotobuf
    }

    getVersionFunc = reinterpret_cast<ModelInterface_GetVersion>(library->resolve(DllGetVersionId.c_str()));
    if (!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllGetVersionId.c_str() << " from DLL";
        return false;
    }

    createInstanceFunc = reinterpret_cast<ModelInterface_CreateInstanceType>(library->resolve(DllCreateInstanceId.c_str()));
    if (!createInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllCreateInstanceId.c_str() << " from DLL";
        return false;
    }

    destroyInstanceFunc = reinterpret_cast<ModelInterface_DestroyInstanceType>(library->resolve(DllDestroyInstanceId.c_str()));
    if (!destroyInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllDestroyInstanceId.c_str() << " from DLL";
        return false;
    }

    updateInputFunc = reinterpret_cast<ModelInterface_UpdateInputType>(library->resolve(DllUpdateInputId.c_str()));
    if (!updateInputFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllUpdateInputId.c_str() << " from DLL";
        return false;
    }

    updateOutputFunc = reinterpret_cast<ModelInterface_UpdateOutputType>(library->resolve(DllUpdateOutputId.c_str()));
    if (!updateOutputFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllUpdateOutputId.c_str() << " from DLL";
        return false;
    }

    triggerFunc = reinterpret_cast<ModelInterface_TriggerType>(library->resolve(DllTriggerId.c_str()));
    if (!triggerFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not resolve " << DllTriggerId.c_str() << " from DLL";
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded model library " << library->fileName().toStdString()
                                        << ", version " << getVersionFunc();

        return true;
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
}

ModelLibrary::~ModelLibrary()
{
    if (!(components.empty()))
    {
        LOG_INTERN(LogLevel::Warning) << "unloading library which is still in use";
    }

    if (library)
    {
        if (library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading library " << modelLibraryName;
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

bool ModelLibrary::ReleaseComponent(ComponentInterface* component)
{
    if (!library)
    {
        return false;
    }

    auto findIter = std::find(components.begin(), components.end(), component);
    if (components.end() == findIter)
    {
        LOG_INTERN(LogLevel::Warning) << "model doesn't belong to library";
        return false;
    }

    try
    {
        destroyInstanceFunc(component->GetImplementation());
    }
    catch (std::runtime_error const& ex)
    {
        LOG_INTERN(LogLevel::Error) << "model could not be released: " << ex.what();
        return false;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "model could not be released";
        return false;
    }

    components.erase(findIter);

    // do not unload library here since shared_ptrs in other DLLs could still reference deleters within this DLL

    return true;
}

ComponentInterface* ModelLibrary::CreateComponent(std::shared_ptr<ComponentType> componentType,
        std::string componentName,
        const openpass::common::RuntimeInformation& runtimeInformation,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        ObservationNetworkInterface* observationNetwork,
        Agent* agent,
        EventNetworkInterface* eventNetwork,
        PublisherInterface* publisher)
{
    if (!library)
    {
        return nullptr;
    }

    if (!library->isLoaded() && !library->load())
    {
        return nullptr;
    }

    std::unique_ptr<ComponentInterface> component(new (std::nothrow) Component(componentName, agent));
    if (!component)
    {
        return nullptr;
    }

    if (!component->SetModelLibrary(this))
    {
        return nullptr;
    }

    // TODO: This is a workaround, as the OSI use case only imports a single observation library -> implement new observation concept
    component->SetObservations(observationNetwork->GetObservationModules());

    ModelInterface* implementation = nullptr;
    auto parameter = openpass::parameter::make<SimulationCommon::Parameters>(runtimeInformation, componentType->GetModelParameters());

    try
    {
        const auto &version = getVersionFunc();

        /*if (version == "0.3.0")
        {
            implementation = reinterpret_cast<UnrestrictedEventPublishModelInterface_CreateInstanceType>(createInstanceFunc)(
                componentName,
                componentType->GetInit(),
                componentType->GetPriority(),
                componentType->GetOffsetTime(),
                componentType->GetResponseTime(),
                componentType->GetCycleTime(),
                stochastics,
                world,
                parameter.get(),
                &component->GetObservations(),
                agent->GetAgentAdapter(),
                callbacks,
                eventNetwork,
                publisher);
        }
        if (version == "0.2.0")
        {
            implementation = reinterpret_cast<UnrestrictedPublishModelInterface_CreateInstanceType>(createInstanceFunc)(
                                 componentName,
                                 componentType->GetInit(),
                                 componentType->GetPriority(),
                                 componentType->GetOffsetTime(),
                                 componentType->GetResponseTime(),
                                 componentType->GetCycleTime(),
                                 stochastics,
                                 world,
                                 parameter.get(),
                                 &component->GetObservations(),
                                 agent->GetAgentAdapter(),
                                 callbacks,
                                 publisher);
        }
        else*/ if (version == "0.1.0")
        {
            implementation = reinterpret_cast<UnrestrictedEventModelInterface_CreateInstanceType>(createInstanceFunc)(
                                 componentName,
                                 componentType->GetInit(),
                                 componentType->GetPriority(),
                                 componentType->GetOffsetTime(),
                                 componentType->GetResponseTime(),
                                 componentType->GetCycleTime(),
                                 stochastics,
                                 world,
                                 parameter.get(),
                                 publisher,
                                 agent->GetAgentAdapter(),
                                 callbacks,
                                 eventNetwork);
        }
        else
        {
            implementation = createInstanceFunc(
                                 componentName,
                                 componentType->GetInit(),
                                 componentType->GetPriority(),
                                 componentType->GetOffsetTime(),
                                 componentType->GetResponseTime(),
                                 componentType->GetCycleTime(),
                                 stochastics,
                                 world,
                                 parameter.get(),
                                 publisher,
                                 agent->GetAgentAdapter(),
                                 callbacks);
        }
    }
    catch (std::runtime_error const& ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not create model instance: " << ex.what();
        return nullptr;
    }
    catch (...)
    {
        LOG_INTERN(LogLevel::Error) << "could not create model instance";
        return nullptr;
    }

    if (!implementation)
    {
        return nullptr;
    }

    component->SetImplementation(implementation);
    component->SetParameter(std::move(parameter));

    ComponentInterface* componentPtr = component.release();
    components.push_back(componentPtr);

    return componentPtr;
}

} // namespace core
