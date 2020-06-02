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
#include "Common/commonTools.h"
#include "Common/SpawnPointDefinitions.h"
#include "SpawnPointPreRunCommonDefinitions.h"

using namespace SpawnPointPreRunCommonDefinitions;

class SpawnPointParameterExtractor
{
public:
    static SpawnPointParameters ExtractSpawnPointParameters(const ParameterInterface& parameter,
                                                            const SamplerInterface * const sampler);

private:
    static inline double ConvertPerHourToPerSecond(const double perHourValue)
    {
        return perHourValue / 3600.0;
    }

};
