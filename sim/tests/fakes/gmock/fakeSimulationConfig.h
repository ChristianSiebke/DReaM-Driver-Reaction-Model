/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/simulationConfigInterface.h"

class FakeSimulationConfig : public SimulationConfigInterface {
 public:
  MOCK_METHOD0(GetExperimentConfig,
      ExperimentConfig &());
  MOCK_METHOD0(GetScenarioConfig,
      ScenarioConfig &());
  MOCK_METHOD0(GetSpawnPointsConfig,
      SpawnPointLibraryInfoCollection &());
  MOCK_METHOD0(GetEnvironmentConfig,
      EnvironmentConfig &());
  MOCK_CONST_METHOD0(GetProfilesCatalog,
               std::string& ());
  MOCK_METHOD1(SetProfilesCatalog,
               void (std::string profilesCatalog));
};
