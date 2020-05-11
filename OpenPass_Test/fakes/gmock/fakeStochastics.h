/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/stochasticsInterface.h"

class FakeStochastics : public StochasticsInterface
{
public:
    MOCK_METHOD2(GetUniformDistributed, double(double a, double b));
    MOCK_METHOD2(GetBinomialDistributed, int(int upperRangeNum, double probSuccess));
    MOCK_METHOD2(GetNormalDistributed, double(double mean, double stdDeviation));
    MOCK_METHOD1(GetExponentialDistributed, double(double lambda));
    MOCK_METHOD2(GetGammaDistributed, double(double mean, double stdDeviation));
    MOCK_METHOD2(GetLogNormalDistributed, double(double mu, double sigma));
    MOCK_METHOD2(GetSpecialDistributed, double(std::string distributionName, std::vector<double> args));
    MOCK_METHOD2(GetRandomCdfLogNormalDistributed, double(double, double));
    MOCK_METHOD3(GetPercentileLogNormalDistributed, double(double, double, double));
    MOCK_METHOD1(SampleFromDiscreteDistribution, double(const struct DiscreteDistributionInformation*));
    MOCK_METHOD3(ConditionOnRandomVariables, DiscreteDistributionInformation*(const struct DiscreteDistributionInformation*, std::vector<double>, int));
    MOCK_CONST_METHOD0(GetRandomSeed, std::uint32_t());
    MOCK_METHOD0(ReInit, void());
    MOCK_METHOD1(InitGenerator, void(std::uint32_t seed));
    MOCK_METHOD1(Instantiate, bool(std::string));
};



