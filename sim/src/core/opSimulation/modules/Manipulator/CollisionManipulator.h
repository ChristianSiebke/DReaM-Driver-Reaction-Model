/********************************************************************************
 * Copyright (c) 2020 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  CollisionManipulator.h
*	\brief This file notifies all necessary modules about the collision.
*
*   \details    This file notifies the event network that a collision happend,
*   which then forwards it to all relevant modules.
*   The colliding agents also get updated on the collision.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "common/events/collisionEvent.h"
#include "ManipulatorCommonBase.h"
#include "collisionDetection_Impact_implementation.h"
#include "common/coreDataPublisher.h"

//-----------------------------------------------------------------------------
/** \brief This class notifies all relevant modules about the collision.
* 	\details    This class notifies the event network and the colliding
*   agents about the collision.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class CollisionManipulator : public ManipulatorCommonBase
{
public:
    CollisionManipulator(WorldInterface *world,
                         core::EventNetworkInterface *eventNetwork,
                         const CallbackInterface *callbacks,
                         openpass::publisher::CoreDataPublisher* publisher);

    virtual ~CollisionManipulator() = default;

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    virtual void Trigger(int time) override;

private:
    struct CrashInfo
    {
        CollisionAngles angles;
        PostCrashDynamic postCrashDynamic1;
        PostCrashDynamic postCrashDynamic2;
    };

    EventContainer GetEvents() override;

    /** This function updates the collisionPartners of
    * the agent, the opponent and recursively of all
    * existing collisionPartners of both.
    */
    void UpdateCollision(AgentInterface* agent, AgentInterface* opponent);

    CrashInfo CalculateCrash(AgentInterface *agent, AgentInterface *opponent);

    //! Handles the calculation of post crash dynamics
    CollisionDetectionPostCrash collisionPostCrash;

    void PublishCrash(const std::shared_ptr<openpass::events::CollisionEvent>& event, const CrashInfo& crashInfo);

    openpass::publisher::CoreDataPublisher* publisher;
};
