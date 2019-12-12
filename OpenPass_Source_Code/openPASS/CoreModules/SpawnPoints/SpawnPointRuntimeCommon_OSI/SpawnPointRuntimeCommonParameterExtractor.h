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

#include "SpawnPointRuntimeCommonDefinitions.h"

#include "Common/commonTools.h"
#include "Interfaces/parameterInterface.h"
#include "Interfaces/samplerInterface.h"
#include "Common/SpawnPointDefinitions.h"

namespace
{
    using namespace SpawnPointRuntimeCommonDefinitions;
    static constexpr std::string_view ROAD_TAG{"Road"};
    static constexpr std::string_view LANES_TAG{"Lanes"};
    static constexpr std::string_view S_POSITION_TAG{"S-Position"};

    /*!
     * \brief AssertSpawnPointRuntimeCommonParametersProvided ensures all
     *        required parameters are provided; throws if not provided
     * \param roadId the roadId read from the parameters
     * \param laneIds the laneIds read from the parameters
     * \param sPosition the sPosition read from the parameters
     */
    static inline void AssertSpawnPointRuntimeCommonParametersProvided(const std::optional<RoadId>& roadId,
                                                                       const std::optional<LaneIds>& laneIds,
                                                                       const std::optional<SPosition>& sPosition)
    {
        if (!roadId
         || !laneIds
         || !sPosition)
        {
            std::string missingParameters;
            if (!roadId)
            {
                missingParameters += ROAD_TAG;
                missingParameters += ",";
            }
            if (!laneIds)
            {
                missingParameters += LANES_TAG;
                missingParameters += ",";
            }
            if (!sPosition)
            {
                missingParameters += S_POSITION_TAG;
            }
            if (missingParameters.back() == ',')
            {
                missingParameters.erase(std::cend(missingParameters) - 1,
                                        std::cend(missingParameters));
            }

            throw(std::runtime_error("Insufficient parameters supplied to SpawnPointRuntimeCommon: " + missingParameters));
        }
    }
}

namespace SpawnPointRuntimeCommonParameterExtractor
{
    using namespace SpawnPointRuntimeCommonDefinitions;

    /*!
     * \brief ExtractSpawnPointParameters extracts the parameters for the
     *        spawn point from the provided parameterInterface
     * \param parameterInterface the parameterInterface from which to extract
     * \param sampler the sampler with which to sample random values
     * \return the parameters for the spawn point
     */
    static SpawnPointRuntimeCommonParameters ExtractSpawnPointParameters(const ParameterInterface& parameter,
                                                                         const SamplerInterface * const sampler)
    {
        using namespace helper;

        const auto& stringParameters = parameter.GetParametersString();
        const auto& intVectorParameters = parameter.GetParametersIntVector();
        const auto& doubleParameters = parameter.GetParametersDouble();

        const auto roadIdElement = map::query(stringParameters, "Road");
        const auto laneIdsElement = map::query(intVectorParameters, "Lanes");
        const auto sPositionElement = map::query(doubleParameters, "S-Position");

        AssertSpawnPointRuntimeCommonParametersProvided(roadIdElement,
                                                        laneIdsElement,
                                                        sPositionElement);

        const auto trafficConfig = SpawnPointDefinitions::ConvertParametersIntoTrafficConfig(parameter);
        const auto sampledTrafficConfig = SpawnPointDefinitions::SampleTrafficConfig(sampler,
                                                                                     trafficConfig);

        return SpawnPointRuntimeCommonParameters(roadIdElement.value(),
                                                 laneIdsElement.value(),
                                                 sPositionElement.value(),
                                                 CommonHelper::PerHourToPerSecond(sampledTrafficConfig.trafficVolume),
                                                 sampledTrafficConfig.platoonRate,
                                                 sampledTrafficConfig.trafficVelocityDistribution,
                                                 trafficConfig.agentProfiles);
    }
};
