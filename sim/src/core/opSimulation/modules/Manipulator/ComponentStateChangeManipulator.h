/********************************************************************************
 * Copyright (c) 2017 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
/** \file  ComponentStateChangeManipulator.h
*	\brief This manipulator toggles the max state of a component
*
*   \details    This manipulator toggles the max state of a component
*               Therefore it needs the name of the component and the state in which
*               it should be put.
*/
//-----------------------------------------------------------------------------

#pragma once

#include "common/openScenarioDefinitions.h"

#include "CustomCommandFactory.h"
#include "ManipulatorCommonBase.h"

//-----------------------------------------------------------------------------
/** \brief This class toggles an assistance system.
* 	\details    This manipulator toggles the max state of a component
*               Therefore it needs the name of the component and the state in which
*               it should be put.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class ComponentStateChangeManipulator : public ManipulatorCommonBase
{
public:
    ComponentStateChangeManipulator(WorldInterface *world,
                                    core::EventNetworkInterface *eventNetwork,
                                    const CallbackInterface *callbacks,
                                    const openScenario::CustomCommandAction action,
                                    const std::string &eventName);

    virtual ~ComponentStateChangeManipulator() = default;

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
    bool AssignComponentState(const std::string& componentState);
    EventContainer GetEvents() override;

    std::string componentName {""};
    std::string componentStateName {""};
    ComponentState componentState;

    static inline bool registered  = CustomCommandFactory::Register<ComponentStateChangeManipulator>("SetComponentState");
};
