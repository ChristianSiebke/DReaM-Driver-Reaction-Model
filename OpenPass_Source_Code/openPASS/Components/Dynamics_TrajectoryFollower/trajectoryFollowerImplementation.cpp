/*******************************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  trajectoryFollowerImplementation.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>

#include "trajectoryFollowerImplementation.h"

#include "Interfaces/parameterInterface.h"
#include "Interfaces/observationInterface.h"
#include "Common/componentStateChangeEvent.h"
#include "Common/trajectorySignal.h"

TrajectoryFollowerImplementation::TrajectoryFollowerImplementation(std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface* stochastics,
        WorldInterface* world,
        const ParameterInterface* parameters,
        const std::map<int, ObservationInterface*>* observations,
        const CallbackInterface* callbacks,
        AgentInterface* agent,
        SimulationSlave::EventNetworkInterface * const eventNetwork) :
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
        observations,
        callbacks,
        agent,
        eventNetwork),
    cycleTimeInSeconds{static_cast<double>(cycleTime) / 1000.0}
{
    dynamicsOutputSignal.positionX = 0;
    dynamicsOutputSignal.positionY = 0;
    dynamicsOutputSignal.yaw = 0;

    ParseParameters(parameters);
}

void TrajectoryFollowerImplementation::ParseParameters(const ParameterInterface* parameters)
{
    try
    {
        auto boolParameters = parameters->GetParametersBool();
        enforceTrajectory = boolParameters.at("EnforceTrajectory");
        automaticDeactivation = boolParameters.at("AutomaticDeactivation");
    }
    catch (const std::out_of_range& error)
    {
        LOG(CbkLogLevel::Error, error.what());
        throw std::runtime_error(error.what());
    }
}

void TrajectoryFollowerImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data,
        int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        if (!enforceTrajectory)
        {
            const std::shared_ptr<ComponentStateSignalInterface const> stateSignal =
                std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
            if (stateSignal != nullptr && stateSignal->componentState == ComponentState::Acting)
            {
                const std::shared_ptr<LateralSignal const> signal = std::dynamic_pointer_cast<LateralSignal const>(data);
                if (!signal)
                {
                    ThrowInvalidSignalTypeError();
                }

                const bool isLateralSignalActing = signal->componentState == ComponentState::Acting;
                if (isLateralSignalActing)
                {
                    UpdateState(ComponentState::Disabled);
                }
            }
        }
    }
    else if (localLinkId == 1)
    {
        if (!enforceTrajectory)
        {
            const std::shared_ptr<ComponentStateSignalInterface const> stateSignal =
                    std::dynamic_pointer_cast<ComponentStateSignalInterface const>(data);
            if (stateSignal != nullptr && stateSignal->componentState == ComponentState::Acting)
            {
                const std::shared_ptr<AccelerationSignal const> signal = std::dynamic_pointer_cast<AccelerationSignal const>(data);
                if (!signal)
                {
                    ThrowInvalidSignalTypeError();
                }

                inputAccelerationActive = true;
                inputAcceleration = signal->acceleration;
            }
            else
            {
                inputAcceleration = 0;
            }
        }
    }
    else if (localLinkId == 2)
    {
        const auto trajectorySignal = std::dynamic_pointer_cast<TrajectorySignal const>(data);
        if (trajectorySignal && trajectorySignal->componentState == ComponentState::Acting)
        {
            trajectory = trajectorySignal->trajectory;
            UpdateState(ComponentState::Acting);
            previousTrajectoryIterator = trajectory.points.begin();
            nextTrajectoryIterator = previousTrajectoryIterator + 1;
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid signaltype";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void TrajectoryFollowerImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        try
        {
            dynamicsOutputSignal.componentState = componentState;
            data = std::make_shared<DynamicsSignal const>(dynamicsOutputSignal);
        }
        catch (const std::bad_alloc&)
        {
            ThrowCouldNotInstantiateSignalError();
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void TrajectoryFollowerImplementation::Trigger(int time)
{
    CalculateNextTimestep(time);
}

ComponentState TrajectoryFollowerImplementation::GetState() const
{
    return componentState;
}

void TrajectoryFollowerImplementation::UpdateState(const ComponentState newState)
{
    // only update state if the newstate differs from the current state
    if (newState != componentState)
    {
        if (newState == ComponentState::Acting)
        {
            if (canBeActivated)
            {
                componentState = newState;
                std::shared_ptr<VehicleComponentEvent> event = std::make_shared<VehicleComponentEvent>(currentTime,
                                                                                                       "TrajectoryFollowerActivated",
                                                                                                       GetComponentName(),
                                                                                                       GetAgent()->GetId());

                GetObservations()->at(0)->InsertEvent(event);
            }
        }
        else if (newState == ComponentState::Disabled)
        {
            canBeActivated = false;
            componentState = newState;
            std::shared_ptr<VehicleComponentEvent> event = std::make_shared<VehicleComponentEvent>(currentTime,
                                                                                                   "TrajectoryFollowerDeactivated",
                                                                                                   GetComponentName(),
                                                                                                   GetAgent()->GetId());

            GetObservations()->at(0)->InsertEvent(event);
        }
        else
        {
            throw std::runtime_error("TrajectoryFollower cannot handle new component state");
        }
    }
}

[[ noreturn ]] void TrajectoryFollowerImplementation::ThrowCouldNotInstantiateSignalError()
{
    const std::string msg = COMPONENTNAME + " could not instantiate signal";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

[[ noreturn ]] void TrajectoryFollowerImplementation::ThrowInvalidSignalTypeError()
{
    const std::string msg = COMPONENTNAME + " invalid signaltype";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

void TrajectoryFollowerImplementation::HandleEndOfTrajectory()
{
    dynamicsOutputSignal.velocity = 0;
    dynamicsOutputSignal.acceleration = 0;
    dynamicsOutputSignal.travelDistance = 0;
    dynamicsOutputSignal.yawRate = 0;

    if(automaticDeactivation)
    {
        UpdateState(ComponentState::Disabled);
    }
}

void TrajectoryFollowerImplementation::TriggerWithActiveAccelerationInput()
{
    TrajectoryPoint previousPosition = lastWorldPosition;
    TrajectoryPoint nextPosition = (*nextTrajectoryIterator);

    const double velocity = lastVelocity + inputAcceleration * cycleTimeInSeconds;

    if (velocity <= 0.0)
    {
        HandleEndOfTrajectory();
        return;
    }

    double remainingDistance = velocity * cycleTimeInSeconds;
    dynamicsOutputSignal.travelDistance = remainingDistance;
    double percentageTraveledBetweenCoordinates = 0.0;

    while (remainingDistance > 0.0)
    {
        double distanceBetweenPoints = CalculateDistanceBetweenWorldCoordinates(previousPosition, nextPosition);
        if (distanceBetweenPoints < remainingDistance)
        {
            previousTrajectoryIterator++;
            nextTrajectoryIterator++;

            previousPosition = *previousTrajectoryIterator;

            if(nextTrajectoryIterator != trajectory.points.end())
            {
                nextPosition = *nextTrajectoryIterator;
            }
            else
            {
                break;
            }
        }
        else
        {
            percentageTraveledBetweenCoordinates = remainingDistance / distanceBetweenPoints;
        }
        remainingDistance -= distanceBetweenPoints;
    }

    Common::Vector2d direction = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double deltaYawAngle = CalculateScaledDeltaYawAngle(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    UpdateDynamics(previousPosition, direction, deltaYawAngle, velocity, inputAcceleration);
}

void TrajectoryFollowerImplementation::TriggerWithInactiveAccelerationInput()
{
    double previousTimestamp = lastCoordinateTimestamp;
    TrajectoryPoint previousCoordinate = lastWorldPosition;
    TrajectoryPoint nextCoordinate = *nextTrajectoryIterator;

    double remainingTime = cycleTimeInSeconds;
    double timeBetweenCoordinates = nextCoordinate.time - previousTimestamp;
    double deltaS {0};

    while (timeBetweenCoordinates <= remainingTime &&
           nextTrajectoryIterator != trajectory.points.end())
    {
        deltaS += CalculateDistanceBetweenWorldCoordinates(previousCoordinate, nextCoordinate);

        previousTrajectoryIterator++;
        previousTimestamp = previousTrajectoryIterator->time;
        previousCoordinate = *previousTrajectoryIterator;
        lastCoordinateTimestamp = previousTimestamp;

        nextTrajectoryIterator++;
        if (nextTrajectoryIterator != trajectory.points.end())
        {
            nextCoordinate = *nextTrajectoryIterator;
            remainingTime -= timeBetweenCoordinates;
            timeBetweenCoordinates = nextCoordinate.time - previousTimestamp;
        }
        else
        {
            remainingTime = 1.0;
            timeBetweenCoordinates = 1.0;
        }
    }

    const auto& previousPosition = previousCoordinate;
    const auto& nextPosition = nextCoordinate;

    percentageTraveledBetweenCoordinates = remainingTime / timeBetweenCoordinates;
    Common::Vector2d direction = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double deltaYawAngle = CalculateScaledDeltaYawAngle(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    deltaS += direction.Length();
    dynamicsOutputSignal.travelDistance = deltaS;

    const double velocity = deltaS / cycleTimeInSeconds;
    const double acceleration = (velocity - lastVelocity) / cycleTimeInSeconds;

    lastCoordinateTimestamp = currentTime * 0.001;
    UpdateDynamics(previousPosition, direction, deltaYawAngle, velocity, acceleration);
}

void TrajectoryFollowerImplementation::CalculateNextTimestep(int time)
{
    currentTime = time;

    if (GetState() == ComponentState::Disabled)
    {
        return;
    }

    lastWorldPosition = {lastCoordinateTimestamp * 0.001, dynamicsOutputSignal.positionX, dynamicsOutputSignal.positionY, dynamicsOutputSignal.yaw};
    lastVelocity = dynamicsOutputSignal.velocity;

    if (previousTrajectoryIterator != trajectory.points.end() &&
            nextTrajectoryIterator != trajectory.points.end())
    {
        if (initialization)
        {
            UpdateDynamics(*previousTrajectoryIterator, {0,0}, 0, 0, 0);
            lastCoordinateTimestamp = previousTrajectoryIterator->time;
            initialization = false;
            return;
        }

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

Common::Vector2d TrajectoryFollowerImplementation::CalculateScaledVector(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor)
{
    Common::Vector2d result (nextPosition.x - previousPosition.x, nextPosition.y - previousPosition.y);
    result.Scale(factor);

    return result;
}

double TrajectoryFollowerImplementation::CalculateScaledDeltaYawAngle(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor)
{
    return (nextPosition.yaw - previousPosition.yaw) * factor;;
}


TrajectoryPoint TrajectoryFollowerImplementation::CalculateStartPosition(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition)
{
    const auto startDirection = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double startDeltaYawAngle = CalculateScaledDeltaYawAngle(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    TrajectoryPoint startPosition = previousPosition;
    startPosition.x += startDirection.x;
    startPosition.y += startDirection.y;
    startPosition.yaw += startDeltaYawAngle;

    return startPosition;
}

std::pair<int, TrajectoryPoint> TrajectoryFollowerImplementation::CalculateStartCoordinate(const std::pair<int, TrajectoryPoint> &previousPosition, const std::pair<int, TrajectoryPoint> &nextPosition)
{
    const auto startPosition = CalculateStartPosition(previousPosition.second, nextPosition.second);

    const double timeDifference = nextPosition.first - previousPosition.first;
    const double startTime = previousPosition.first + timeDifference * percentageTraveledBetweenCoordinates;

    return {startTime, startPosition};
}

double TrajectoryFollowerImplementation::CalculateDistanceBetweenWorldCoordinates(TrajectoryPoint previousPosition, TrajectoryPoint nextPosition)
{
    return std::hypot(nextPosition.x - previousPosition.x, nextPosition.y - previousPosition.y);
}

void TrajectoryFollowerImplementation::UpdateDynamics(const TrajectoryPoint &previousPosition,
                                                      const Common::Vector2d &direction,
                                                      double deltaYawAngle,
                                                      double velocity,
                                                      double acceleration)
{
    dynamicsOutputSignal.positionX = previousPosition.x + direction.x;
    dynamicsOutputSignal.positionY = previousPosition.y + direction.y;
    dynamicsOutputSignal.yaw = previousPosition.yaw + deltaYawAngle;

    dynamicsOutputSignal.yawRate = (dynamicsOutputSignal.yaw - lastWorldPosition.yaw) / cycleTimeInSeconds;

    dynamicsOutputSignal.velocity = velocity;
    dynamicsOutputSignal.acceleration = acceleration;
    dynamicsOutputSignal.centripetalAcceleration = dynamicsOutputSignal.yawRate * dynamicsOutputSignal.velocity;
}
