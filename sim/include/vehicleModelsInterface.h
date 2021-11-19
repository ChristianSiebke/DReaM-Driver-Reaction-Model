/********************************************************************************
 * Copyright (c) 2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  VehicleModelsInterface.h
//! @brief This file provides the interface of the vehicle model container.
//-----------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <algorithm>
#include <math.h>

#include "common/globalDefinitions.h"
#include "common/openScenarioDefinitions.h"

//! Resolves a parametrized attribute
//!
//! \param attribute                attribute is defined in the catalog
//! \param parameterAssignments     parameter assignments in the catalog reference
template <typename T>
T GetAttribute(openScenario::ParameterizedAttribute<T> attribute, const openScenario::Parameters& parameterAssignments)
{
    const auto& assignedParameter = parameterAssignments.find(attribute.name);
    if (assignedParameter != parameterAssignments.cend())
    {
        auto valueString = std::get<std::string>(assignedParameter->second);
        if constexpr (std::is_same_v<T, std::string>)
        {
            return valueString;
        }
        try
        {
            if constexpr (std::is_same_v<T, double>)
            {
                return std::stod(valueString);
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return std::stoi(valueString);
            }
        }
        catch (const std::invalid_argument&)
        {
            throw std::runtime_error("Type of assigned parameter \"" + attribute.name + "\" in scenario does not match.");
        }
        catch (const std::out_of_range&)
        {
            throw std::runtime_error("Value of assigned parameter \"" + attribute.name + "\" is out of range.");
        }
    }
    else
    {
        return attribute.defaultValue;
    }
}

//! Contains the VehicleModelParameters as defined in the VehicleModelCatalog.
//! Certain values may be parametrized and can be overwriten in the Scenario via ParameterAssignment
struct ParametrizedVehicleModelParameters
{
    AgentVehicleType vehicleType;

    struct BoundingBoxCenter
    {
        openScenario::ParameterizedAttribute<double> x;
        openScenario::ParameterizedAttribute<double> y;
        openScenario::ParameterizedAttribute<double> z;

        VehicleModelParameters::BoundingBoxCenter Get(const openScenario::Parameters& assignedParameters) const
        {
            return {GetAttribute(x, assignedParameters),
                    GetAttribute(y, assignedParameters),
                    GetAttribute(z, assignedParameters)};
        }
    } boundingBoxCenter;

    struct BoundingBoxDimensions
    {
        openScenario::ParameterizedAttribute<double> width;
        openScenario::ParameterizedAttribute<double> length;
        openScenario::ParameterizedAttribute<double> height;

        VehicleModelParameters::BoundingBoxDimensions Get(const openScenario::Parameters& assignedParameters) const
        {
            return {GetAttribute(width, assignedParameters),
                    GetAttribute(length, assignedParameters),
                    GetAttribute(height, assignedParameters)};
        }
    } boundingBoxDimensions;

    struct Performance
    {
        openScenario::ParameterizedAttribute<double> maxSpeed;
        openScenario::ParameterizedAttribute<double> maxAcceleration;
        openScenario::ParameterizedAttribute<double> maxDeceleration;

        VehicleModelParameters::Performance Get(const openScenario::Parameters& assignedParameters) const
        {
            return {GetAttribute(maxSpeed, assignedParameters),
                    GetAttribute(maxAcceleration, assignedParameters),
                    GetAttribute(maxDeceleration, assignedParameters)};
        }
    } performance;

    struct Axle
    {
        openScenario::ParameterizedAttribute<double> maxSteering;
        openScenario::ParameterizedAttribute<double> wheelDiameter;
        openScenario::ParameterizedAttribute<double> trackWidth;
        openScenario::ParameterizedAttribute<double> positionX;
        openScenario::ParameterizedAttribute<double> positionZ;

        VehicleModelParameters::Axle Get(const openScenario::Parameters& assignedParameters) const
        {
            return {GetAttribute(maxSteering, assignedParameters),
                    GetAttribute(wheelDiameter, assignedParameters),
                    GetAttribute(trackWidth, assignedParameters),
                    GetAttribute(positionX, assignedParameters),
                    GetAttribute(positionZ, assignedParameters)};
        }
    };
    Axle frontAxle;
    Axle rearAxle;

    std::map<std::string, openScenario::ParameterizedAttribute<double>> properties;

    VehicleModelParameters Get(const openScenario::Parameters& assignedParameters) const
    {
        std::map<std::string, double> assignedProperties;
        for (const auto[key, value] : properties)
        {
            assignedProperties.insert({key, GetAttribute(value, assignedParameters)});
        }

        return {vehicleType,
                boundingBoxCenter.Get(assignedParameters),
                boundingBoxDimensions.Get(assignedParameters),
                performance.Get(assignedParameters),
                frontAxle.Get(assignedParameters),
                rearAxle.Get(assignedParameters),
                assignedProperties
        };
    }
};

using VehicleModelMap = std::unordered_map<std::string, ParametrizedVehicleModelParameters>;

class VehicleModelsInterface
{
public:
    VehicleModelsInterface() = default;
    ~VehicleModelsInterface() = default;

    virtual VehicleModelMap& GetVehicleModelMap() = 0;

    //! Returns the VehicleModel with the specified name
    //!
    //! \param vehicleModelType     name of the vehicle model
    //! \param parameters           assigned parameters, that overwrite the default parameters
    virtual VehicleModelParameters GetVehicleModel(std::string vehicleModelType, const openScenario::Parameters& parameters = {}) = 0;
};
