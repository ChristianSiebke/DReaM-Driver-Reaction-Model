/*********************************************************************
* Copyright (c) 2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "dynamics_collision_implementation.h"
#include <qglobal.h>
#include <QString>

Dynamics_Collision_Implementation::Dynamics_Collision_Implementation(
        std::string componentName,
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
        AgentInterface *agent) :
    DynamicsInterface(
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
    LOGINFO("Constructing Dynamics_Collision");
    cycleTimeSec = (double)cycleTime / 1000;
    LOGINFO("Constructing Dynamics_Collision successful");
}

void Dynamics_Collision_Implementation::UpdateInput(int localLinkId,
                                                    const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    std::stringstream log;
    log << COMPONENTNAME << " UpdateInput";
    LOG(CbkLogLevel::Debug, log.str());
    log.str(std::string());

    bool success = inputPorts.at(localLinkId)->SetSignalValue(data);

    if (success)
    {
        log << COMPONENTNAME << " UpdateInput successful";
        LOG(CbkLogLevel::Debug, log.str());
    }
    else
    {
        log << COMPONENTNAME << " UpdateInput failed";
        LOG(CbkLogLevel::Error, log.str());
    }
}

void Dynamics_Collision_Implementation::UpdateOutput(int localLinkId,
                                                     std::shared_ptr<SignalInterface const> &data, int time)
{
    // no outputs of the module
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);

    std::runtime_error("invalid link");
}

void Dynamics_Collision_Implementation::Trigger(int time)
{
    Q_UNUSED(time);

    CollisionState nextState = CollisionState::NOCOLLISION;
    bool in_collisionOccured = collisionOccured.GetValue();
    switch (collisionState)
    {
    case CollisionState::NOCOLLISION:
        if (in_collisionOccured)
        {
            nextState = CollisionState::COLLISIONIMPULS;
        }
        else
        {
            nextState = CollisionState::NOCOLLISION;
        }
        break;
    case CollisionState::COLLISIONIMPULS:
        if (in_collisionOccured)
        {
            nextState = CollisionState::COLLISION;
        }
        else
        {
            nextState = CollisionState::NOCOLLISION;
        }
        break;
    case CollisionState::COLLISION:
        if (in_collisionOccured)
        {
            nextState = CollisionState::COLLISION;
        }
        else
        {
            nextState = CollisionState::NOCOLLISION;
        }
        break;
    }

    collisionState = nextState; // immediate transition

    if (collisionState == CollisionState::COLLISIONIMPULS)
    {
        auto collisionPartners = GetAgent()->GetCollisionPartners();

        if (collisionPartners.size() > 0)
        {
            PostCrashVelocity postCrashVel = GetAgent()->GetPostCrashVelocity();

            Common::Vector2d velocityPrevious(
                        GetAgent()->GetVelocity(VelocityScope::DirectionX),
                        GetAgent()->GetVelocity(VelocityScope::DirectionY)); // global CS

            GetAgent()->SetPositionX(GetAgent()->GetPositionX() + velocityPrevious.x * cycleTimeSec); // global CS
            GetAgent()->SetPositionY(GetAgent()->GetPositionY() + velocityPrevious.y * cycleTimeSec); // global CS
            GetAgent()->SetYaw(GetAgent()->GetYaw() + GetAgent()->GetYawRate()*cycleTimeSec);

            Common::Vector2d velocityImpact(postCrashVel.velocityAbsolute, 0.0);
            velocityImpact.Rotate(postCrashVel.velocityDirection);
            GetAgent()->SetVelocityVector(velocityImpact.x, velocityImpact.y, 0.0);
            GetAgent()->SetYawRate(postCrashVel.yawVelocity);
            LOGDEBUG(QString().sprintf("Override Velocity by Dynamics_CollisionPCM for agent %d: %f, %f, %f",
                                       GetAgent()->GetId(), velocityImpact.x, velocityImpact.y, postCrashVel.yawVelocity).toStdString());

        }
        Common::Vector2d accelerationImpact(0.0, 0.0);
        double yawAccel = 0.0;
        GetAgent()->SetTangentialAcceleration(accelerationImpact.x);
        GetAgent()->SetCentripetalAcceleration(accelerationImpact.y);
        GetAgent()->SetYawAcceleration(yawAccel);

        // for logging purposes: only longitudinal acceleration in vehicle CS
        GetAgent()->SetAcceleration(accelerationImpact.x);

        // Application of acceleration instead of setting velocities is not possible:
        // The acceleration values are far too high for a single simulation step (200g and above)     

        LOGDEBUG(QString().sprintf("Override Acceleration by Dynamics_CollisionPCM for agent %d: %f, %f, %f",
                                   GetAgent()->GetId(), accelerationImpact.x, accelerationImpact.y, yawAccel).toStdString());
    }
}
