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

constexpr double PROBABILITY_SUM_TOLERANCE = 1e-5;

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

static void ParseProbabilities(const std::vector<std::shared_ptr<ParameterInterface>> &parameterLists,
                               const std::string &tag,
                               std::unordered_map<double, double> &result)
{
    try
    {
        double probabilitySum = 0.0;

        for(const auto& parameterListItem : parameterLists)
        {
            const auto& probability = parameterListItem->GetParametersDouble().at("Probability");
            probabilitySum += probability;

            const auto& value = parameterListItem->GetParametersDouble().at(tag);

            result.emplace(value, probability);
        }

        if(std::abs(probabilitySum - 1.0) > PROBABILITY_SUM_TOLERANCE)
        {
            LogErrorAndThrow(tag + " probability needs to add up to 1.0!");
        }
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "; SpawnPoint needs at least one " + tag + ".";
        LogErrorAndThrow(fullError);
    }
}

static void ParseProbabilities(const std::vector<std::shared_ptr<ParameterInterface>> &parameterLists,
                               const std::string &tag,
                               std::unordered_map<std::string, double> &result)
{
    try
    {
        double probabilitySum = 0.0;

        for(const auto& parameterListItem : parameterLists)
        {
            const auto& probability = parameterListItem->GetParametersDouble().at("Probability");
            probabilitySum += probability;

            const auto& value = parameterListItem->GetParametersString().at(tag);

            result.emplace(value, probability);
        }

        if(std::abs(probabilitySum - 1.0) > PROBABILITY_SUM_TOLERANCE)
        {
            LogErrorAndThrow(tag + " probability needs to add up to 1.0!");
        }
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one " + tag + ".";
        LogErrorAndThrow(fullError);
    }
}

static void ParseProbabilities(const std::vector<std::shared_ptr<ParameterInterface>> &parameterLists,
                               const std::string &tag,
                               std::unordered_map<openpass::parameter::NormalDistribution, double, hash_fn> &result)
{
    try
    {
        double probabilitySum = 0.0;

        for(const auto& parameterListItem : parameterLists)
        {
            const auto& value = parameterListItem->GetParametersNormalDistribution().at(tag);
            const auto& probability = parameterListItem->GetParametersDouble().at("Probability");
            probabilitySum += probability;

            result.emplace(value, probability);
        }

        if(std::abs(probabilitySum - 1.0) > PROBABILITY_SUM_TOLERANCE)
        {
            LogErrorAndThrow(tag + " probability needs to add up to 1.0!");
        }
    }
    catch (const std::out_of_range& error)
    {
        const std::string errorMessage = error.what();
        const std::string fullError = errorMessage + "SpawnPoint needs at least one " + tag + ".";
        LogErrorAndThrow(fullError);
    }
}

static TrafficConfig ConvertParametersIntoTrafficConfig(const ParameterInterface& parameter)
{
    try
    {
        TrafficConfig trafficConfig;

        ParseProbabilities(parameter.GetParameterLists().at("TrafficVolumes"),
                           "TrafficVolume",
                           trafficConfig.trafficVolumes);

        ParseProbabilities(parameter.GetParameterLists().at("PlatoonRates"),
                           "PlatoonRate",
                           trafficConfig.platoonRates);

        ParseProbabilities(parameter.GetParameterLists().at("AgentProfiles"),
                           "AgentProfile",
                           trafficConfig.agentProfiles);

        ParseProbabilities(parameter.GetParameterLists().at("Velocities"),
                           "Velocity",
                           trafficConfig.velocities);

        return trafficConfig;
    }
    catch (const std::out_of_range& error)
    {
        LogErrorAndThrow(error.what());
    }
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
