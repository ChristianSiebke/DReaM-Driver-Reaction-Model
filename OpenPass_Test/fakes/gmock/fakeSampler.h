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
#include "Interfaces/samplerInterface.h"

class FakeSampler : public SamplerInterface
{
public:
    MOCK_CONST_METHOD1(RollFor,
                       bool(double));
    MOCK_CONST_METHOD1(RollGapBetweenCars,
                       double(double));
    MOCK_CONST_METHOD1(RollGapExtension,
                       double(double));
    MOCK_CONST_METHOD2(RollForVelocity,
                       double(double, double));
    MOCK_CONST_METHOD4(RollForStochasticAttribute,
                       double(double, double, double, double));
    MOCK_CONST_METHOD1(RollUniformDistributedVectorIndex,
                       size_t(size_t));
    MOCK_CONST_METHOD1(SampleStringProbability,
                       std::string(StringProbabilities));
    MOCK_CONST_METHOD1(SampleDoubleProbability,
                       double(DoubleProbabilities));
    MOCK_CONST_METHOD1(SampleIntProbability,
                       int(IntProbabilities));
    MOCK_CONST_METHOD1(SampleWorldParameters,
                       std::unique_ptr<ParameterInterface>(const EnvironmentConfig&));
    MOCK_CONST_METHOD1(SampleSpawnPointParameters,
                       std::unique_ptr<ParameterInterface>(const TrafficConfig&));
};


