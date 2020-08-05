/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2017, 2018, 2019 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  dynamics_collisionPostCrashImplementation.cpp
//! @brief This file contains the implementation header file
//-----------------------------------------------------------------------------

/**
 * @defgroup module_coll Collision post crash model
 * The dynamics after a collision is modelled.
 *
 * \image html collAngles.png "Definition of original collision angles at time of first contact"
 * The transformed collision angles HCPA and OCPA are scaled such that width and length of the
 * vehicle are 1.
 * For more information see http://indexsmart.mirasmart.com/26esv/PDFfiles/26ESV-000177.pdf
 * \image html collision.png "Calculation of post crash dynamics takes place after penetration time"
 *
 */

/**
 * @ingroup module_coll
 * @defgroup retrieve_coll Retrieve collision partners
*/
/**
 * @ingroup module_coll
 * @defgroup pc_fading Post crash fading out
*/

#include "dynamics_collisionPostCrashImplementation.h"
#include <qglobal.h>

DynamicsPostCrashImplementation::DynamicsPostCrashImplementation(std::string componentName,
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
                                                                 AgentInterface *agent)
    : UnrestrictedModelInterface { componentName,
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
                                   agent },
      dynamicsSignal{}
{
    dynamicsSignal.componentState = ComponentState::Disabled;
}

void DynamicsPostCrashImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void DynamicsPostCrashImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(time);

    if(localLinkId == 0)
    {
        try
        {
            data = std::make_shared<DynamicsSignal const>(dynamicsSignal);
        }
        catch(const std::bad_alloc&)
        {
            const std::string msg = COMPONENTNAME + " could not instantiate signal";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid link";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void DynamicsPostCrashImplementation::Trigger(int time)
{
    bool signalSet = false;
    /** @addtogroup retrieve_coll
     * - For the given host agent, retrieve the list of collision partners from the simulation core.
     * - Iterate over collision partners.
     * - Trigger calculation of post crash dynamics.
     */
    if (GetAgent()->GetCollisionPartners().size() > numberOfCollisionPartners)
    {
        numberOfCollisionPartners = GetAgent()->GetCollisionPartners().size();
        isActive = true;
        dynamicsSignal.componentState = ComponentState::Acting;

        signalSet = TriggerPostCrashCheck(time);
    }

    /** @addtogroup pc_fading
     * In case the collision has occurred before:
     * - Calculate a dynamics signal for fading out.
     */
    else if (isActive)
    {
        SetFadingDynamics();
        signalSet = true;
    }
    if (!signalSet) {
        SetFadingDynamics();
    }
}

void DynamicsPostCrashImplementation::SetFadingDynamics()
{
    dynamicsSignal.yaw = GetAgent()->GetYaw();
    dynamicsSignal.yawRate = GetAgent()->GetYawRate();
    const double deceleration = 10.0;
    velocity -= deceleration * GetCycleTime() * 0.001;
    velocity = std::max(0.0, velocity);
    // change of path coordinate since last time step [m]
    double ds = velocity * static_cast<double>(GetCycleTime()) * 0.001;
    // change of inertial x-position due to ds and yaw [m]
    double dx = ds * std::cos(movingDirection);
    // change of inertial y-position due to ds and yaw [m]
    double dy = ds * std::sin(movingDirection);
    // new inertial x-position [m]
    double x = GetAgent()->GetPositionX() + dx;
    // new inertial y-position [m]
    double y = GetAgent()->GetPositionY() + dy;

    dynamicsSignal.velocity = velocity;
    dynamicsSignal.acceleration = 0.0;
    dynamicsSignal.positionX = x;
    dynamicsSignal.positionY = y;
    dynamicsSignal.travelDistance = ds;
}

bool DynamicsPostCrashImplementation::TriggerPostCrashCheck(int time)
{
    PostCrashVelocity postCrashVel = GetAgent()->GetPostCrashVelocity();
    if (!postCrashVel.isActive) {
        return false;
    }

    double cycleTime = (double)GetCycleTime() / 1000;
    double yawAngle = GetAgent()->GetYaw();
    Common::Vector2d velocityPrevious(GetAgent()->GetVelocity(VelocityScope::Longitudinal), GetAgent()->GetVelocity(VelocityScope::Lateral)); // car CS
    velocityPrevious.Rotate(yawAngle); // global CS
    double posX = GetAgent()->GetPositionX() + velocityPrevious.x * cycleTime;//global CS
    double posY = GetAgent()->GetPositionY() + velocityPrevious.y * cycleTime;//global CS
    yawAngle = yawAngle + GetAgent()->GetYawRate() * cycleTime;

    velocity = postCrashVel.velocityAbsolute;
    movingDirection = postCrashVel.velocityDirection;
    double yawRate = postCrashVel.yawVelocity;

    Common::Vector2d velocityVector(velocity * std::cos(movingDirection),
                                    velocity * std::sin(movingDirection));
    velocityVector.Rotate(-yawAngle);

    double acceleration = 0.0;
    double travelDist = velocity * cycleTime;

    dynamicsSignal.yaw = yawAngle;
    dynamicsSignal.yawRate = yawRate;
    dynamicsSignal.velocity = velocity;
    dynamicsSignal.acceleration = acceleration;
    dynamicsSignal.positionX = posX;
    dynamicsSignal.positionY = posY;
    dynamicsSignal.travelDistance = travelDist;

    GetAgent()->SetPostCrashVelocity({false, 0.0, 0.0, 0.0});

    return true;
}
