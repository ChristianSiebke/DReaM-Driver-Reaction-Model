/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  agentUpdaterImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "agentUpdaterImpl.h"
#include <qglobal.h>

void AgentUpdaterImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, [[maybe_unused]] int time)
{
    if (localLinkId == 0)
        {
            // from DynamicsPrioritizer
            const std::shared_ptr<DynamicsSignal const> signal = std::dynamic_pointer_cast<DynamicsSignal const>(data);
            if (!signal)
            {
                const std::string msg = COMPONENTNAME + " invalid signaltype";
                LOG(CbkLogLevel::Debug, msg);
                throw std::runtime_error(msg);
            }

            acceleration = signal->acceleration;
            velocity = signal->velocity;
            positionX = signal->positionX;
            positionY = signal->positionY;
            yaw = signal->yaw;
            yawRate = signal->yawRate;
            roll = signal->roll;
            steeringWheelAngle = signal->steeringWheelAngle;
            centripetalAcceleration = signal->centripetalAcceleration;
            travelDistance = signal->travelDistance;
        }
        else
        {
            const std::string msg = COMPONENTNAME + " invalid link";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
}

void AgentUpdaterImplementation::UpdateOutput([[maybe_unused]] int localLinkId,
                                              [[maybe_unused]] std::shared_ptr<SignalInterface const> &data,
                                              [[maybe_unused]] int time)
{
}

void AgentUpdaterImplementation::Trigger([[maybe_unused]] int time)
{
    AgentInterface *agent = GetAgent();

    Validate(acceleration, "acceleration");
    agent->SetAcceleration(acceleration);
    Validate(velocity, "velocity");
    agent->SetVelocity(velocity);
    Validate(positionX, "positionX");
    agent->SetPositionX(positionX);
    Validate(positionY, "positionY");
    agent->SetPositionY(positionY);
    Validate(yaw, "yaw");
    agent->SetYaw(yaw);
    Validate(yawRate, "yawRate");
    agent->SetYawRate(yawRate);
    Validate(roll, "roll");
    agent->SetRoll(roll);
    Validate(steeringWheelAngle, "steeringWheelAngle");
    agent->SetSteeringWheelAngle(steeringWheelAngle);
    Validate(centripetalAcceleration, "centripetalAcceleration");
    agent->SetCentripetalAcceleration(centripetalAcceleration);
    Validate(travelDistance, "travelDistance");
    agent->SetDistanceTraveled(agent->GetDistanceTraveled() + travelDistance);
}


void AgentUpdaterImplementation::Validate(double value, const std::string &description)
{
    if (std::isnan(value))
    {
        LOGERRORANDTHROW("AgentUpdater got NaN as value of " + description + " for Agent " + std::to_string(GetAgent()->GetId()));
    }
}
