/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentControllerImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "openScenarioActionsImpl.h"

#include "include/parameterInterface.h"

OpenScenarioActionsImplementation::OpenScenarioActionsImplementation(std::string componentName,
                                                                     bool isInit,
                                                                     int priority,
                                                                     int offsetTime,
                                                                     int responseTime,
                                                                     int cycleTime,
                                                                     StochasticsInterface *stochastics,
                                                                     WorldInterface *world,
                                                                     const ParameterInterface *parameters,
                                                                     PublisherInterface *const publisher,
                                                                     const CallbackInterface *callbacks,
                                                                     AgentInterface *agent,
                                                                     SimulationSlave::EventNetworkInterface *const eventNetwork) :
    UnrestrictedEventModelInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent,
        eventNetwork)
{
    if (parameters)
    {
        const auto customTransformerLinkAssignment = parameters->GetParametersInt();
        for (auto [key, value] : customTransformerLinkAssignment)
        {
            linkIdMapping.emplace(key, value);
        }
    }
}

void OpenScenarioActionsImplementation::UpdateInput(int, const std::shared_ptr<const SignalInterface> &, int)
{
}

void OpenScenarioActionsImplementation::UpdateOutput(LinkId localLinkId, std::shared_ptr<SignalInterface const> &data, [[maybe_unused]] int time)
{
    bool link_validated{false};
    bool update_sent{false};
    std::shared_ptr<SignalInterface const> unchanged_state{nullptr};

    // Transformers are allowed to write onto the same link ids
    // Yet, they are not allowed to write at the same time
    // Furher, on every update, data needs to be sent, even if there is no action available
    // This data needs to carry the correct subtype of the SignalInterface (e.g TrajectorySignal)
    // So every transformer delivers also an empty type, if nothing is to
    // Case 1: Transformer1 and Transformer2 has nothing to do = 2 entries, each with an empty object => relay only one
    // Case 2: Only one Transformer has something to do = still 2 entries => send only the active one
    // Case 3: Transformer1 and Transformer2 has something to do = 2 entries => report error
    for (auto [identifier, update_for_current_agent, signal] : pendingSignals)
    {
        if (linkIdMapping.find(identifier) == linkIdMapping.end())
        {
            if (update_for_current_agent)
            {
                ThrowUnregisteredIdentifier(identifier);
            }
            else
            {
                continue;
            }
        }

        if (localLinkId != linkIdMapping[identifier])
        {
            continue;
        }

        if (!link_validated)
        {
            unchanged_state = signal;
            link_validated = true;
        }

        if (update_for_current_agent)
        {
            if (update_sent)
            {
                ThrowOnTooManySignals(localLinkId);
            }

            update_sent = true;
            data = signal;
        }
    }

    if (link_validated && !update_sent)
    {
        data = unchanged_state;
    }

    if (!link_validated)
    {
        ThrowOnInvalidLinkId(localLinkId);
    }
}

void OpenScenarioActionsImplementation::Trigger([[maybe_unused]] int time)
{
    pendingSignals = ActionTransformRepository::Transform(GetEventNetwork(), GetWorld(), GetAgent(), GetCycleTime());
}

void OpenScenarioActionsImplementation::ThrowUnregisteredIdentifier(const std::string identifier)
{
    const std::string msg = std::string(COMPONENTNAME) + " Cannot find linkId assignement for identifier " + identifier;
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}

void OpenScenarioActionsImplementation::ThrowOnTooManySignals(int localLinkId)
{
    const std::string msg = std::string(COMPONENTNAME) + " More than one signal for localLinkId " + std::to_string(localLinkId);
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}

void OpenScenarioActionsImplementation::ThrowOnInvalidLinkId(int localLinkId)
{
    const std::string msg = std::string(COMPONENTNAME) + " No signal for localLinkId " + std::to_string(localLinkId);
    LOG(CbkLogLevel::Error, msg);
    throw std::runtime_error(msg);
}
