/********************************************************************************
 * Copyright (c) 2020 HLRS, University of Stuttgart
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
#include <QLibrary>
#include <sstream>
#include "bindings/eventDetectorLibrary.h"
#include "bindings/observationBinding.h"
#include "eventDetector.h"
#include "common/log.h"

namespace core
{

bool EventDetectorLibrary::Init()
{
    std::string suffix = DEBUG_POSTFIX;

    library = new (std::nothrow) QLibrary(QString::fromStdString(libraryPath+suffix));
    if(!library)
    {
        return false;
    }

    if(!library->load())
    {
        LOG_INTERN(LogLevel::Error) << library->errorString().toStdString();
        delete library;
        library = nullptr;
        return false;
    }

    getVersionFunc = (EventDetectorInterface_GetVersion)library->resolve(DllGetVersionId.c_str());
    if(!getVersionFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    createCollisionDetectorInstanceFunc = (EventDetectorInterface_CreateCollisionDetectorInstanceType)library->resolve(DllCreateCollisionDetectorInstanceId.c_str());
    if(!createCollisionDetectorInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not instantiate class from DLL";
        return false;
    }
    createConditionalDetectorInstanceFunc = (EventDetectorInterface_CreateConditionalDetectorInstanceType)library->resolve(DllCreateConditionalDetectorInstanceId.c_str());
    if(!createConditionalDetectorInstanceFunc)
    {
        LOG_INTERN(LogLevel::Error) << "could not instantiate class from DLL";
        return false;
    }

    destroyInstanceFunc = (EventDetectorInterface_DestroyInstanceType)library->resolve(DllDestroyInstanceId.c_str());
    if(!destroyInstanceFunc)
    {
        LOG_INTERN(LogLevel::Warning) << "event detector could not be released";
        return false;
    }

    try
    {
        LOG_INTERN(LogLevel::DebugCore) << "loaded event detector library " << library->fileName().toStdString()
                                        << ", version " << getVersionFunc();
    }
    catch(std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL: " << ex.what();
        return false;
    }
    catch(...)
    {
        LOG_INTERN(LogLevel::Error) << "could not retrieve version information from DLL";
        return false;
    }

    return true;
}

EventDetectorLibrary::~EventDetectorLibrary()
{
    if(!(eventDetectors.empty()))
    {
        LOG_INTERN(LogLevel::Warning) << "unloading library which is still in use";
    }

    if(library)
    {
        if(library->isLoaded())
        {
            LOG_INTERN(LogLevel::DebugCore) << "unloading event detector library ";
            library->unload();
        }

        delete library;
        library = nullptr;
    }
}

bool EventDetectorLibrary::ReleaseEventDetector(EventDetector *eventDetector)
{
    if(!library)
    {
        return false;
    }

    auto findIter = std::find(eventDetectors.begin(), eventDetectors.end(), eventDetector);
    if(eventDetectors.end() == findIter)
    {
        LOG_INTERN(LogLevel::Warning) << "event detector doesn't belong to library";
        return false;
    }

    try
    {
        destroyInstanceFunc(eventDetector->GetImplementation());
    }
    catch(std::runtime_error const &ex)
    {
        LOG_INTERN(LogLevel::Error) << "event detector could not be released: " << ex.what();
        return false;
    }
    catch(...)
    {
        LOG_INTERN(LogLevel::Error) << "event detector could not be released";
        return false;
    }

    eventDetectors.erase(findIter);

    return true;
}

EventDetector *EventDetectorLibrary::CreateCollisionDetector(EventNetworkInterface* eventNetwork,
                                                             WorldInterface *world,
                                                             StochasticsInterface *stochastics)
{
    if(!library)
    {
        return nullptr;
    }

    if(!library->isLoaded())
    {
        if(!library->load())
        {
            return nullptr;
        }
    }

    auto eventDetectorInterface = createCollisionDetectorInstanceFunc(world,
                                                                      eventNetwork,
                                                                      callbacks,
                                                                      stochastics);

    if(!eventDetectorInterface)
    {
        throw std::runtime_error("Could not create CollisionEventDetector");
    }

    EventDetector *eventDetector = new EventDetector(eventDetectorInterface,
                                                     this);

    eventDetectors.push_back(eventDetector);
    return eventDetector;
}

EventDetector *EventDetectorLibrary::CreateConditionalDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation,
                                                               EventNetworkInterface *eventNetwork,
                                                               WorldInterface *world,
                                                               StochasticsInterface *stochastics)
{
    if(!library)
    {
        return nullptr;
    }

    if(!library->isLoaded())
    {
        if(!library->load())
        {
            return nullptr;
        }
    }

    auto eventDetectorInterface = createConditionalDetectorInstanceFunc(world,
                                                                        eventDetectorInformation,
                                                                        eventNetwork,
                                                                        callbacks,
                                                                        stochastics);

    if(!eventDetectorInterface)
    {
        throw std::runtime_error("Could not create Conditional Event Detector");
    }

    EventDetector *eventDetector = new EventDetector(eventDetectorInterface,
                                                     this);

    eventDetectors.push_back(eventDetector);
    return eventDetector;
}

} // namespace core
