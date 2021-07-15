/*******************************************************************************
* Copyright (c) 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  sceneryDynamicsInterface.h
//! @brief This file contains the interface to the dynamic portion of the scenery
//-----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "common/openScenarioDefinitions.h"

//-----------------------------------------------------------------------------
//! Class representing the dynamic portion of the scenery
//! i.e. states that may change during simulation
//-----------------------------------------------------------------------------
class SceneryDynamicsInterface
{
public:
    SceneryDynamicsInterface() = default;
    virtual ~SceneryDynamicsInterface() = default;

    //! Returns the environment as defined in the scenario file
    virtual openScenario::EnvironmentAction GetEnvironment() const = 0;

    //! Returns the list of traffic signal controllers as defined in the scenario file
    virtual std::vector<openScenario::TrafficSignalController> GetTrafficSignalControllers() const = 0;
};
