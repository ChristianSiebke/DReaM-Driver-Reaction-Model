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

/** \addtogroup Dynamics_CollisionPostCrash
* @{
*
* \brief Collision component that calculates post-crash dynamics.
*
* The algorithms for collision detection and calculation of the post-crash dynamics are based
* on algorithms provided by Harald Kolk (TU Graz).
*
* The algorithm for collision dection is based on the Sutherland–Hodgman algorithm for
* polygon clipping.
*
* The algorithm for calculating the post-crash-dynamics is based on conservation of momentum
* and angular momentum. It works in 2D and is only valid for non-sliding collisions.
*
* Note that this module is provisional regarding several aspect. openPASS needs to be
* changed in order to improve the following:
* - The post-crash dynamics is currently only calculated and written to the log-file since
*   openPASS does not allow to forward the results to dynamic-components and
*   observation modules, currently.
* - This module needs several parameters (coeffRest, interFriction and timeOfPenetration).
*   openPASS currently does not allow to set parameters for this module in the config files.
*   Therefore, these parameters are hard-coded.
* - The calculation of the post-crash dynamics is based on the assumptions that the time
*   where the collision starts is known since the post-crash dynamics shall be calculated a
*   certain time ( given by the parameter timeOfPenetration ) after the first contact of the
*   vehicles.
*   Currently, this module does not know anything about sample rates etc. which makes it
*   difficult to find the correct time to calculate the post-crash dynamics.
* - The algorithm for the post-crash dynamics uses the pre-crash yaw velocities of both agents.
*   Since the yaw velocity is not available for the agents currently, the yaw velocities are
*   set to zero here.
*
* Note:
* - The method GetVertexTypes in collisionDetection assumes that the intersection of the two vehicles is rather small.
*   It has not been checked if the method covers all possible cases for bigger intersections which
*   are possible in openPASS depending on the cycle times.
*
* \section Dynamics_Collision_Outputs Outputs
* Output variables:
* name | meaning
* -----|------
* dynamicsSignal| Signal containing all dynamics information
*
* Output channel IDs:
* Output ID | signal class | contained variables
* ------------|--------------|-------------
* 0 |  DynamicsSignal | dynamicsSignal
*
* @} */

#pragma once

#include <memory>
#include "include/modelInterface.h"
#include "include/observationInterface.h"
#include "common/dynamicsSignal.h"
#include "include/worldInterface.h"

/**
* \ingroup Dynamics_CollisionPostCrash
*/
class DynamicsPostCrashImplementation : public UnrestrictedModelInterface
{
public:
    const std::string COMPONENTNAME = "DynamicsCollisionPostCrash";

    DynamicsPostCrashImplementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface * const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent);
    DynamicsPostCrashImplementation(const DynamicsPostCrashImplementation&) = delete;
    DynamicsPostCrashImplementation(DynamicsPostCrashImplementation&&) = delete;
    DynamicsPostCrashImplementation& operator=(const DynamicsPostCrashImplementation&) = delete;
    DynamicsPostCrashImplementation& operator=(DynamicsPostCrashImplementation&&) = delete;
    virtual ~DynamicsPostCrashImplementation() = default;

    /*!
    * \brief Update Inputs
    *
    * Function is called by framework when another component delivers a signal over
    * a channel to this component (scheduler calls update taks of other component).
    *
    * Refer to module description for input channels and input ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentInput"
    * @param[in]     data           Referenced signal (copied by sending component)
    * @param[in]     time           Current scheduling time
    */
    virtual void UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Update outputs.
    *
    * Function is called by framework when this component has to deliver a signal over
    * a channel to another component (scheduler calls update task of this component).
    *
    * Refer to module description for output channels and output ids.
    *
    * @param[in]     localLinkId    Corresponds to "id" of "ComponentOutput"
    * @param[out]    data           Referenced signal (copied by this component)
    * @param[in]     time           Current scheduling time
    */
    virtual void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time);

    /*!
    * \brief Process data within component.
    *
    * Function is called by framework when the scheduler calls the trigger task
    * of this component
    *
    * @param[in]     time           Current scheduling time
    */
    virtual void Trigger(int time);

private:

    /*!
    * \brief Process data for collisionPartners as given by simulation core.
    *
    * Function is called by Trigger
    *
    * @param[in]     time           Current scheduling time
    */
    bool TriggerPostCrashCheck(int time);

    /*!
    * \brief Calculate and set vehicle dynamics for fading out after collision.
    *
    * Function is called by Trigger
    */
    void SetFadingDynamics();

    /** \name Internal states
     * @{ */
    double velocity {};//!< host velocity
    double movingDirection {};//!< host velocity direction
    unsigned int numberOfCollisionPartners = 0;//!< number of collision partners of host agent
    bool isActive = false;//!< flag that shows if collision has occurred

    //! Specifies the current state of collision the agent is in
    enum class CollisionState
    {
        NOCOLLISION,
        COLLISIONIMPULS,
        COLLISION
    } collisionState = CollisionState::NOCOLLISION;
    /** @} */

    /** \name Internal objects
     * @{ */
    //! Output Signal
    DynamicsSignal dynamicsSignal;
    /** @} */
};
