/*******************************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_definitions.h
//! @brief This file contains several classes for global purposes
//-----------------------------------------------------------------------------

#pragma once

#include <list>
#include <map>
#include <string>
#include <tuple>
#include <vector>

//-----------------------------------------------------------------------------
//! Agent type classification
//-----------------------------------------------------------------------------
enum class AgentVehicleType
{
    NONE = -2,
    Undefined = -1,
    Car = 0,
    Pedestrian,
    Motorbike,
    Bicycle,
    Truck
};

static constexpr std::array<const char *, 5> AgentVehicleTypeMapping{
    "car",
    "pedestrian",
    "motorbike",
    "bicycle",
    "truck"};

//// convert a vehicle type name to VehicleType enum
inline AgentVehicleType GetAgentVehicleType(const std::string &strVehicleType)
{
    if (0 == strVehicleType.compare("car"))
    {
        return AgentVehicleType::Car;
    }
    else if (0 == strVehicleType.compare("pedestrian"))
    {
        return AgentVehicleType::Pedestrian;
    }
    else if (0 == strVehicleType.compare("motorbike"))
    {
        return AgentVehicleType::Motorbike;
    }
    else if (0 == strVehicleType.compare("bicycle"))
    {
        return AgentVehicleType::Bicycle;
    }
    else if (0 == strVehicleType.compare("truck"))
    {
        return AgentVehicleType::Truck;
    }
    return AgentVehicleType::Undefined;
}

//// convert a AgentVehicleType to VehicleType string
inline std::string GetAgentVehicleTypeStr(const AgentVehicleType &vehicleType)
{
    int vehicleTypeCode = static_cast<int>(vehicleType);
    std::string resultString;

    if (vehicleTypeCode < 0)
    {
        resultString = "unknown type";
    }
    else
    {
        resultString = AgentVehicleTypeMapping[static_cast<size_t>(vehicleType)];
    }

    return resultString;
}

//// convert a string of type code to VehicleType string
inline std::string GetAgentVehicleTypeStr(const int vehicleTypeCode)
{
    std::string resultString;
    switch (vehicleTypeCode)
    {
    case -2:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::NONE);
        break;
    case -1:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Undefined);
        break;
    case 0:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Car);
        break;
    case 1:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Pedestrian);
        break;
    case 2:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Motorbike);
        break;
    case 3:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Bicycle);
        break;
    case 4:
        resultString = GetAgentVehicleTypeStr(AgentVehicleType::Truck);
        break;
    default:
        resultString = "unknown type";
    }
    return resultString;
}

//! Enum of potential types of marks.
enum class MarkType
{
    NONE,
    CONTINUOUS,
    INTERRUPTED_LONG,
    INTERRUPTED_SHORT,
    ROADSIDE,
    NumberOfMarkTypes
};

//! Enum of potential types of objects.
enum class ObjectType
{
    NONE,
    OBJECT,
    VIEWOBJECT,
    NumberOfObjectTypes
};

