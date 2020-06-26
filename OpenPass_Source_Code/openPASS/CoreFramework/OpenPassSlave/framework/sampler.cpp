/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
* Copyright (c) 2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  Sampler.cpp */
//-----------------------------------------------------------------------------

#include "sampler.h"
#include <stdexcept>
#include <cmath>

bool Sampler::RollFor(double chance, StochasticsInterface* stochastics)
{
    if (chance == 0.0)
    {
        return false;
    }

    double roll = stochastics->GetUniformDistributed(0, 1);
    return (roll <= chance);
}

using namespace openpass::parameter;

constexpr int MAX_RETRIES{10};

double Sampler::RollForStochasticAttribute(const StochasticDistribution& distribution, StochasticsInterface* stochastics)
{
    if (std::holds_alternative<NormalDistribution>(distribution))
    {
        auto normalDistribution = std::get<NormalDistribution>(distribution);

        if(CommonHelper::DoubleEquality(normalDistribution.min, normalDistribution.max))
        {
            return normalDistribution.min;
        }

        int run = 0;
        double result = stochastics->GetNormalDistributed(normalDistribution.mean, normalDistribution.standardDeviation);

        while (result > normalDistribution.max || result < normalDistribution.min)
        {
            run++;
            result = stochastics->GetNormalDistributed(normalDistribution.mean, normalDistribution.standardDeviation);
            if (run == MAX_RETRIES)
            {
                return normalDistribution.mean;
            }
        }
        return result;
    }
    else if (std::holds_alternative<LogNormalDistribution>(distribution))
    {
        auto logNormalDistribution = std::get<LogNormalDistribution>(distribution);

        if(CommonHelper::DoubleEquality(logNormalDistribution.min, logNormalDistribution.max))
        {
            return logNormalDistribution.min;
        }

        int run = 0;
        double result = stochastics->GetMuSigmaLogNormalDistributed(logNormalDistribution.mu, logNormalDistribution.sigma);

        while (result > logNormalDistribution.max || result < logNormalDistribution.min)
        {
            run++;
            result = stochastics->GetMuSigmaLogNormalDistributed(logNormalDistribution.mu, logNormalDistribution.sigma);
            if (run == MAX_RETRIES)
            {
                return 0.5 * (logNormalDistribution.min + logNormalDistribution.max);
            }
        }
        return result;
    }
    else if (std::holds_alternative<UniformDistribution>(distribution))
    {
        auto uniformDistribution = std::get<UniformDistribution>(distribution);
        return stochastics->GetUniformDistributed(uniformDistribution.min, uniformDistribution.max);
    }
    else if (std::holds_alternative<ExponentialDistribution>(distribution))
    {
        int run = 0;
        auto exponentialDistribution = std::get<ExponentialDistribution>(distribution);
        double result = stochastics->GetExponentialDistributed(exponentialDistribution.lambda);

        while (result > exponentialDistribution.max || result < exponentialDistribution.min)
        {
            run++;
            result = stochastics->GetExponentialDistributed(exponentialDistribution.lambda);
            if (run == MAX_RETRIES)
            {
                return 1 / exponentialDistribution.lambda;
            }
        }
        return result;
    }
    else
    {
        throw std::runtime_error("Unsupported distribution type. Variant index: " + std::to_string(distribution.index()));
    }
}
