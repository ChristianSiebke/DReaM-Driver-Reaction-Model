/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/sceneryDynamicsInterface.h"
#include "common/opExport.h"

class SIMULATIONCOREEXPORT SceneryDynamics : public SceneryDynamicsInterface
{
public:
    SceneryDynamics();
    virtual ~SceneryDynamics() = default;

    openScenario::EnvironmentAction GetEnvironment() const override;

    //! Sets the environment from the scenario
    void SetEnvironment(const openScenario::EnvironmentAction& environment);

    std::vector<openScenario::TrafficSignalController> GetTrafficSignalControllers() const override;

    //! Adds a new controller to the list of traffic signal controllers
    //!
    //! \param controller   controller to add
    void AddTrafficSignalController(const openScenario::TrafficSignalController &controller);

private:
    openScenario::EnvironmentAction environment;
    std::vector<openScenario::TrafficSignalController> trafficSignalControllers;
};
