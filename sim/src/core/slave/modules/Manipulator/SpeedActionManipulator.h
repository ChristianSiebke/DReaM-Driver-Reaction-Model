/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SpeedActionManipulator.h
*   \brief This manipulator adjusts the acceleration of an actor based on different variables
*/
//-----------------------------------------------------------------------------

#pragma once

#include "ManipulatorCommonBase.h"
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This class emits the SpeedActionEvent.
* 	\details    This class emits the SpeedActionEvent which allows for an acceleration manipulation of actors.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class SpeedActionManipulator : public ManipulatorCommonBase
{
public:
    SpeedActionManipulator(WorldInterface *world,
                                                 SimulationSlave::EventNetworkInterface *eventNetwork,
                                                 const CallbackInterface *callbacks,
                                                 const openScenario::SpeedAction action,
                                                 const std::string &eventName);

    virtual ~SpeedActionManipulator() = default;

    /*!
    * \brief Triggers the functionality of this class
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    virtual void Trigger(int time);

private:
    EventContainer GetEvents() override;

    const openScenario::SpeedAction action;
};
