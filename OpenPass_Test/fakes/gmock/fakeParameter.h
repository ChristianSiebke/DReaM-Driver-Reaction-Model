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
#include "Interfaces/parameterInterface.h"

class FakeParameter : public ParameterInterface
{
public:
    MOCK_METHOD2(AddParameterDouble,
                 bool(std::string, double));
    MOCK_METHOD2(AddParameterInt,
                 bool(std::string, int));
    MOCK_METHOD2(AddParameterBool,
                 bool(std::string, bool));
    MOCK_METHOD2(AddParameterString,
                 bool(std::string, const std::string&));
    MOCK_METHOD2(AddParameterDoubleVector,
                 bool(std::string, const std::vector<double>));
    MOCK_METHOD2(AddParameterIntVector,
                 bool(std::string, const std::vector<int>));
    MOCK_METHOD2(AddParameterBoolVector,
                 bool(std::string, const std::vector<bool>));
    MOCK_METHOD2(AddParameterStringVector,
                 bool(std::string, const std::vector<std::string>));
    MOCK_METHOD2(AddParameterNormalDistribution,
                 bool(std::string, const StochasticDefintions::NormalDistributionParameter));

    MOCK_CONST_METHOD0(GetParametersDouble,
                       const std::map<std::string, double>&());
    MOCK_CONST_METHOD0(GetParametersInt,
                       const std::map<std::string, int>&());
    MOCK_CONST_METHOD0(GetParametersBool,
                       std::map<std::string, bool>&());
    MOCK_CONST_METHOD0(GetParametersString,
                       const std::map<std::string, const std::string>&());
    MOCK_CONST_METHOD0(GetParametersDoubleVector,
                       const std::map<std::string, const std::vector<double>>&());
    MOCK_CONST_METHOD0(GetParametersIntVector,
                       const std::map<std::string, const std::vector<int>>&());
    MOCK_CONST_METHOD0(GetParametersBoolVector,
                       const std::map<std::string, const std::vector<bool>>&());
    MOCK_CONST_METHOD0(GetParametersStringVector,
                       const std::map<std::string, const std::vector<std::string>>&());
    MOCK_CONST_METHOD0(GetParametersNormalDistribution,
                       const std::map<std::string, const StochasticDefintions::NormalDistributionParameter>&());
    MOCK_CONST_METHOD0(GetParameterLists,
                       const std::map<std::string, ParameterLists>&());

    MOCK_METHOD1(InitializeListItem,
                 ParameterInterface&(std::string key));

    virtual ~FakeParameter() = default;
};
