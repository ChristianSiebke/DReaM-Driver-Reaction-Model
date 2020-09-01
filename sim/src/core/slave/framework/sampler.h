/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2018 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Sampler.h
//! @brief This file contains the Sampler, which samples parameters and
//!  probabilities.
//-----------------------------------------------------------------------------

#pragma once

#include "include/stochasticsInterface.h"
#include "include/slaveConfigInterface.h"
#include "common/runtimeInformation.h"
#include <numeric>
#include <algorithm>

//-----------------------------------------------------------------------------
/** \brief Sampler samples probabilities and SpawnPoint and World parameters.
*   \details This class contains all stochastics functions of the SpawnPoint and the world
*               It samples parameters according to probabilities specified in the configurations
*
*/
//-----------------------------------------------------------------------------
namespace Sampler
{
    bool RollFor(double chance, StochasticsInterface* stochastics);
    double RollForStochasticAttribute(const openpass::parameter::StochasticDistribution& distribution, StochasticsInterface* stochastics);

    template <typename T>
    T Sample(std::vector<std::pair<T, double>> weightedValues, StochasticsInterface* stochastics)
    {
        double sumOfWeights{0.0};
        for (const auto& [value, weight] : weightedValues)
        {
            sumOfWeights += weight;
        }
        auto roll = stochastics->GetUniformDistributed(0, sumOfWeights);
        for (const auto& [value, weight] : weightedValues)
        {
            roll -= weight;
            if (roll <= 0)
            {
                return value;
            }
        }
        throw std::runtime_error("Invalid roll in Sampler");
    }

};
