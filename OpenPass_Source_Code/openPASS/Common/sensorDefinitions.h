/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Common/parameter.h"

namespace openpass::sensors
{
struct Position
{
    std::string name {};
    double longitudinal {};
    double lateral {};
    double height {};
    double pitch {};
    double yaw {};
    double roll {};
};

struct Profile
{
    std::string name {};
    std::string type {};
    openpass::parameter::ParameterSetLevel1 parameter {};
};

struct Parameter
{
    int id {};
    Position position {};
    Profile profile {};
};

using Profiles = std::vector<Profile>;
using Parameters = std::vector<Parameter>;

} // namespace openpass::sensors
