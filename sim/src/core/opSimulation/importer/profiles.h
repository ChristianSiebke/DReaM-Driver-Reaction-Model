/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/opExport.h"
#include "include/profilesInterface.h"

class SIMULATIONCOREEXPORT Profiles : public ProfilesInterface
{
public:
    virtual ~Profiles() override = default;

    virtual std::unordered_map<std::string, AgentProfile>& GetAgentProfiles() override;

    virtual std::unordered_map<std::string, VehicleProfile>& GetVehicleProfiles() override;

    virtual ProfileGroups& GetProfileGroups() override;

    virtual StringProbabilities& GetDriverProbabilities(std::string agentProfileName) override;

    virtual StringProbabilities& GetVehicleProfileProbabilities(std::string agentProfileName) override;

    virtual openpass::parameter::ParameterSetLevel1 GetProfile(std::string type, std::string name) override;

private:
    std::unordered_map<std::string, AgentProfile> agentProfiles {};
    std::unordered_map<std::string, VehicleProfile> vehicleProfiles {};
    ProfileGroups profileGroups;
};
