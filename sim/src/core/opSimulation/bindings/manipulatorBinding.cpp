/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "bindings/manipulatorBinding.h"
#include "bindings/manipulatorLibrary.h"
#include "common/log.h"

namespace core
{

ManipulatorBinding::ManipulatorBinding(CallbackInterface *callbacks) :
    callbacks{callbacks}
{}

ManipulatorBinding::~ManipulatorBinding()
{
    Unload();
}

std::vector<const Manipulator*> ManipulatorBinding::Instantiate(const std::string libraryPath,
                                                          ScenarioInterface *scenario,
                                                          EventNetworkInterface* eventNetwork,
                                                          WorldInterface *world,
                                                          PublisherInterface *publisher)
{
    library = new ManipulatorLibrary(libraryPath,
                                     callbacks);

    if(!library)
    {
        LOG_INTERN(LogLevel::Error) << "Manipulator library could not be created";
        throw std::runtime_error("Manipulator library could not be created");
    }

    if(!library->Init())
    {
        delete library;
        LOG_INTERN(LogLevel::Error) << "Manipulator library could not be initalized";
        throw std::runtime_error("Manipulator library could not be initalized");
    }

    std::vector<const Manipulator*> manipulators;

    //Instantiates all default eventDetectors
    for(auto manipulatorType : defaultManipulatorTypes)
    {
        const auto manipulator = library->CreateManipulator(manipulatorType,
                                                            eventNetwork,
                                                            world,
                                                            publisher);

        manipulators.push_back(manipulator);
    }

    //Instantiates all manipulators
    for(const auto& action : scenario->GetActions())
    {
        const auto manipulator = library->CreateManipulator(action,
                                                            eventNetwork,
                                                            world);
        manipulators.push_back(manipulator);
    }

    return manipulators;
}

void ManipulatorBinding::Unload()
{
    if (library != nullptr)
    {
        delete library;
    }
    library = nullptr;
}

} // namespace core
