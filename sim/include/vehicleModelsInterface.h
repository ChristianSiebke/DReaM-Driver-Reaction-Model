/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

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

/*!
 * \brief Container for axle parameters in OpenSCENARIO vehicle model
 */
struct VehicleAxle
{
    openScenario::ParameterizedAttribute<double> maxSteering = -999.0;     //!< Maximum steering angle
    openScenario::ParameterizedAttribute<double> wheelDiameter = -999.0;   //!< Diameter of the wheels
    openScenario::ParameterizedAttribute<double> trackWidth = -999.0;      //!< Trackwidth of the axle
    openScenario::ParameterizedAttribute<double> positionX = -999.0;       //!< Longitudinal position offset (measured from reference point)
    openScenario::ParameterizedAttribute<double> positionZ = -999.0;       //!< Vertical position offset (measured from reference point)
};

//! Contains the VehicleModelParameters as defined in the VehicleModelCatalog.
//! Certain values may be parametrized and can be overwriten in the Scenario via ParameterAssignment
struct ParametrizedVehicleModelParameters
{
    AgentVehicleType vehicleType = AgentVehicleType::Undefined;
    openScenario::ParameterizedAttribute<double> width = -999.0;
    openScenario::ParameterizedAttribute<double> length = -999.0;
    openScenario::ParameterizedAttribute<double> height = -999.0;
    VehicleAxle frontAxle{};
    VehicleAxle rearAxle{};
    openScenario::ParameterizedAttribute<double> distanceReferencePointToLeadingEdge = -999.0;
    openScenario::ParameterizedAttribute<double> maxVelocity = -999.0;
    openScenario::ParameterizedAttribute<double> weight = -999.0;
    openScenario::ParameterizedAttribute<double> heightCOG = -999.0;
    openScenario::ParameterizedAttribute<double> momentInertiaRoll = -999.0;
    openScenario::ParameterizedAttribute<double> momentInertiaPitch = -999.0;
    openScenario::ParameterizedAttribute<double> momentInertiaYaw = -999.0;
    openScenario::ParameterizedAttribute<double> frontSurface = -999.0;
    openScenario::ParameterizedAttribute<double> airDragCoefficient = -999.0;
    openScenario::ParameterizedAttribute<double> minimumEngineSpeed = -999.0;
    openScenario::ParameterizedAttribute<double> maximumEngineSpeed = -999.0;
    openScenario::ParameterizedAttribute<double> minimumEngineTorque = -999.0;
    openScenario::ParameterizedAttribute<double> maximumEngineTorque = -999.0;
    openScenario::ParameterizedAttribute<int> numberOfGears = -999;
    std::vector<openScenario::ParameterizedAttribute<double>> gearRatios;
    openScenario::ParameterizedAttribute<double> axleRatio = -999.0;
    openScenario::ParameterizedAttribute<double> decelerationFromPowertrainDrag = -999.0;
    openScenario::ParameterizedAttribute<double> steeringRatio = -999.0;
    openScenario::ParameterizedAttribute<double> frictionCoeff = -999.0;

    VehicleModelParameters Get(const openScenario::Parameters& assignedParameters) const
    {
        auto wheelbase = std::abs(frontAxle.positionX.defaultValue - rearAxle.positionX.defaultValue);
        std::vector<double> transformedGearRatios;
        std::transform(gearRatios.cbegin(), gearRatios.cend(), std::back_inserter(transformedGearRatios), [&](const auto& value){return GetAttribute(value, assignedParameters);});
        return VehicleModelParameters{
                    vehicleType,
                    GetAttribute(width, assignedParameters),
                    GetAttribute(length, assignedParameters),
                    GetAttribute(height, assignedParameters),
                    wheelbase,
                    GetAttribute(rearAxle.trackWidth, assignedParameters),
                    GetAttribute(distanceReferencePointToLeadingEdge, assignedParameters),
                    GetAttribute(frontAxle.positionX, assignedParameters),
                    GetAttribute(maxVelocity, assignedParameters),
                    GetAttribute(weight, assignedParameters),
                    GetAttribute(heightCOG, assignedParameters),
                    GetAttribute(momentInertiaRoll, assignedParameters),
                    GetAttribute(momentInertiaPitch, assignedParameters),
                    GetAttribute(momentInertiaYaw, assignedParameters),
                    GetAttribute(frontSurface, assignedParameters),
                    GetAttribute(airDragCoefficient, assignedParameters),
                    GetAttribute(minimumEngineSpeed, assignedParameters),
                    GetAttribute(maximumEngineSpeed, assignedParameters),
                    GetAttribute(minimumEngineTorque, assignedParameters),
                    GetAttribute(maximumEngineTorque, assignedParameters),
                    GetAttribute(numberOfGears, assignedParameters),
                    transformedGearRatios,
                    GetAttribute(axleRatio, assignedParameters),
                    GetAttribute(decelerationFromPowertrainDrag, assignedParameters),
                    GetAttribute(steeringRatio, assignedParameters),
                    GetAttribute(frontAxle.maxSteering, assignedParameters) * GetAttribute(steeringRatio, assignedParameters),
                    std::sin(GetAttribute(frontAxle.maxSteering, assignedParameters)) / wheelbase,
                    rearAxle.wheelDiameter.defaultValue / 2.0,
                    GetAttribute(frictionCoeff, assignedParameters)
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
