/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \file  CustomLaneChangeManipulator.h
*	\brief This manipulator sets a custom lane change for an agent.
*
*   \details    This manipulator sets a custom lane change for an agent.
*               Therefore it needs the number of lanes over which the
*               lane change should take place.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "ManipulatorCommonBase.h"
#include "Interfaces/agentInterface.h"
#include "Common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This manipulator sets a custom lane change for an agent.
*   \details    This manipulator sets a custom lane change for an agent.
*               Therefore it needs the number of lanes over which the
*               lane change should take place.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class CustomLaneChangeManipulator : public ManipulatorCommonBase
{
public:
    CustomLaneChangeManipulator(WorldInterface *world,
                          std::shared_ptr<openScenario::UserDefinedCommandAction> action,
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
    EventContainer GetEvents() override;

    int deltaLaneID;
};
