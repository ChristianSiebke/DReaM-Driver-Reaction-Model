/*******************************************************************************
 * Copyright (c) 2017, 2018, 2019 in-tech GmbH
 *               2018 AMFD GmbH
 *               2016, 2017, 2018 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ParameterInterface.h
//! @brief This file contains the interface to retrieve the parameters.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Common/runtimeInformation.h"
#include "Common/stochasticDefinitions.h"

//-----------------------------------------------------------------------------
//! Interface provides access to the configuration parameters
//-----------------------------------------------------------------------------
class ParameterInterface
{
public:
    using ParameterLists = std::vector<std::shared_ptr<ParameterInterface>>;

    virtual ~ParameterInterface() = default;

    //-----------------------------------------------------------------------------
    //! Initializes a new parameter list item
    //!
    //! @return                Pointer to the list item
    //-----------------------------------------------------------------------------
    virtual ParameterInterface &InitializeListItem(std::string key) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a double to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterDouble(std::string name, double value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a int to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterInt(std::string name, int value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a bool to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterBool(std::string name, bool value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a string to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterString(std::string name, const std::string& value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a string vector to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterStringVector(std::string name, const std::vector<std::string> value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a double vector to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterDoubleVector(std::string name, const std::vector<double> value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a int vector to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterIntVector(std::string name, const std::vector<int> value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a bool vector to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterBoolVector(std::string name, const std::vector<bool> value) = 0;

    //-----------------------------------------------------------------------------
    //! Adds a normal distribution to the parameters
    //!
    //! @return                true, if value was not already in the map
    //-----------------------------------------------------------------------------
    virtual bool AddParameterNormalDistribution(std::string name,
                                                const openpass::parameter::NormalDistribution value) = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the runetime information from the interface
    //!
    //! @return                RuntimeInformation
    //-----------------------------------------------------------------------------
    virtual const openpass::common::RuntimeInformation& GetRuntimeInformation() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "Double"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, double>& GetParametersDouble() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "Int"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, int>& GetParametersInt() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "Bool"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, bool>& GetParametersBool() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "String"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const std::string>& GetParametersString() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "DoubleVector"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const std::vector<double>>& GetParametersDoubleVector() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "IntVector"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const std::vector<int>>& GetParametersIntVector() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "BoolVector"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const std::vector<bool>>& GetParametersBoolVector() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "StringVector"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const std::vector<std::string>>& GetParametersStringVector() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameters of type "NormalDistribution"
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, const openpass::parameter::NormalDistribution>& GetParametersNormalDistribution() const = 0;

    //-----------------------------------------------------------------------------
    //! Retrieves the parameter lists.
    //!
    //! @return                Mapping of "id" to "value"
    //-----------------------------------------------------------------------------
    virtual const std::map<std::string, ParameterLists>& GetParameterLists() const = 0;
};
