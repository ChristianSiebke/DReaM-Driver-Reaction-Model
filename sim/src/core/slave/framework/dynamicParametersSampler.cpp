/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "dynamicParametersSampler.h"
#include "sampler.h"
#include <algorithm>

DynamicParameterSampler::DynamicParameterSampler(StochasticsInterface& stochastics,
                                                 std::string &vehicleProfileName,
                                                 ProfilesInterface* profiles) :
    stochastics(stochastics),
    vehicleProfileName(vehicleProfileName),
    profiles(profiles)
{
}

DynamicParameterSampler &DynamicParameterSampler::SampleSensorLatencies()
{
    const auto& vehicleProfiles = profiles->GetVehicleProfiles();

    if (vehicleProfiles.find(vehicleProfileName) != vehicleProfiles.end())
    {
        for (const auto& sensor : vehicleProfiles.at(vehicleProfileName).sensors)
        {
            try
            {
                const auto& profile = profiles->GetProfile(sensor.profile.type, sensor.profile.name);
                const auto& optionalLatencyParameters = openpass::parameter::Get<openpass::parameter::StochasticDistribution>(profile, "Latency");

                if (!optionalLatencyParameters.has_value())
                {
                    throw std::runtime_error("'Latency' parameter not defined");
                }

                double latency = Sampler::RollForStochasticAttribute(optionalLatencyParameters.value(), &stochastics);
                dynamicParameter.sensorLatencies.insert({sensor.id, latency});
            }
            catch (const std::runtime_error& error)
            {
                throw std::runtime_error("Could not sample sensor latencies for vehicle profile '" + vehicleProfileName + "': " + error.what());
            }
        }
    }

    return *this;
}

DynamicParameterSampler DynamicParameters::make(StochasticsInterface& stochastics,
                                                std::string& vehicleProfileName,
                                                ProfilesInterface* profiles)
{
    return DynamicParameterSampler(stochastics, vehicleProfileName, profiles);
}

DynamicParameters DynamicParameters::empty()
{
    return DynamicParameters();
}
