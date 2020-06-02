/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "SpawnPointPreRunCommonParameterExtractor.h"

SpawnPointParameters SpawnPointParameterExtractor::ExtractSpawnPointParameters(const ParameterInterface &parameter,
                                                                               const SamplerInterface * const sampler)
{
    using namespace helper;

    const auto& stringParameters = parameter.GetParametersString();
    const auto& intVectorParameters = parameter.GetParametersIntVector();
    const auto& doubleParameters = parameter.GetParametersDouble();

    auto roadIdElement = map::query(stringParameters, "Road");
    auto laneIdsElement = map::query(intVectorParameters, "Lanes");
    auto sStartElement = map::query(doubleParameters, "S-Start");
    auto sEndElement = map::query(doubleParameters, "S-End");

    if (!roadIdElement
     || !laneIdsElement
     || !sStartElement
     || !sEndElement)
    {
        std::string missingParameters;
        missingParameters += roadIdElement
                                ? ""
                                : "Road,";
        missingParameters += laneIdsElement
                                ? ""
                                : "Lanes,";
        missingParameters += sStartElement
                                ? ""
                                : "S-Start,";
        missingParameters += sEndElement
                                ? ""
                                : "S-End";
        if (missingParameters.back() == ',')
        {
            missingParameters.erase(std::end(missingParameters) - 1,
                                    std::end(missingParameters));
        }
        throw(std::runtime_error("Insufficient parameters supplied to SpawnPointPreRunCommon: " + missingParameters));
    }

    const auto trafficConfig = SpawnPointDefinitions::ConvertParametersIntoTrafficConfig(parameter);
    const auto sampledTrafficConfig = SpawnPointDefinitions::SampleTrafficConfig(sampler, trafficConfig);

    return SpawnPointParameters(roadIdElement.value(),
                                laneIdsElement.value(),
                                sStartElement.value(),
                                sEndElement.value(),
                                CommonHelper::PerHourToPerSecond(sampledTrafficConfig.trafficVolume),
                                sampledTrafficConfig.platoonRate,
                                sampledTrafficConfig.trafficVelocityDistribution,
                                trafficConfig.agentProfiles);
}

