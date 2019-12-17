/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  roadCoordinateTrajectoryFollower.cpp */
//-----------------------------------------------------------------------------

#include "Common/vector2d.h"
#include "Interfaces/worldInterface.h"

#include "roadCoordinateTrajectoryFollower.h"

namespace {
static Common::Vector2d CalculateScaledVector(const RoadPosition previousPosition, const RoadPosition nextPosition, const double factor);
static double CalculateScaledDeltaHeading(const RoadPosition previousPosition, const RoadPosition nextPosition, const double factor);
}

RoadCoordinateTrajectoryFollower::RoadCoordinateTrajectoryFollower(std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        const std::map<int, ObservationInterface*> *observations,
        const CallbackInterface *callbacks,
        AgentInterface *agent,
        TrajectoryInterface *trajectory,
        SimulationSlave::EventNetworkInterface * const eventNetwork) :
    TrajectoryFollowerCommonBase(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        observations,
        callbacks,
        agent,
        eventNetwork),
    roadTrajectory { *(trajectory->GetRoadCoordinates()) },
    isRelativeTrajectory { trajectory->GetTrajectoryType() == TrajectoryType::RoadCoordinatesRelative }
{
    Init();
}

void RoadCoordinateTrajectoryFollower::Init()
{
    lastRoadPosition = GetAgent()->GetRoadPosition();
    lastCoordinateTimestamp = 0;
    lastVelocity = GetAgent()->GetVelocity();

    if (isRelativeTrajectory)
    {
        if (roadTrajectory.size() < 2)
        {
            throw std::runtime_error("Relative trajectory needs at least two coordinates");
        }

        // The first point of a relative trajectory is always skipped
        roadTrajectory.erase(roadTrajectory.begin());

        startPosition = GetAgent()->GetRoadPosition();

        // Adjust t-coordinate to middle of the road by subtracting lane widths
        startPosition.t -= 0.5 * GetAgent()->GetLaneWidth();
        startLaneId = GetAgent()->GetMainLaneId(MeasurementPoint::Reference);
        const int numberOfLanesToTheLeft = -startLaneId - 1;
        for (int i = 1; i <= numberOfLanesToTheLeft; i++)
        {
            startPosition.t -= GetAgent()->GetLaneWidth(i);
        }
    }

    nextTrajectoryIterator = roadTrajectory.begin();
}

void RoadCoordinateTrajectoryFollower::CalculateNextTimestep(int time)
{
    currentTime = time;

    if (GetState() == ComponentState::Disabled)
    {
        return;
    }

    if (nextTrajectoryIterator != roadTrajectory.end())
    {
        if (inputAccelerationActive)
        {
            TriggerWithActiveAccelerationInput();
        }
        else
        {
            TriggerWithInactiveAccelerationInput();
        }
    }

    else
    {
        HandleEndOfTrajectory();
    }
}

void RoadCoordinateTrajectoryFollower::TriggerWithActiveAccelerationInput()
{
    RoadPosition previousPosition = lastRoadPosition;
    if (isRelativeTrajectory)
    {
        previousPosition.s -= startPosition.s;
        previousPosition.t -= startPosition.t;
        previousPosition.hdg -= startPosition.hdg;
    }
    RoadPosition nextPosition = (*nextTrajectoryIterator).second;

    const double velocity = lastVelocity + inputAcceleration * cycleTimeInSeconds;

    if (velocity <= 0.0)
    {
        HandleEndOfTrajectory();
        return;
    }

    double distanceRemaining = velocity * cycleTimeInSeconds;
    dynamicsOutputSignal.travelDistance = distanceRemaining;
    double percentageTraveledBetweenCoordinates = 0.0;

    while (distanceRemaining > 0.0)
    {
        double distanceBetweenPoints = std::hypot(nextPosition.s - previousPosition.s,
                                                  nextPosition.t - previousPosition.t);
        if (distanceBetweenPoints < distanceRemaining)
        {
            previousTrajectoryIterator = nextTrajectoryIterator;
            nextTrajectoryIterator++;

            previousPosition = (*previousTrajectoryIterator).second;

            if (nextTrajectoryIterator != roadTrajectory.end())
            {
                nextPosition = (*nextTrajectoryIterator).second;
            }
        }
        else
        {
            percentageTraveledBetweenCoordinates = distanceRemaining / distanceBetweenPoints;

        }
        distanceRemaining -= distanceBetweenPoints;
    }
    Common::Vector2d direction = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double deltaHeading = CalculateScaledDeltaHeading(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    const DynamicsInformation dynamicsInformation {previousPosition,
                                                   direction,
                                                   deltaHeading,
                                                   velocity,
                                                   inputAcceleration};
    UpdateDynamics(dynamicsInformation);
}

void RoadCoordinateTrajectoryFollower::TriggerWithInactiveAccelerationInput()
{
    std::pair<int, RoadPosition> previousCoordinate = {lastCoordinateTimestamp, lastRoadPosition};
    if (isRelativeTrajectory)
    {
        previousCoordinate.second.s -= startPosition.s;
        previousCoordinate.second.t -= startPosition.t;
        previousCoordinate.second.hdg -= startPosition.hdg;
    }
    std::pair<int, RoadPosition> nextCoordinate = *nextTrajectoryIterator;

    double remainingTime = GetCycleTime();
    double timeBetweenCoordinates = nextCoordinate.first - previousCoordinate.first;
    double deltaS {0};

    while (timeBetweenCoordinates <= remainingTime &&
           (nextTrajectoryIterator) != roadTrajectory.end())
    {
        //This contains a slight error since road curvature is not considered
        deltaS += std::hypot(nextCoordinate.second.s - previousCoordinate.second.s,
                             nextCoordinate.second.t - previousCoordinate.second.t);

        previousTrajectoryIterator = nextTrajectoryIterator;
        previousCoordinate = *previousTrajectoryIterator;
        lastCoordinateTimestamp = previousCoordinate.first;

        nextTrajectoryIterator++;
        if(nextTrajectoryIterator != roadTrajectory.end())
        {
            nextCoordinate = *nextTrajectoryIterator;
            remainingTime -= timeBetweenCoordinates;
            timeBetweenCoordinates = nextCoordinate.first - previousCoordinate.first;
        }
        else
        {
            remainingTime = 1.0;
            timeBetweenCoordinates = 1.0;
        }
    }

    percentageTraveledBetweenCoordinates = remainingTime / timeBetweenCoordinates;
    const double deltaTimestamp = (nextCoordinate.first - lastCoordinateTimestamp) * percentageTraveledBetweenCoordinates;
    lastCoordinateTimestamp = lastCoordinateTimestamp + static_cast<int>(deltaTimestamp);

    const auto dynamicsInformation = PrepareDynamicsInformationWithoutExternalAccelaration(previousCoordinate.second,
                                                                nextCoordinate.second,
                                                                deltaS);
    UpdateDynamics(dynamicsInformation);
}

DynamicsInformation RoadCoordinateTrajectoryFollower::PrepareDynamicsInformationWithoutExternalAccelaration(
        const RoadPosition previousPosition,
        const RoadPosition nextPosition,
        const double additionalDistance)
{
    Common::Vector2d direction = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double deltaYawAngle = CalculateScaledDeltaHeading(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    double deltaS = additionalDistance + direction.Length();
    dynamicsOutputSignal.travelDistance = deltaS;

    const double velocity = deltaS / cycleTimeInSeconds;
    const double acceleration = (velocity - lastVelocity) / cycleTimeInSeconds;

    return { previousPosition, direction, deltaYawAngle, velocity, acceleration };
}

void RoadCoordinateTrajectoryFollower::UpdateDynamics(const DynamicsInformation dynamicsInformation)
{
    RoadPosition currentRoadPosition;

    const auto &previousPosition = dynamicsInformation.previousPosition;
    currentRoadPosition.s = previousPosition.s + dynamicsInformation.direction.x;
    currentRoadPosition.t = previousPosition.t + dynamicsInformation.direction.y;
    currentRoadPosition.hdg = previousPosition.hdg + dynamicsInformation.deltaHeading;

    if (isRelativeTrajectory)
    {
        currentRoadPosition.s += startPosition.s;
        currentRoadPosition.t += startPosition.t;
        currentRoadPosition.hdg += startPosition.hdg;
    }

    Position result = GetWorld()->RoadCoord2WorldCoord(currentRoadPosition);

    dynamicsOutputSignal.positionX = result.xPos;
    dynamicsOutputSignal.positionY = result.yPos;
    dynamicsOutputSignal.yaw = result.yawAngle;

    dynamicsOutputSignal.yawRate = (dynamicsOutputSignal.yaw - lastWorldPosition.yawAngle) / cycleTimeInSeconds;

    dynamicsOutputSignal.velocity = dynamicsInformation.velocity;
    dynamicsOutputSignal.acceleration = dynamicsInformation.acceleration;
    dynamicsOutputSignal.centripetalAcceleration = dynamicsOutputSignal.yawRate * dynamicsOutputSignal.velocity;

    lastRoadPosition = currentRoadPosition;
    lastWorldPosition = {dynamicsOutputSignal.positionX,
                         dynamicsOutputSignal.positionY,
                         dynamicsOutputSignal.yaw,
                         0.0};
    lastVelocity = dynamicsOutputSignal.velocity;
}

namespace {

static Common::Vector2d CalculateScaledVector(
        const RoadPosition previousPosition,
        const RoadPosition nextPosition,
        const double factor)
{
    Common::Vector2d result (nextPosition.s - previousPosition.s, nextPosition.t - previousPosition.t);
    result.Scale(factor);

    return result;
}

static double CalculateScaledDeltaHeading(
        const RoadPosition previousPosition,
        const RoadPosition nextPosition,
        const double factor)
{
    return (nextPosition.hdg - previousPosition.hdg) * factor;
}

}
