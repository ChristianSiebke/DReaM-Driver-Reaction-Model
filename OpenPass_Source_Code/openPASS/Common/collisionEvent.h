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
    CollisionEvent(int time,
                   std::string source,
                   std::string sequenceName,
                   EventDefinitions::EventType eventType,
                   bool collisionWithAgent,
                   int collisionAgentId,
                   int collisionOpponentId):
        BasicEvent(time,
                   source,
                   sequenceName,
                   eventType),
        collisionWithAgent(collisionWithAgent),
        collisionAgentId(collisionAgentId),
        collisionOpponentId(collisionOpponentId)
    {
        eventParameters.push_back(std::pair<std::string, std::string>("CollisionWithAgent", collisionWithAgent ? "true" : "false"));
        eventParameters.push_back(std::pair<std::string, std::string>("CollisionAgentId", std::to_string(collisionAgentId)));
        eventParameters.push_back(std::pair<std::string, std::string>("CollisionOpponentId", std::to_string(collisionOpponentId)));
    }
    CollisionEvent(const CollisionEvent&) = delete;
    CollisionEvent(CollisionEvent&&) = delete;
    CollisionEvent& operator=(const CollisionEvent&) = delete;
    CollisionEvent& operator=(CollisionEvent&&) = delete;
    virtual ~CollisionEvent() = default;

    /*!
    * \brief Returns all parameters of the event as string list.
    * \details Returns the CollisionWithAgent flag and both worldobject ids as string list.
    *
    * @return	     List of string pairs of the event parameters.
    */
    virtual std::list<std::pair<std::string, std::string>> GetEventParametersAsString()
    {
        return eventParameters;
    }

    /*!
    * \brief Adds parameters from the post crash calculation to event parameters.
    *
    * @return	     List of string pairs of the event parameters.
    */
    void AddParameters(CollisionAngles angles, PostCrashDynamic postCrashDyn1, PostCrashDynamic postCrashDyn2)
    {
        eventParameters.push_back(std::pair<std::string, std::string>("AgentVelocity", std::to_string(postCrashDyn1.GetVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentVelocityChange", std::to_string(postCrashDyn1.GetVelocityChange())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentVelocityDirection", std::to_string(postCrashDyn1.GetVelocityDirection())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentYawVelocity", std::to_string(postCrashDyn1.GetYawVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPulseX", std::to_string(postCrashDyn1.GetPulse().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPulseY", std::to_string(postCrashDyn1.GetPulse().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPulseDirection", std::to_string(postCrashDyn1.GetPulseDirection())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPulseLocalX", std::to_string(postCrashDyn1.GetPulseLocal().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPulseLocalY", std::to_string(postCrashDyn1.GetPulseLocal().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPointOfContactLocalX", std::to_string(postCrashDyn1.GetPointOfContactLocal().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentPointOfContactLocalY", std::to_string(postCrashDyn1.GetPointOfContactLocal().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentCollisionVelocity", std::to_string(postCrashDyn1.GetCollisionVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("AgentSliding", std::to_string(postCrashDyn1.GetSliding())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentVelocity", std::to_string(postCrashDyn2.GetVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentVelocityChange", std::to_string(postCrashDyn2.GetVelocityChange())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentVelocityDirection", std::to_string(postCrashDyn2.GetVelocityDirection())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentYawVelocity", std::to_string(postCrashDyn2.GetYawVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPulseX", std::to_string(postCrashDyn2.GetPulse().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPulseY", std::to_string(postCrashDyn2.GetPulse().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPulseDirection", std::to_string(postCrashDyn2.GetPulseDirection())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPulseLocalX", std::to_string(postCrashDyn2.GetPulseLocal().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPulseLocalY", std::to_string(postCrashDyn2.GetPulseLocal().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPointOfContactLocalX", std::to_string(postCrashDyn2.GetPointOfContactLocal().x)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentPointOfContactLocalY", std::to_string(postCrashDyn2.GetPointOfContactLocal().y)));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentCollisionVelocity", std::to_string(postCrashDyn2.GetCollisionVelocity())));
        eventParameters.push_back(std::pair<std::string, std::string>("OpponentSliding", std::to_string(postCrashDyn2.GetSliding())));
        eventParameters.push_back(std::pair<std::string, std::string>("OYA", std::to_string(angles.OYA)));
        eventParameters.push_back(std::pair<std::string, std::string>("HCPAo", std::to_string(angles.HCPAo)));
        eventParameters.push_back(std::pair<std::string, std::string>("OCPAo", std::to_string(angles.OCPAo)));
        eventParameters.push_back(std::pair<std::string, std::string>("HCPA", std::to_string(angles.HCPA)));
        eventParameters.push_back(std::pair<std::string, std::string>("OCPA", std::to_string(angles.OCPA)));
    }

    std::list<std::pair<std::string, std::string>> eventParameters {};

    bool collisionWithAgent;
    int collisionAgentId;
    int collisionOpponentId;
};

