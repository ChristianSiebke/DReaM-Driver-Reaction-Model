/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "common/log.h"
#include "manipulator.h"
#include "bindings/manipulatorBinding.h"
#include "manipulatorNetwork.h"

namespace core {

ManipulatorNetwork::ManipulatorNetwork(ManipulatorBinding* manipulatorBinding,
                                       WorldInterface* world,
                                       PublisherInterface* publisher):
    manipulatorBinding(manipulatorBinding),
    world(world),
    publisher(publisher)
{
}

ManipulatorNetwork::~ManipulatorNetwork()
{
    Clear();
}

bool ManipulatorNetwork::Instantiate(const std::string libraryPath,
                                     ScenarioInterface* scenario,
                                     EventNetworkInterface* eventNetwork)
{
    if (scenario != nullptr)
    {
        //Instantiate all manipulators
        try
        {
            manipulators = manipulatorBinding->Instantiate(libraryPath,
                           scenario,
                           eventNetwork,
                           world,
                           publisher);
        }
        catch (const std::runtime_error& error)
        {
            LOG_INTERN(LogLevel::Error) << "Could not instantiate all Manipulators: " << error.what();
            return false;
        }
    }
    return true;
}

void ManipulatorNetwork::Clear()
{
    for (auto manipulator : manipulators)
    {
        delete manipulator;
        manipulator = nullptr;
    }
}

std::vector<const Manipulator*> ManipulatorNetwork::GetManipulators()
{
    return manipulators;
}

} // namespace core
