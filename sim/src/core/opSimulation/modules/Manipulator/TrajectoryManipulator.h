/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/** \file  TrajectoryManipulator.h
*	\brief This manipulator sets a trajectory for an agent.
*
*   \details    This manipulator sets a trajectory for an agent.
*/

#pragma once

#include "ManipulatorCommonBase.h"
#include "include/agentInterface.h"
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This manipulator sets a trajectory for an agent.
*   \details    This manipulator sets a trajectory for an agent.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class TrajectoryManipulator : public ManipulatorCommonBase
{
public:
    TrajectoryManipulator(WorldInterface *world,
                          core::EventNetworkInterface *eventNetwork,
                          const CallbackInterface *callbacks,
                          const openScenario::FollowTrajectoryAction action,
                          const std::string &eventName);

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
    openScenario::FollowTrajectoryAction action;

    EventContainer GetEvents() override;
};
