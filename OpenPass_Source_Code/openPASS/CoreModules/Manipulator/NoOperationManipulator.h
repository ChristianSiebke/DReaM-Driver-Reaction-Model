/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/


#pragma once

#include "ManipulatorCommonBase.h"

//-----------------------------------------------------------------------------
/** \brief This class is a manipulator, that doesn't trigger any action and solely
* returns the event. It existes to met the OpenSCENARIO conventions.
*
* 	\ingroup Manipulator
*/
//-----------------------------------------------------------------------------
class NoOperationManipulator : public ManipulatorCommonBase
{
public:
    NoOperationManipulator(WorldInterface *world,
                         SimulationSlave::EventNetworkInterface *eventNetwork,
                         const CallbackInterface *callbacks);

    virtual ~NoOperationManipulator() = default;

    /*!
    * \brief Triggers no functionality
    *
    * \details Trigger gets called each cycle timestep.
    * This function is repsonsible for creating events
    *
    * @param[in]     time    Current time.
    */
    virtual void Trigger(int time);

private:
    EventContainer GetEvents() override;
};


