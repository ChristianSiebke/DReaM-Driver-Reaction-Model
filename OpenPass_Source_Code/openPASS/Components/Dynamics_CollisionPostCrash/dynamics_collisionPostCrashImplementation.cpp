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
 * @defgroup pc_calc Post crash calculation
*/
/**
 * @ingroup module_coll
 * @defgroup pc_fading Post crash fading out
*/

#include "dynamics_collisionPostCrashImplementation.h"
#include <qglobal.h>

DynamicsCollisionPostCrashImplementation::DynamicsCollisionPostCrashImplementation(std::string componentName,
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

DynamicsCollisionPostCrashImplementation::~DynamicsCollisionPostCrashImplementation() {
    #ifdef POSTCRASH_DEBUG
        logFile.close();
    #endif
}

void DynamicsCollisionPostCrashImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time)
{
    Q_UNUSED(localLinkId);
    Q_UNUSED(data);
    Q_UNUSED(time);
}

void DynamicsCollisionPostCrashImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time)
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

void DynamicsCollisionPostCrashImplementation::Trigger(int time)
{
    #ifdef POSTCRASH_DEBUG
        if (clockCount == 0) {
            InitLog();
        }
        clockCount += timeStep_ms;
    #endif

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

        signalSet = TriggerPostCrashCalculation(time);
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

void DynamicsCollisionPostCrashImplementation::SetFadingDynamics()
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

bool DynamicsCollisionPostCrashImplementation::TriggerPostCrashCalculation(int time)
{
    auto collisionPartners = GetAgent()->GetCollisionPartners();

    /** @addtogroup pc_calc
     * Calculate a dynamics signal resulting from a crash.
     */
    bool dynamicsSignalIsSet = false;
    for (const auto &partner : collisionPartners)
    {
        //! Stores calculated dynamics for ego/host agent
        PostCrashDynamic postCrashDynamic1;
        //! Stores calculated dynamics for opponent agent
        PostCrashDynamic postCrashDynamic2;

        if (partner.first == ObjectTypeOSI::Object)
        {
            break;
        }
        int collAgentId = partner.second;

        int timeOfFirstContact = 0;
        if (!collisionDetection.CreatePostCrashDynamics(GetAgent(),
                                                        GetWorld()->GetAgent(collAgentId),
                                                        &postCrashDynamic1,
                                                        &postCrashDynamic2,
                                                        timeOfFirstContact)) {
            continue;
        }

        double cycleTime = (double)GetCycleTime() / 1000;

        double yawAngle = GetAgent()->GetYaw();
        Common::Vector2d velocityPrevious(GetAgent()->GetVelocity(VelocityScope::Longitudinal), GetAgent()->GetVelocity(VelocityScope::Lateral)); // car CS
        velocityPrevious.Rotate(yawAngle); // global CS
        double posX = GetAgent()->GetPositionX() + velocityPrevious.x * cycleTime;//global CS
        double posY = GetAgent()->GetPositionY() + velocityPrevious.y * cycleTime;//global CS
        yawAngle = yawAngle + GetAgent()->GetYawRate() * cycleTime;

        velocity = postCrashDynamic1.GetVelocity();
        movingDirection = postCrashDynamic1.GetVelocityDirection();
        Common::Vector2d velocityVector(velocity * std::cos(movingDirection),
                                        velocity * std::sin(movingDirection));
        velocityVector.Rotate(-yawAngle);

        double yawRate = postCrashDynamic1.GetYawVelocity();

        double acceleration = 0.0;
        double travelDist = velocity * cycleTime;

        dynamicsSignal.yaw = yawAngle;
        dynamicsSignal.yawRate = yawRate;
        dynamicsSignal.velocity = velocity;
        dynamicsSignal.acceleration = acceleration;
        dynamicsSignal.positionX = posX;
        dynamicsSignal.positionY = posY;
        dynamicsSignal.travelDistance = travelDist;

        dynamicsSignalIsSet = true;
        #ifdef POSTCRASH_DEBUG
            LogTimeStep(time + timeOfFirstContact, collAgentId, &postCrashDynamic1, &postCrashDynamic2);
        #endif
    }
    return dynamicsSignalIsSet;
}

void DynamicsCollisionPostCrashImplementation::InitLog()
{
    logFile.open("results/dataLog_PostCrash_" + std::to_string(GetAgent()->GetId()) + ".csv", std::ios::trunc);
    logFile << "time;agent;Velocity;VelocityChange;VelocityDirection;YawVelocity;PulseX;PulseY;PulseDirection;"
               "PulseLocalX;PulseLocalY;PointOfContactLocalX;PointOfContactLocalY;CollisionVelocity;Sliding;"
               "OYA;HCPAo;OCPAo;HCPA;OCPA\n";
}

void DynamicsCollisionPostCrashImplementation::LogTimeStep(int time, int collPartnerId, PostCrashDynamic* postCrashDynamic1, PostCrashDynamic* postCrashDynamic2)
{
    logFile << std::to_string(time) + ";"
               + std::to_string(GetAgent()->GetId()) + ";"
               + std::to_string(postCrashDynamic1->GetVelocity()) + ";"
               + std::to_string(postCrashDynamic1->GetVelocityChange()) + ";"
               + std::to_string(postCrashDynamic1->GetVelocityDirection()) + ";"
               + std::to_string(postCrashDynamic1->GetYawVelocity()) + ";"
               + std::to_string(postCrashDynamic1->GetPulse().x) + ";"
               + std::to_string(postCrashDynamic1->GetPulse().y) + ";"
               + std::to_string(postCrashDynamic1->GetPulseDirection()) + ";"
               + std::to_string(postCrashDynamic1->GetPulseLocal().x) + ";"
               + std::to_string(postCrashDynamic1->GetPulseLocal().y) + ";"
               + std::to_string(postCrashDynamic1->GetPointOfContactLocal().x) + ";"
               + std::to_string(postCrashDynamic1->GetPointOfContactLocal().y) + ";"
               + std::to_string(postCrashDynamic1->GetCollisionVelocity()) + ";"
               + std::to_string(postCrashDynamic1->GetSliding()) + ";"
               + std::to_string(collisionDetection.GetCollisionAngles().OYA) + ";"
               + std::to_string(collisionDetection.GetCollisionAngles().HCPAo) + ";"
               + std::to_string(collisionDetection.GetCollisionAngles().OCPAo) + ";"
               + std::to_string(collisionDetection.GetCollisionAngles().HCPA) + ";"
               + std::to_string(collisionDetection.GetCollisionAngles().OCPA) + ";"
            << std::endl;
    logFile << std::to_string(time) + ";"
               + std::to_string(collPartnerId) + ";"
               + std::to_string(postCrashDynamic2->GetVelocity()) + ";"
               + std::to_string(postCrashDynamic2->GetVelocityChange()) + ";"
               + std::to_string(postCrashDynamic2->GetVelocityDirection()) + ";"
               + std::to_string(postCrashDynamic2->GetYawVelocity()) + ";"
               + std::to_string(postCrashDynamic2->GetPulse().x) + ";"
               + std::to_string(postCrashDynamic2->GetPulse().y) + ";"
               + std::to_string(postCrashDynamic2->GetPulseDirection()) + ";"
               + std::to_string(postCrashDynamic2->GetPulseLocal().x) + ";"
               + std::to_string(postCrashDynamic2->GetPulseLocal().y) + ";"
               + std::to_string(postCrashDynamic2->GetPointOfContactLocal().x) + ";"
               + std::to_string(postCrashDynamic2->GetPointOfContactLocal().y) + ";"
               + std::to_string(postCrashDynamic2->GetCollisionVelocity()) + ";"
               + std::to_string(postCrashDynamic2->GetSliding()) + ";"
            << std::endl;
}
