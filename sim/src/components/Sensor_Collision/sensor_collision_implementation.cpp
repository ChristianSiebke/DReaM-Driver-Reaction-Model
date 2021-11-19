/********************************************************************************
 * Copyright (c) 2020-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <memory>
#include <qglobal.h>
#include "sensor_collision_implementation.h"

Sensor_Collision_Implementation::Sensor_Collision_Implementation(
        std::string componentName,
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
    SensorInterface(
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
        agent)
{
    LOGINFO("Constructing Sensor_Collision");
    penetrationTime = parameters->GetParametersInt().at("0");
    LOGINFO(QString().sprintf("Constructing Sensor_Collision successful with t_hold = %d ms", penetrationTime).toStdString());
}

Sensor_Collision_Implementation::~Sensor_Collision_Implementation()
{
}

void Sensor_Collision_Implementation::UpdateInput(int localLinkId,
                                                  const std::shared_ptr<SignalInterface const> &data, int time)
{
    // no input necessary
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);

    std::runtime_error("invalid link");
}

void Sensor_Collision_Implementation::UpdateOutput(int localLinkId,
                                                   std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " UpdateOutput";
    LOGDEBUG(log.str());
    log.str(std::string());

    bool success = outputPorts.at(localLinkId)->GetSignalValue(data);

    if (success)
    {
        log << COMPONENTNAME << " UpdateOutput successful";
        LOGDEBUG(log.str());
    }
    else
    {
        log << COMPONENTNAME << " UpdateOutput failed";
        LOGERROR(log.str());
    }
}

void Sensor_Collision_Implementation::Trigger(int time)
{
    double timeSec = (double)time / 1000.0;
    int numberOfCollisionData = GetAgent()->GetCollisionPartners().size();

    CollisionState nextState = CollisionState::NOCOLLISION;

    switch (collisionState)
    {
    case CollisionState::NOCOLLISION:
        if (previousNumberOfCollisionData == numberOfCollisionData)
        {
            nextState = CollisionState::NOCOLLISION;
        }
        else
        {
            if (penetrationTime < GetCycleTime())
            {
                nextState = CollisionState::COLLISION;
            }
            else
            {
                timeOfFirstCollision = time;
                nextState = CollisionState::COLLISIONPENETRATING;
            }
        }
        break;
    case CollisionState::COLLISIONPENETRATING:
        if (((time - timeOfFirstCollision) < (penetrationTime -
                                              GetCycleTime())) // collision penetration time is not over
                && (previousNumberOfCollisionData == numberOfCollisionData))   // collision is still happening
        {
            nextState = CollisionState::COLLISIONPENETRATING;
        }
        else
        {
            nextState = CollisionState::COLLISION;
        }
        break;
    case CollisionState::COLLISION:
        if (previousNumberOfCollisionData == numberOfCollisionData)
        {
            nextState = CollisionState::NOCOLLISION;
        }
        else
        {
            nextState = CollisionState::COLLISION;
        }
        break;
    }

    switch (nextState)
    {
    case CollisionState::NOCOLLISION:
        LOGDEBUG(QString().sprintf("t = %f s: no collision detected.", timeSec).toStdString());
        collisionOccured.SetValue(false);
        break;
    case CollisionState::COLLISIONPENETRATING:
        LOGDEBUG(QString().sprintf("t = %f s: collision detected! Penetration phase...", timeSec).toStdString());
        collisionOccured.SetValue(false);
        //GetAgent()->SetCollisionState(false);
        break;
    case CollisionState::COLLISION:
        LOGDEBUG(QString().sprintf("t = %f s: collision detected! Apply impulse!", timeSec).toStdString());
        collisionOccured.SetValue(true);
        //GetAgent()->SetCollisionState(true);
        break;
    }

    collisionState = nextState;

    previousNumberOfCollisionData = numberOfCollisionData;
}
