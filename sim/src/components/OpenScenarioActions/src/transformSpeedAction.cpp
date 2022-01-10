/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "transformSpeedAction.h"

#include "common/openScenarioDefinitions.h"
#include "include/egoAgentInterface.h"
#include "oscActionsCalculation.h"

namespace openScenario::transformation {

std::shared_ptr<SpeedActionSignal> SpeedAction::ConvertToSignal(const openpass::events::SpeedActionEvent &event, WorldInterface *world, AgentInterface *agent, int cycleTime)
{
    const auto &speedAction = event.speedAction;

    double speedActionTargetSpeed;
    double speedActionAcceleration;

    // Determine target speed
    if (std::holds_alternative<openScenario::RelativeTargetSpeed>(speedAction.target))
    {
        const auto &relativeTargetSpeed = std::get<openScenario::RelativeTargetSpeed>(speedAction.target);
        auto referenceAgent = world->GetAgentByName(relativeTargetSpeed.entityRef);

        if (!referenceAgent)
        {
            throw std::runtime_error("Can't find entityref: " + relativeTargetSpeed.entityRef + " of SpeedAction.");
        }

        if (relativeTargetSpeed.speedTargetValueType == openScenario::SpeedTargetValueType::Delta)
        {
            speedActionTargetSpeed = referenceAgent->GetVelocity() + relativeTargetSpeed.value;
        }
        else if (relativeTargetSpeed.speedTargetValueType == openScenario::SpeedTargetValueType::Factor)
        {
            speedActionTargetSpeed = referenceAgent->GetVelocity() * relativeTargetSpeed.value;
        }
    }
    else if (std::holds_alternative<openScenario::AbsoluteTargetSpeed>(speedAction.target))
    {
        const auto &absoluteTargetSpeed = std::get<openScenario::AbsoluteTargetSpeed>(speedAction.target);
        speedActionTargetSpeed = absoluteTargetSpeed.value;
    }

    // Determine acceleration
    if (speedAction.transitionDynamics.shape == openScenario::Shape::Step)
    {
        double deltaVelocity = speedActionTargetSpeed - agent->GetVelocity();

        speedActionAcceleration = std::abs(deltaVelocity / (cycleTime / 1000.0));
    }
    else if (speedAction.transitionDynamics.shape == openScenario::Shape::Linear)
    {
        speedActionAcceleration = speedAction.transitionDynamics.value;
    }
    else
    {
        throw std::runtime_error("Invalid TransitionsDyanimcs shape in SpeedAction.");
    }

    return std::make_shared<SpeedActionSignal>(ComponentState::Acting, speedActionTargetSpeed, speedActionAcceleration);
}
} // namespace openScenario::transformation
