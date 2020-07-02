/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <map>
#include <memory>

#include "Interfaces/callbackInterface.h"
#include "Interfaces/eventDetectorNetworkInterface.h"
#include "Interfaces/manipulatorInterface.h"
#include "Interfaces/worldInterface.h"
#include "Common/openScenarioDefinitions.h"

/// \brief Use this class as static auto registering factory for custom commands
///
/// Note that registration happens automatically without additional construction code,
/// when the register method is used in a static assignment. This means that registering
/// will be done if the translation unit holds an according statement (!)
///
/// Example:
/// SomeClass.h -> static inline bool registered = CustomCommandFactory::Register("myCommand");
/// SomeClass.cpp is part of the translation unit
///
/// Effect:
/// SomeOtherFile.cpp -> CustomCommandFactory::Create("myCommand", ...); // Args must be delivered
class CustomCommandFactory final
{
    using CreateSignature = ManipulatorInterface *(*)(WorldInterface *,
                                                      SimulationSlave::EventNetworkInterface *,
                                                      const CallbackInterface *,
                                                      const openScenario::CustomCommandAction,
                                                      const std::string &);

public:
    /// \brief  Create registered class from command
    /// \param  command   The command, used during Register
    /// \param  args      Arguments of the underlying class
    /// \return The created instance
    ///
    template <typename... Args>
    static ManipulatorInterface* Create(const std::string &command, Args... args)
    {
        if (auto it = repository().find(command); it != repository().end())
        {
            return it->second(std::forward<Args>(args)...);
        }
        else
        {
            return nullptr;
        }
    }

    /// \brief Register a type T for a given command
    /// \return True if command could been registered
    template <typename T>
    static bool Register(const std::string command)
    {
        if (auto it = repository().find(command); it == repository().end())
        {
            repository()[command] = [](
                                        WorldInterface *world,
                                        SimulationSlave::EventNetworkInterface *eventNetwork,
                                        const CallbackInterface *callback,
                                        const openScenario::CustomCommandAction customCommandAction,
                                        const std::string &eventName) -> ManipulatorInterface * {
                return new T(world, eventNetwork, callback, customCommandAction, eventName);
            };
            return true;
        }
        return false;
    }

private:
    /// \brief Gets the static repository of registered classes
    ///
    /// As the method register is also static, it's not sure, that repository would
    /// be initialized before (issue with static initialization order). The is solved
    /// by wrapping the static repository in a static method, as it is guaranteed,
    /// that static variables in static methods are initialized first.
    ///
    /// \return  The repository
    static std::map<std::string, CreateSignature> &repository()
    {
        static std::map<std::string, CreateSignature> repository;
        return repository;
    }
};
