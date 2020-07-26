/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  CollisionEvent.h
* @brief This file contains all functions for collision based events.
*
* This class contains all functionality of the module. */
//-----------------------------------------------------------------------------
#pragma once

#include "basicEvent.h"
#include "Common/globalDefinitions.h"
#include "Common/postCrashDynamic.h"

//-----------------------------------------------------------------------------
/** This class implements all functionality of the CollisionEvent.
 *
 * \ingroup Events */
//-----------------------------------------------------------------------------
class CollisionEvent : public BasicEvent
{
public:
    CollisionEvent(int time, std::string source, bool collisionWithAgent, int collisionAgentId, int collisionOpponentId) :
        BasicEvent{time, "Collision", source, {collisionAgentId, collisionOpponentId}, {}},
        collisionWithAgent{collisionWithAgent},
        collisionAgentId{collisionAgentId},
        collisionOpponentId{collisionOpponentId}
    {
        parameter.emplace("CollisionWithAgent", collisionWithAgent);
    }

    ~CollisionEvent() override = default;

    EventDefinitions::EventCategory GetCategory() const override
    {
        return EventDefinitions::EventCategory::OpenPASS;
    }

    EventParameters GetParametersAsString() override
    {
        EventParameters eventParameters = BasicEvent::GetParametersAsString();
        return eventParameters;
    }

    void AddParameters(CollisionAngles angles, PostCrashDynamic postCrashDyn1, PostCrashDynamic postCrashDyn2)
    {
        parameter.emplace("Velocity", std::to_string(postCrashDyn1.GetVelocity()));
        parameter.emplace("Velocity", std::to_string(postCrashDyn1.GetVelocity()));
        parameter.emplace("VelocityChange", std::to_string(postCrashDyn1.GetVelocityChange()));
        parameter.emplace("VelocityDirection", std::to_string(postCrashDyn1.GetVelocityDirection()));
        parameter.emplace("YawVelocity", std::to_string(postCrashDyn1.GetYawVelocity()));
        parameter.emplace("PointOfContactLocalX", std::to_string(postCrashDyn1.GetPointOfContactLocal().x));
        parameter.emplace("PointOfContactLocalY", std::to_string(postCrashDyn1.GetPointOfContactLocal().y));
        parameter.emplace("CollisionVelocity", std::to_string(postCrashDyn1.GetCollisionVelocity()));
        parameter.emplace("Sliding", std::to_string(postCrashDyn1.GetSliding()));
        parameter.emplace("OpponentVelocity", std::to_string(postCrashDyn2.GetVelocity()));
        parameter.emplace("OpponentVelocityChange", std::to_string(postCrashDyn2.GetVelocityChange()));
        parameter.emplace("OpponentVelocityDirection", std::to_string(postCrashDyn2.GetVelocityDirection()));
        parameter.emplace("OpponentYawVelocity", std::to_string(postCrashDyn2.GetYawVelocity()));
        parameter.emplace("OpponentPointOfContactLocalX", std::to_string(postCrashDyn2.GetPointOfContactLocal().x));
        parameter.emplace("OpponentPointOfContactLocalY", std::to_string(postCrashDyn2.GetPointOfContactLocal().y));
        parameter.emplace("OpponentCollisionVelocity", std::to_string(postCrashDyn2.GetCollisionVelocity()));
        parameter.emplace("OpponentSliding", std::to_string(postCrashDyn2.GetSliding()));
        parameter.emplace("OYA", std::to_string(angles.OYA));
        parameter.emplace("HCPAo", std::to_string(angles.HCPAo));
        parameter.emplace("OCPAo", std::to_string(angles.OCPAo));
        parameter.emplace("HCPA", std::to_string(angles.HCPA));
        parameter.emplace("OCPA", std::to_string(angles.OCPA));
    }

    [[deprecated("Will be replaced by BasicEvent::parameter")]] bool collisionWithAgent;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionAgentId;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionOpponentId;
};
