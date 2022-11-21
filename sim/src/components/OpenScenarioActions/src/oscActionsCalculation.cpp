/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "oscActionsCalculation.h"

OscActionsCalculation::OscActionsCalculation(const WorldInterface *world) :
    world(world)
{
}

openScenario::Trajectory OscActionsCalculation::CalculateSinusiodalLaneChange(double deltaS, double deltaT, double deltaTime, double timeStep, GlobalRoadPosition startPosition, double startTime) const
{
    openScenario::Trajectory trajectory;
    double startT = startPosition.roadPosition.t + (startPosition.laneId > 0 ? 0.5 : -0.5) * world->GetLaneWidth(startPosition.roadId, startPosition.laneId, startPosition.roadPosition.s);
    if (startPosition.laneId > 0)
    {
        for (int laneId = 1; laneId < startPosition.laneId; ++laneId)
        {
            startT += world->GetLaneWidth(startPosition.roadId, laneId, startPosition.roadPosition.s);
        }
    }
    else
    {
        for (int laneId = -1; laneId > startPosition.laneId; --laneId)
        {
            startT -= world->GetLaneWidth(startPosition.roadId, laneId, startPosition.roadPosition.s);
        }
    }
    for (double timeSinceStart = 0; timeSinceStart <= deltaTime + 0.5 * timeStep; timeSinceStart += timeStep)
    {
        timeSinceStart = std::min(timeSinceStart, deltaTime); //For reducing rounding errors we also consider an additional point if difference of last time to deltaTime was at most half a timeStep
        double alpha = timeSinceStart / deltaTime;            // Position on the sinus curve (0 = start, 1 = end)
        double s = startPosition.roadPosition.s + alpha * deltaS;
        double t = startT + deltaT * 0.5 * (1 - std::cos(alpha * M_PI));
        double yaw = deltaT / deltaS * M_PI * 0.5 * std::sin(alpha * M_PI);
        auto worldPosition = world->RoadCoord2WorldCoord({s, t, yaw}, startPosition.roadId);
        openScenario::TrajectoryPoint nextPoint{startTime + timeSinceStart, worldPosition.xPos, worldPosition.yPos, worldPosition.yawAngle};
        trajectory.points.push_back(nextPoint);
    }
    return trajectory;
}
