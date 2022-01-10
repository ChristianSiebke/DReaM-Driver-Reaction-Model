/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/opExport.h"
#include "common/log.h"
#include "include/vehicleModelsInterface.h"

namespace Configuration
{

class SIMULATIONCOREEXPORT VehicleModels : public VehicleModelsInterface
{
public:
    VehicleModels();
    ~VehicleModels();

    VehicleModelMap& GetVehicleModelMap();
    VehicleModelParameters GetVehicleModel(std::string vehicleModelType, const openScenario::Parameters& parameters = {});

private:
    VehicleModelMap vehicleModelMap;
};


} //namespace Configuration
