/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <map>
#include <memory>
#include <tuple>

#include "include/agentInterface.h"
#include "include/eventNetworkInterface.h"
#include "include/signalInterface.h"
#include "include/worldInterface.h"

using Identifier = const std::string;
using LinkId = int;
using AgentAffected = bool;
using Signal_ptr = std::shared_ptr<SignalInterface const>;
using TransformResult = std::tuple<Identifier, bool, Signal_ptr>;
using TransformResults = std::vector<TransformResult>;

/// \brief Use this class as mixed set for fixed and auto registering transformations
///
/// When used as fixed resource simply call:
/// ActionTransformRepository::Register(customTransformer);
///
/// This class can be used in conjunction with automatic compile time registration.
/// Note that registration happens automatically without additional construction code,
/// when the register method is used in a static assignment. This means that registering
/// will be done if the translation unit holds an accodring statement (!)
///
/// Example:
/// SomeClass.h -> static inline bool registered = ActionTransformRepository::Register(Transform);
/// SomeClass.cpp is part of the translation unit
///
/// Effect:
/// SomeOtherFile.cpp -> ActionTransformRepository::Register(customTransformer);
class ActionTransformRepository final
{
    using TransformSignature = TransformResult (*)(const core::EventNetworkInterface *eventNetwork,
                                                   WorldInterface *world,
                                                   AgentInterface *agent,
                                                   int cycleTime);

public:
    /// \brief Applies an event to signal transformation on each event for the internally defined identifier
    /// \param args Relayed to the registered transformer
    /// \return TransformResult holding a list of
    ///         identifiers (for link id assignment),
    ///         update state (does the current agent need an update),
    ///         and signal (must not be nullptr)
    template <typename... Args>
    static TransformResults Transform(Args... args)
    {
        TransformResults transformedActions;
        for (const auto &transform : repository())
        {
            auto [identifier, agent_is_affected, signal] = transform(std::forward<Args>(args)...);
            transformedActions.emplace_back(identifier, agent_is_affected, signal);
        }
        return transformedActions;
    }

    /// \brief Register a new transformation
    /// \param transform
    /// \return true
    static bool Register(TransformSignature transform)
    {
        repository().emplace_back(transform);
        return true;
    }

private:
    static std::vector<TransformSignature> &repository()
    {
        static std::vector<TransformSignature> repository;
        return repository;
    }
};
