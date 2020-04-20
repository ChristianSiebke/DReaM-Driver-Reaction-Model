/*******************************************************************************
* Copyright (c) 2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \file  GazeFollowerManipulator.h
*	\brief This manipulator sets the gaze for an agent.
*
*   \details    This manipulator sets the gaze for an agent.
*               Therefore it needs the name of the file that contains the gaze
*               information and the activity state in which it should be put.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "ManipulatorCommonBase.h"
#include "Interfaces/scenarioActionInterface.h"
#include "Common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This manipulator sets the gaze for an agent.
*   \details    This manipulator sets the gaze for an agent.
*               Therefore it needs the name of the file that contains the gaze
*               information and the activity state in which it should be put.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class GazeFollowerManipulator : public ManipulatorCommonBase
{
public:
    GazeFollowerManipulator(WorldInterface *world,
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

    std::string gazeActivityState {""};
    std::string gazeFileName {""};
};
