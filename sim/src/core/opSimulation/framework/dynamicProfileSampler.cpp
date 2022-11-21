/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "dynamicProfileSampler.h"
#include "sampler.h"

DynamicProfileSampler& DynamicProfileSampler::SampleDriverProfile()
{
    auto probabilities = profiles->GetDriverProbabilities(agentProfileName);
    sampledProfiles.driverProfileName = Sampler::Sample(probabilities, &stochastics);
    return *this;
}

DynamicProfileSampler& DynamicProfileSampler::SampleVehicleProfile()
{
    auto probabilities = profiles->GetVehicleProfileProbabilities(agentProfileName);
    sampledProfiles.vehicleProfileName = Sampler::Sample(probabilities, &stochastics);
    return *this;
}

DynamicProfileSampler& DynamicProfileSampler::SampleVehicleComponentProfiles()
{
    auto find_result = profiles->GetVehicleProfiles().find(sampledProfiles.vehicleProfileName);
    if (find_result == profiles->GetVehicleProfiles().end())
    {
        throw std::runtime_error("No vehicle profile with name \""+sampledProfiles.vehicleProfileName+"\" defined");
    }
    VehicleProfile& vehicleProfile = find_result->second;
    for (VehicleComponent vehicleComponentInProfile : vehicleProfile.vehicleComponents)
    {
        std::string vehicleComponentName = Sampler::Sample(vehicleComponentInProfile.componentProfiles, &stochastics);
        if (vehicleComponentName != "")
        {
            sampledProfiles.vehicleComponentProfileNames.insert(std::make_pair<std::string&, std::string&>
                    (vehicleComponentInProfile.type,
                     vehicleComponentName));
        }
    }
    return *this;
}

DynamicProfileSampler SampledProfiles::make(std::string agentProfileName,
    StochasticsInterface& stochastics,
    ProfilesInterface* profiles)
{
    return DynamicProfileSampler(agentProfileName, stochastics, profiles);
}
