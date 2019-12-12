/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "Interfaces/parameterInterface.h"
#include "Interfaces/samplerInterface.h"
#include "Interfaces/slaveConfigInterface.h"
#include "CoreFramework/CoreShare/log.h"
#include "Common/spawnPointLibraryDefinitions.h"

namespace SpawnPointDefinitions
{

using RoadId = std::string;
using LaneId = int;
using LaneIds = std::vector<LaneId>;
using SPosition = double;
using Range = std::pair<SPosition, SPosition>;
using ValidLaneSpawningRanges = std::vector<Range>;
using StringProbabilities = std::unordered_map<std::string, double>;
using VehicleRearAndFrontCoordinates = std::pair<SPosition, SPosition>;

enum class Direction
{
    FORWARD = 0,
    BACKWARD
};

struct TrafficConfig
{
    DoubleProbabilities trafficVolumes{};
    DoubleProbabilities platoonRates{};
    NormalDistributionProbabilities velocities{};
    StringProbabilities agentProfiles {};
};

static TrafficConfig ConvertParametersIntoTrafficConfig(const ParameterInterface& parameter)
{
    TrafficConfig trafficConfig;

    try
    {
        const auto& trafficVolumesParameterListItems = parameter.GetParameterLists().at("TrafficVolumes");
        std::transform(std::begin(trafficVolumesParameterListItems),
                       std::end(trafficVolumesParameterListItems),
                       std::inserter(trafficConfig.trafficVolumes, std::end(trafficConfig.trafficVolumes)),
                       [](const auto& parameterListItem) -> std::pair<double, double>
        {
            const auto& parametersDouble = parameterListItem->GetParametersDouble();
            const auto& trafficVolume = parametersDouble.at("TrafficVolume");
            const auto& probability = parametersDouble.at("Probability");

            return std::make_pair(trafficVolume, probability);
        });
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one traffic volume.";
        LOG_INTERN(LogLevel::Error) << fullError;
    }

    try
    {
        const auto& platoonRatesParameterListItems = parameter.GetParameterLists().at("PlatoonRates");
        std::transform(std::begin(platoonRatesParameterListItems),
                       std::end(platoonRatesParameterListItems),
                       std::inserter(trafficConfig.platoonRates, std::end(trafficConfig.platoonRates)),
                       [](const auto& parameterListItem) -> std::pair<double, double>
        {
           const auto& parametersDouble = parameterListItem->GetParametersDouble();
           const auto& platoonRate = parametersDouble.at("PlatoonRate");
           const auto& probability = parametersDouble.at("Probability");

           return std::make_pair(platoonRate, probability);
        });
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one platoon rate.";
        LOG_INTERN(LogLevel::Error) << fullError;
    }

    try
    {
        const auto& velocitiesParameterListItems = parameter.GetParameterLists().at("Velocities");
        std::transform(std::begin(velocitiesParameterListItems),
                       std::end(velocitiesParameterListItems),
                       std::inserter(trafficConfig.velocities, std::end(trafficConfig.velocities)),
                       [](const auto& parameterListItem) -> std::pair<openpass::parameter::NormalDistribution, double>
        {
           const auto& velocity = parameterListItem->GetParametersNormalDistribution().at("Velocity");
           const auto& probability = parameterListItem->GetParametersDouble().at("Probability");

           return std::make_pair(velocity, probability);
        });
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one velocity.";
        LOG_INTERN(LogLevel::Error) << fullError;
    }

    try
    {
        const auto& agentProfilesListItems = parameter.GetParameterLists().at("AgentProfiles");
        std::transform(std::begin(agentProfilesListItems),
                       std::end(agentProfilesListItems),
                       std::inserter(trafficConfig.agentProfiles, std::end(trafficConfig.agentProfiles)),
                       [](const auto& parameterListItem) -> std::pair<std::string, double>
        {
            const auto& agentProfile = parameterListItem->GetParametersString().at("AgentProfile");
            const auto& probability = parameterListItem->GetParametersDouble().at("Probability");

            return std::make_pair(agentProfile, probability);
        });
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one agent profile.";
        LOG_INTERN(LogLevel::Error) << fullError;
    }

    return trafficConfig;
}

struct SampledTrafficConfig
{
    double trafficVolume{};
    double platoonRate{};
    openpass::parameter::NormalDistribution trafficVelocityDistribution {};
};

static SampledTrafficConfig SampleTrafficConfig(const SamplerInterface * const sampler,
                                                const SpawnPointDefinitions::TrafficConfig& trafficConfig)
{
    SampledTrafficConfig trafficConfigParameters;

    const auto trafficVolumes = sampler->SampleDoubleProbability(trafficConfig.trafficVolumes);
    trafficConfigParameters.trafficVolume = trafficVolumes;

    const auto platoonRates = sampler->SampleDoubleProbability(trafficConfig.platoonRates);
    trafficConfigParameters.platoonRate = platoonRates;

    const auto velocities = sampler->SampleNormalDistributionProbability(trafficConfig.velocities);
    trafficConfigParameters.trafficVelocityDistribution = velocities;

    return trafficConfigParameters;
}
} // namespace SpawnPointDefinitions
