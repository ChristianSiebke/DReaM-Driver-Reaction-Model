/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

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

        if(CommonHelper::DoubleEquality(normalDistribution.GetMin(), normalDistribution.GetMax()))
        {
            return normalDistribution.GetMin();
        }

        int run = 0;
        double result = stochastics->GetNormalDistributed(normalDistribution.GetMean(), normalDistribution.GetStandardDeviation());

        while (result > normalDistribution.GetMax() || result < normalDistribution.GetMin())
        {
            run++;
            result = stochastics->GetNormalDistributed(normalDistribution.GetMean(), normalDistribution.GetStandardDeviation());
            if (run == MAX_RETRIES)
            {
                return normalDistribution.GetMean();
            }
        }
        return result;
    }
    else if (std::holds_alternative<LogNormalDistribution>(distribution))
    {
        auto logNormalDistribution = std::get<LogNormalDistribution>(distribution);

        if(CommonHelper::DoubleEquality(logNormalDistribution.GetMin(), logNormalDistribution.GetMax()))
        {
            return logNormalDistribution.GetMin();
        }

        int run = 0;
        double result = stochastics->GetLogNormalDistributedMuSigma(logNormalDistribution.GetMu(), logNormalDistribution.GetSigma());

        while (result > logNormalDistribution.GetMax() || result < logNormalDistribution.GetMin())
        {
            run++;
            result = stochastics->GetLogNormalDistributedMuSigma(logNormalDistribution.GetMu(), logNormalDistribution.GetSigma());
            if (run == MAX_RETRIES)
            {
                return 0.5 * (logNormalDistribution.GetMin() + logNormalDistribution.GetMax());
            }
        }
        return result;
    }
    else if (std::holds_alternative<UniformDistribution>(distribution))
    {
        auto uniformDistribution = std::get<UniformDistribution>(distribution);
        return stochastics->GetUniformDistributed(uniformDistribution.GetMin(), uniformDistribution.GetMax());
    }
    else if (std::holds_alternative<ExponentialDistribution>(distribution))
    {
        int run = 0;
        auto exponentialDistribution = std::get<ExponentialDistribution>(distribution);
        double result = stochastics->GetExponentialDistributed(exponentialDistribution.GetLambda());

        while (result > exponentialDistribution.GetMax() || result < exponentialDistribution.GetMin())
        {
            run++;
            result = stochastics->GetExponentialDistributed(exponentialDistribution.GetLambda());
            if (run == MAX_RETRIES)
            {
                return 1 / exponentialDistribution.GetLambda();
            }
        }
        return result;
    }
    else if (std::holds_alternative<GammaDistribution>(distribution))
    {
        int run = 0;
        auto gammaDistribution = std::get<GammaDistribution>(distribution);
        double result = stochastics->GetGammaDistributedShapeScale(gammaDistribution.GetShape(), gammaDistribution.GetScale());

        while (result > gammaDistribution.GetMax() || result < gammaDistribution.GetMin())
        {
            run++;

            if (run == MAX_RETRIES)
            {
                return gammaDistribution.GetMean();
            }

            result = stochastics->GetGammaDistributedShapeScale(gammaDistribution.GetShape(), gammaDistribution.GetScale());
        }
        return result;
    }
    else
    {
        throw std::runtime_error("Unsupported distribution type. Variant index: " + std::to_string(distribution.index()));
    }
}
