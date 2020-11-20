/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

/** \file  CustomParametersManipulator.h
*	\brief This manipulator sends a custom parameter to anybody whos interested.
*
*   \details This manipulator sets a custom parameter to anybody whos interested
*            Syntax: `CustomCommand SetParameters STR1 STR2 ...` , where the STR 
             parameter are seperated at every whitespace and sent as vector.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "CustomCommandFactory.h"
#include "ManipulatorCommonBase.h"
#include "include/agentInterface.h"
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
/** \brief This manipulator sets a custom lane change for an agent.
*   \details    This manipulator sets a custom lane change for an agent.
*               Therefore it needs the number of lanes over which the
*               lane change should take place.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class CustomParametersManipulator : public ManipulatorCommonBase
{
public:
    CustomParametersManipulator(WorldInterface *world,
                                SimulationSlave::EventNetworkInterface *eventNetwork,
                                const CallbackInterface *callbacks,
                                const openScenario::CustomCommandAction action,
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
    EventContainer GetEvents() override;

    std::vector<std::string> customParameters;

    static inline bool registered  = CustomCommandFactory::Register<CustomParametersManipulator>("SetCustomParameters");
};
