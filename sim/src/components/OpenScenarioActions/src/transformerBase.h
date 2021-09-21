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

#include <tuple>

#include "include/agentInterface.h"
#include "include/eventNetworkInterface.h"
#include "include/worldInterface.h"
#include "actionTransformRepository.h"

namespace openScenario::transformation {

/// \brief CRTP Base class for Transformer
///
/// This class encapsulates the extraction of a specific TriggeringEvent from the eventNetwork,
/// so that the derived classes only need to take care of the tranformation to the Signal itself
template <typename T, typename Signal, typename TriggeringEvent>
class TransformerBase
{
public:
    /// \brief Entry point for the transformation
    /// \param eventNetwork Necessary for extracting the TriggeringEvent
    /// \param world     Releayed to the actual transformer
    /// \param agent     Releayed to the actual transformer
    /// \param cycleTime Releayed to the actual transformer
    /// \return Transform result for the given triggering event
    static TransformResult Transform(const core::EventNetworkInterface *eventNetwork,
                                     WorldInterface *world,
                                     AgentInterface *agent,
                                     int cycleTime)
    {
        const auto agentId = agent->GetId();
        const auto triggers = eventNetwork->GetTrigger(TriggeringEvent::TOPIC);

        for (const auto &trigger : triggers)
        {
            if (TriggeringEvent const *genericTrigger = dynamic_cast<TriggeringEvent const *>(trigger);
                genericTrigger && AgentIsAffected(agentId, genericTrigger))
            {
                return {TriggeringEvent::TOPIC, true, T::ConvertToSignal(*genericTrigger, world, agent, cycleTime)};
            }
        }
        return {TriggeringEvent::TOPIC, false, std::make_shared<Signal const>()};
    }

private:
    /// \brief Checks if the agentId is affected of the triggering event
    /// \param agentId  the current agent id
    /// \param event    the triggering event
    /// \return true if the agentId is contained in the acting agents
    static bool AgentIsAffected(const int agentId, EventInterface const *event)
    {
        const auto &actingEntities = event->GetActingAgents().entities;
        return std::find(actingEntities.cbegin(), actingEntities.cend(), agentId) != actingEntities.end();
    }
};

} // namespace openScenario::transformation
