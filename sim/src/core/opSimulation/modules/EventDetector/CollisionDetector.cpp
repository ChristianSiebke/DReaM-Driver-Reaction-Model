/********************************************************************************
 * Copyright (c) 2016-2018 ITK Engineering GmbH
 *               2017-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** @file  CollisionDetector.cpp */
//-----------------------------------------------------------------------------

#include "CollisionDetector.h"

#include "include/agentInterface.h"
#include "include/trafficObjectInterface.h"
#include "include/worldObjectInterface.h"

#include "common/events/collisionEvent.h"
#include "common/commonTools.h"

CollisionDetector::CollisionDetector(WorldInterface *world,
                                     core::EventNetworkInterface *eventNetwork,
                                     const CallbackInterface *callbacks,
                                     StochasticsInterface *stochastics) :
    EventDetectorCommonBase(
        world,
        eventNetwork,
        callbacks,
        stochastics)
{
    cycleTime = 100;
    trafficObjects = &(world->GetTrafficObjects());
    agents = &(world->GetAgents());
}

void CollisionDetector::Trigger(int time)
{
    // accumulate collisions
    for (auto it = agents->cbegin(); it != agents->cend(); ++it)
    {
        AgentInterface *agent = it->second;
        assert(agent != nullptr);

        for (auto otherIt = std::next(it); otherIt != agents->cend(); ++otherIt)
        {
            AgentInterface *other = otherIt->second;
            if (!DetectCollision(other, agent))
            {
                continue;
            }
            else
            {
                // no separations given on any axis -> collision
                DetectedCollisionWithAgent(time, agent, other);
            }
        }

        // second loop to avoid comparing traffic objects with traffic objects
        for (auto trafficObjectIt = trafficObjects->begin(); trafficObjectIt != trafficObjects->end(); ++trafficObjectIt)
        {
            const TrafficObjectInterface *otherObject = *trafficObjectIt;
            if (!otherObject)
            {
                LOG(CbkLogLevel::Warning, "collision detection aborted");
                throw std::runtime_error("Invalid other worldObject. Collision detection cancled.");
            }
            if (!DetectCollision(otherObject, agent))
            {
                continue;
            }
            else
            {
                DetectedCollisionWithObject(time, agent, otherObject);
            }
        }
    }
}

template <typename T>
bool IsInVector(const std::vector<T> &v, T element)
{
    return std::find(v.begin(), v.end(), element) != v.end();
}

bool CollisionDetector::DetectCollision(const WorldObjectInterface *other,
                                        AgentInterface *agent)
{
    if (IsInVector(agent->GetCollisionPartners(), std::make_pair(other->GetType(), other->GetId())))
    {
        return false;
    }

    const auto& ownBoundingBox = agent->GetBoundingBox2D();
    const auto& otherBoundingBox = other->GetBoundingBox2D();
    const auto [netX, netY] = CommonHelper::GetCartesianNetDistance(ownBoundingBox, otherBoundingBox);
    if(netX != 0.0 || netY != 0.0)
    {
        return false;
    }

    return !CommonHelper::IntersectionCalculation::GetIntersectionPoints(ownBoundingBox, otherBoundingBox).empty();

}

void CollisionDetector::DetectedCollisionWithObject(int time, AgentInterface *agent, const WorldObjectInterface *other)
{
    eventNetwork->InsertEvent(std::make_shared<openpass::events::CollisionEvent>(time, COMPONENTNAME, false, agent->GetId(), other->GetId()));
}

void CollisionDetector::DetectedCollisionWithAgent(int time, AgentInterface *agent, AgentInterface *other)
{
    eventNetwork->InsertEvent(std::make_shared<openpass::events::CollisionEvent>(time, COMPONENTNAME, true, agent->GetId(), other->GetId()));
}
