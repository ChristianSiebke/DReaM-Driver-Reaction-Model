/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <stdexcept>

#include "profiles.h"
#include <stdexcept>

std::unordered_map<std::string, AgentProfile>& Profiles::GetAgentProfiles()
{
    return agentProfiles;
}

std::unordered_map<std::string, VehicleProfile>& Profiles::GetVehicleProfiles()
{
    return vehicleProfiles;
}

ProfileGroups& Profiles::GetProfileGroups()
{
    return profileGroups;
}

StringProbabilities& Profiles::GetDriverProbabilities(std::string agentProfileName)
{
    try
    {
        return agentProfiles.at(agentProfileName).driverProfiles;
    }
    catch (const std::out_of_range&)
    {
        throw std::runtime_error("Cannot retrieve driver probabilities. Unknown agent profile " + agentProfileName);
    }
}

StringProbabilities& Profiles::GetVehicleProfileProbabilities(std::string agentProfileName)
{
    try
    {
        return agentProfiles.at(agentProfileName).vehicleProfiles;
    }
    catch (const std::out_of_range&)
    {
        throw std::runtime_error("Cannot retrieve vehicle probabilities. Unknown agent profile " + agentProfileName);
    }
}

openpass::parameter::ParameterSetLevel1 Profiles::GetProfile(std::string type, std::string name)
{
    if(profileGroups.find(type) != profileGroups.end()
       && profileGroups.at(type).find(name) != profileGroups.at(type).end())
    {
        return profileGroups[type][name];
    }

    throw std::runtime_error("Could not find Profile: " + name + " in ProfileGroup: " + type);
}
