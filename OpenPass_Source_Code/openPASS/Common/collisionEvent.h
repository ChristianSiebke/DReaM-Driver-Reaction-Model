/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2020 ITK Engineering GmbH
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
        parameter.emplace("Velocity", postCrashDyn1.GetVelocity());
        parameter.emplace("Velocity", postCrashDyn1.GetVelocity());
        parameter.emplace("VelocityChange", postCrashDyn1.GetVelocityChange());
        parameter.emplace("VelocityDirection", postCrashDyn1.GetVelocityDirection());
        parameter.emplace("YawVelocity", postCrashDyn1.GetYawVelocity());
        parameter.emplace("PointOfContactLocalX", postCrashDyn1.GetPointOfContactLocal().x);
        parameter.emplace("PointOfContactLocalY", postCrashDyn1.GetPointOfContactLocal().y);
        parameter.emplace("CollisionVelocity", postCrashDyn1.GetCollisionVelocity());
        parameter.emplace("Sliding", postCrashDyn1.GetSliding());
        parameter.emplace("OpponentVelocity", postCrashDyn2.GetVelocity());
        parameter.emplace("OpponentVelocityChange", postCrashDyn2.GetVelocityChange());
        parameter.emplace("OpponentVelocityDirection", postCrashDyn2.GetVelocityDirection());
        parameter.emplace("OpponentYawVelocity", postCrashDyn2.GetYawVelocity());
        parameter.emplace("OpponentPointOfContactLocalX", postCrashDyn2.GetPointOfContactLocal().x);
        parameter.emplace("OpponentPointOfContactLocalY", postCrashDyn2.GetPointOfContactLocal().y);
        parameter.emplace("OpponentCollisionVelocity", postCrashDyn2.GetCollisionVelocity());
        parameter.emplace("OpponentSliding", postCrashDyn2.GetSliding());
        parameter.emplace("OYA", angles.OYA);
        parameter.emplace("HCPAo", angles.HCPAo);
        parameter.emplace("OCPAo", angles.OCPAo);
        parameter.emplace("HCPA", angles.HCPA);
        parameter.emplace("OCPA", angles.OCPA);
    }

    [[deprecated("Will be replaced by BasicEvent::parameter")]] bool collisionWithAgent;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionAgentId;
    [[deprecated("Will be replaced by BasicEvent::parameter")]] int collisionOpponentId;
};
