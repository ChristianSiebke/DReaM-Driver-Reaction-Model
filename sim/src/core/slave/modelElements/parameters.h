/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  Parameters.h
//! @brief This file contains the interface of the internal representation of
//!        configuration parameters.
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <map>

#include "common/opExport.h"
#include "include/parameterInterface.h"

namespace SimulationCommon {

class Parameters : public ParameterInterface
{
public:
    virtual ~Parameters() override = default;

    Parameters(const openpass::common::RuntimeInformation& runtimeInformation) :
        runtimeInformation(runtimeInformation)
    {}

    Parameters(const ParameterInterface& other) :
        runtimeInformation{other.GetRuntimeInformation()},
        parametersDouble{ other.GetParametersDouble()},
        parametersInt{other.GetParametersInt()},
        parametersBool{other.GetParametersBool()},
        parametersString{other.GetParametersString()},
        parametersDoubleVector{other.GetParametersDoubleVector()},
        parametersIntVector{other.GetParametersIntVector()},
        parametersBoolVector{other.GetParametersBoolVector()},
        parametersStringVector{other.GetParametersStringVector()},
        parametersStochastic{other.GetParametersStochastic()},
        parameterLists{other.GetParameterLists()} {}

    bool AddParameterDouble(std::string name, double value) override;
    bool AddParameterInt(std::string name, int value) override;
    bool AddParameterBool(std::string name, bool value) override;
    bool AddParameterString(std::string name, const std::string& value) override;
    bool AddParameterDoubleVector(std::string name, const std::vector<double> value) override;
    bool AddParameterIntVector(std::string name, const std::vector<int> value) override;
    bool AddParameterBoolVector(std::string name, const std::vector<bool> value) override;
    bool AddParameterStringVector(std::string name, const std::vector<std::string> value) override;
    bool AddParameterStochastic(std::string name, const openpass::parameter::StochasticDistribution value) override;

    ParameterInterface& InitializeListItem(std::string key) override;

    //NOTE: The primitive getters are in header on purpose

    virtual const openpass::common::RuntimeInformation& GetRuntimeInformation() const override
    {
        return runtimeInformation;
    }

    virtual const std::map<std::string, double>& GetParametersDouble() const override
    {
        return parametersDouble;
    }

    virtual const std::map<std::string, int>& GetParametersInt() const override
    {
        return parametersInt;
    }

    virtual const std::map<std::string, bool>& GetParametersBool() const override
    {
        return parametersBool;
    }

    virtual const std::map<std::string, const std::string>& GetParametersString() const override
    {
        return parametersString;
    }

    virtual const std::map<std::string, const std::vector<double>>& GetParametersDoubleVector() const override
    {
        return parametersDoubleVector;
    }

    virtual const std::map<std::string, const std::vector<int>>& GetParametersIntVector() const override
    {
        return parametersIntVector;
    }

    virtual const std::map<std::string, const std::vector<bool>>& GetParametersBoolVector() const override
    {
        return parametersBoolVector;
    }

    virtual const std::map<std::string, const std::vector<std::string>>& GetParametersStringVector() const override
    {
        return parametersStringVector;
    }

    virtual const std::map<std::string, const openpass::parameter::StochasticDistribution>& GetParametersStochastic() const override
    {
        return parametersStochastic;
    }

    virtual const std::map<std::string, ParameterLists>& GetParameterLists() const override
    {
        return parameterLists;
    }

protected:
    const openpass::common::RuntimeInformation& runtimeInformation;
    std::map<std::string, double> parametersDouble;
    std::map<std::string, int> parametersInt;
    std::map<std::string, bool> parametersBool;
    std::map<std::string, const std::string> parametersString;
    std::map<std::string, const std::vector<double>> parametersDoubleVector;
    std::map<std::string, const std::vector<int>> parametersIntVector;
    std::map<std::string, const std::vector<bool>> parametersBoolVector;
    std::map<std::string, const std::vector<std::string>> parametersStringVector;
    std::map<std::string, const openpass::parameter::StochasticDistribution> parametersStochastic;
    std::map<std::string, ParameterLists> parameterLists;
};

class ModelParameters : public Parameters
{
public:
    ModelParameters (const openpass::common::RuntimeInformation& runtimeInformation) :
        Parameters(runtimeInformation)
    {}
};

class SpawnPointParameters : public Parameters
{
public:
    SpawnPointParameters (const openpass::common::RuntimeInformation& runtimeInformation) :
        Parameters(runtimeInformation)
    {}
};

class ObservationParameters : public Parameters
{
public:
    ObservationParameters (const openpass::common::RuntimeInformation& runtimeInformation) :
        Parameters(runtimeInformation)
    {}
};

class WorldParameters : public Parameters
{
public:
    WorldParameters (const openpass::common::RuntimeInformation& runtimeInformation) :
        Parameters(runtimeInformation)
    {}
};

} // namespace SimulationCommon


