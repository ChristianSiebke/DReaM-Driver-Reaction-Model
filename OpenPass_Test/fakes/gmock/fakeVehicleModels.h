/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/vehicleModelsInterface.h"

class FakeVehicleModels : public VehicleModelsInterface {
 public:
  MOCK_METHOD0(GetVehicleModelMap,
      VehicleModelMap&());
  MOCK_METHOD2(GetVehicleModel,
      VehicleModelParameters(std::string, const openScenario::Parameters&));
};


