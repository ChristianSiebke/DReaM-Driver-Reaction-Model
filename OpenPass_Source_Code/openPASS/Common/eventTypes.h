/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <map>
#include <string>

namespace EventDefinitions
{

enum struct EventCategory
{
    Basic = 0,
    Conditional,
    Collision,
    ComponentStateChange,
    LaneChange,
    SetTrajectory,
    VehicleComponent
};

}//namespace EventDefinitions
