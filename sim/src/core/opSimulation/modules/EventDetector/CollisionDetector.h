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
/** @file  CollisionDetector.h
*	@brief Detects wether agents collided.
*
*   This manipulator detects wether agents collided with either other agents or traffic objects.
*   Once a collision is detected a CollisionEvent is created and forwarded to the EventNetwork. */
//-----------------------------------------------------------------------------

#pragma once

#include "EventDetectorCommonBase.h"

#include "common/boostGeometryCommon.h"
#include "common/vector2d.h"

class AgentInterface;
class TrafficObjectInterface;
class WorldObjectInterface;


typedef enum
{
    UpperLeft = 0,
    UpperRight,
    LowerRight,
    LowerLeft,
    NumberCorners
} CornerType;

typedef enum
{
    UpperEdge = 0,
    RightEdge,
    LowerEdge,
    LeftEdge,
    NumberEdges
} EdgeType;

typedef enum
{
    Right = 0,
    Up,
    NumberNormals
} NormalType;

typedef enum
{
    ProjectedFirst = 0,
    ProjectedSecond,
    NumberProjectedOwnAxis,
    ProjectedThird = 2,
    ProjectedFourth,
    NumberProjectedOpponentAxis
} ProjectedType;

static const double ROTATION_EPS = 0.0001;

//-----------------------------------------------------------------------------
/** \brief This class detectes wether a collision happen in the simulation.
*   \details This class detects wether an agent collided with either another agent
*   or a traffic object. In case a collision happend an event is created.
*
* 	\ingroup EventDetector */
//-----------------------------------------------------------------------------
class CollisionDetector : public EventDetectorCommonBase
{
public:
    CollisionDetector(WorldInterface *world,
                      core::EventNetworkInterface *eventNetwork,
                      const CallbackInterface *callbacks,
                      StochasticsInterface *stochastics);

    virtual ~CollisionDetector() = default;

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    virtual void Trigger(int time);

    //-----------------------------------------------------------------------------
    /*! Check For Collision between worldObjects
    *
    * @param[in]  other          pointer to worldObject
    * @param[in]  agent          pointer to agent
    *
    * @return                    true when collision detected */
    //-----------------------------------------------------------------------------
    bool DetectCollision(const WorldObjectInterface *other,
                         AgentInterface *agent);

    const std::map<int, AgentInterface*> *agents = nullptr;
    const std::vector<const TrafficObjectInterface*> *trafficObjects = nullptr;

private:
    //-----------------------------------------------------------------------------
    /*! Creates a CollisionEvent and inserts it into the event network
    *
    * @param[in]  time
    * @param[in]  agent          pointer to agent
    * @param[in]  other          pointer to other world object */
    //-----------------------------------------------------------------------------
    void DetectedCollisionWithObject(int time,
                                     AgentInterface *agent,
                                     const WorldObjectInterface *other);

    //-----------------------------------------------------------------------------
    /*! Creates a CollisionEvent and inserts it into the event network
    *
    * @param[in]  time
    * @param[in]  agent          pointer to agent
    * @param[in]  other          pointer to other agent */
    //-----------------------------------------------------------------------------
    void DetectedCollisionWithAgent(int time,
                                    AgentInterface *agent,
                                    AgentInterface *other);
};


