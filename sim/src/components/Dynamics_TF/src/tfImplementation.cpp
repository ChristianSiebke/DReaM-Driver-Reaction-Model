/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  trajectoryFollowerImplementation.cpp */
//-----------------------------------------------------------------------------

#include "tfImplementation.h"

#include <memory>

#include <qglobal.h>

#include "common/trajectorySignal.h"
#include "include/parameterInterface.h"
#include "include/publisherInterface.h"

TrajectoryFollowerImplementation::TrajectoryFollowerImplementation(std::string componentName,
                                                                   bool isInit,
                                                                   int priority,
                                                                   int offsetTime,
                                                                   int responseTime,
                                                                   int cycleTime,
                                                                   StochasticsInterface *stochastics,
                                                                   WorldInterface *world,
                                                                   const ParameterInterface *parameters,
                                                                   PublisherInterface * const publisher,
                                                                   const CallbackInterface *callbacks,
                                                                   AgentInterface *agent) :
    UnrestrictedModelInterface(
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
        agent),
    cycleTimeInSeconds{static_cast<double>(cycleTime) / 1000.0}
{
    ParseParameters(parameters);
    Init();
}

void TrajectoryFollowerImplementation::ParseParameters(const ParameterInterface *parameters)
{
    try
    {
        auto boolParameters = parameters->GetParametersBool();
        enforceTrajectory = boolParameters.at("EnforceTrajectory");
        automaticDeactivation = boolParameters.at("AutomaticDeactivation");
    }
    catch (const std::out_of_range &error)
    {
        LOG(CbkLogLevel::Error, error.what());
        throw std::runtime_error(error.what());
    }
}

void TrajectoryFollowerImplementation::Init()
{
    lastWorldPosition = {0, GetAgent()->GetPositionX(), GetAgent()->GetPositionY(), GetAgent()->GetYaw()};
    lastVelocity = GetAgent()->GetVelocity();

    nextTrajectoryIterator = trajectory.points.begin();
    currentTime = 0;
}

void TrajectoryFollowerImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data,
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
                const std::shared_ptr<SteeringSignal const> signal = std::dynamic_pointer_cast<SteeringSignal const>(data);
                if (!signal)
                {
                    ThrowInvalidSignalTypeError();
                }

                if (signal->componentState == ComponentState::Acting)
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
        const std::string msg = std::string(COMPONENTNAME) + " invalid signaltype";
        LOG(CbkLogLevel::Error, msg);
        throw std::runtime_error(msg);
    }
}

void TrajectoryFollowerImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if (localLinkId == 0)
    {
        try
        {
            dynamicsOutputSignal.componentState = componentState;
            data = std::make_shared<DynamicsSignal const>(dynamicsOutputSignal);
        }
        catch (const std::bad_alloc &)
        {
            ThrowCouldNotInstantiateSignalError();
        }
    }
    else
    {
        const std::string msg = std::string(COMPONENTNAME) + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void TrajectoryFollowerImplementation::Trigger([[maybe_unused]]int time)
{
    CalculateNextTimestep();
}

ComponentState TrajectoryFollowerImplementation::GetState() const
{
    return componentState;
}

void TrajectoryFollowerImplementation::SetComponentState(const ComponentState newState)
{
    componentState = newState;
    GetPublisher()->Publish(COMPONENTNAME, ComponentEvent({{"ComponentState", openpass::utils::to_string(componentState)}}));
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
                SetComponentState(newState);
                Init();
            }
        }
        else if (newState == ComponentState::Disabled)
        {
            canBeActivated = false;
            SetComponentState(newState);
        }
        else
        {
            throw std::runtime_error("TrajectoryFollower cannot handle new component state");
        }
    }
}

bool TrajectoryFollowerImplementation::CheckEndOfTrajectory()
{
    constexpr double EPSILON = 1e-3;
    if (nextTrajectoryIterator == trajectory.points.end())
    {
        return true;
    }
    if (!inputAccelerationActive)
    {
        if (nextTrajectoryIterator + 1 == trajectory.points.end()
            && nextTrajectoryIterator->time - lastCoordinateTimestamp < EPSILON)
        {
            return true;
        }
    }
    return false;
}

[[noreturn]] void TrajectoryFollowerImplementation::ThrowCouldNotInstantiateSignalError()
{
    const std::string msg = std::string(COMPONENTNAME) + " could not instantiate signal";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

[[noreturn]] void TrajectoryFollowerImplementation::ThrowInvalidSignalTypeError()
{
    const std::string msg = std::string(COMPONENTNAME) + " invalid signaltype";
    LOG(CbkLogLevel::Debug, msg);
    throw std::runtime_error(msg);
}

void TrajectoryFollowerImplementation::HandleEndOfTrajectory()
{
    dynamicsOutputSignal.velocity = 0;
    dynamicsOutputSignal.acceleration = 0;
    dynamicsOutputSignal.travelDistance = 0;
    dynamicsOutputSignal.yawRate = 0;

    if (automaticDeactivation)
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

            if (nextTrajectoryIterator != trajectory.points.end())
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

    double remainingTime = GetCycleTime() / 1000.0;
    double timeBetweenCoordinates = nextCoordinate.time - previousTimestamp;
    double deltaS{0};

    while (timeBetweenCoordinates <= remainingTime &&
           nextTrajectoryIterator != trajectory.points.end())
    {

        previousTrajectoryIterator = nextTrajectoryIterator;
        previousTimestamp = previousTrajectoryIterator->time;
        lastCoordinateTimestamp = previousTimestamp;

        nextTrajectoryIterator++;
        if (nextTrajectoryIterator != trajectory.points.end())
        {
            deltaS += CalculateDistanceBetweenWorldCoordinates(previousCoordinate, nextCoordinate);
            previousCoordinate = *previousTrajectoryIterator;
            nextCoordinate = *nextTrajectoryIterator;
            remainingTime -= timeBetweenCoordinates;
            timeBetweenCoordinates = nextCoordinate.time - previousTimestamp;
        }
    }

    const auto &previousPosition = previousCoordinate;
    const auto &nextPosition = nextCoordinate;

    percentageTraveledBetweenCoordinates = remainingTime / timeBetweenCoordinates;
    Common::Vector2d direction = CalculateScaledVector(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);
    const double deltaYawAngle = CalculateScaledDeltaYawAngle(previousPosition, nextPosition, percentageTraveledBetweenCoordinates);

    deltaS += direction.Length();
    dynamicsOutputSignal.travelDistance = deltaS;

    const double velocity = deltaS / cycleTimeInSeconds;
    const double acceleration = (velocity - lastVelocity) / cycleTimeInSeconds;

    UpdateDynamics(previousPosition, direction, deltaYawAngle, velocity, acceleration);
}

void TrajectoryFollowerImplementation::CalculateNextTimestep()
{
    lastCoordinateTimestamp = currentTime;
    currentTime += GetCycleTime() / 1000.0;

    if (GetState() == ComponentState::Disabled)
    {
        return;
    }

    if (CheckEndOfTrajectory())
    {
        HandleEndOfTrajectory();
    }
    else
    {
        if(inputAccelerationActive)
        {
            TriggerWithActiveAccelerationInput();
        }
        else
        {
            TriggerWithInactiveAccelerationInput();
        }
    }
}

Common::Vector2d TrajectoryFollowerImplementation::CalculateScaledVector(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor)
{
    Common::Vector2d result(nextPosition.x - previousPosition.x, nextPosition.y - previousPosition.y);
    result.Scale(factor);

    return result;
}

double TrajectoryFollowerImplementation::CalculateScaledDeltaYawAngle(const TrajectoryPoint &previousPosition, const TrajectoryPoint &nextPosition, const double &factor)
{
    return (nextPosition.yaw - previousPosition.yaw) * factor;
    ;
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

    lastWorldPosition = {currentTime,
                         dynamicsOutputSignal.positionX,
                         dynamicsOutputSignal.positionY,
                         dynamicsOutputSignal.yaw};
    lastVelocity = dynamicsOutputSignal.velocity;
}
