/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \file  LaneChangeManipulator.h
*	\brief This file removes the agent.
*
*   \details    This file removes an agent.
*/

#pragma once

#include "ManipulatorCommonBase.h"
#include "Interfaces/agentInterface.h"
#include "Common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This class removes an agent.
*   \details    This class removes the agent.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class LaneChangeManipulator : public ManipulatorCommonBase
{
public:
    LaneChangeManipulator(WorldInterface *world,
                          std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> action,
                          SimulationSlave::EventNetworkInterface *eventNetwork,
                          const CallbackInterface *callbacks);

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
    std::shared_ptr<openScenario::PrivateLateralLaneChangeAction> action;

    EventContainer GetEvents() override;
};
