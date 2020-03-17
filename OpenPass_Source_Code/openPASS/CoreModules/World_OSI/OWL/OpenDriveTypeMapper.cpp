/*******************************************************************************
* Copyright (c) 2018, 2019 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  OpenDriveTypeMapper.cpp
//! @brief This file provides conversions from OpenDRIVE constants to OSI
//!        constants
//-----------------------------------------------------------------------------


#include "OpenDriveTypeMapper.h"

#include "Interfaces/roadInterface/roadElementTypes.h"

#include "osi3/osi_road.pb.h"
#include "osi3/osi_lane.pb.h"
#include "OWL/DataTypes.h"

osi3::world::Road_Type OpenDriveTypeMapper::OdToOsiRoadType(const RoadTypeInformation odRoadType)
{
    osi3::world::Road_Type osiType;

    switch (odRoadType)
    {
        case RoadTypeInformation::Bicycle:
            osiType = osi3::world::Road_Type_TYPE_BIKEWAY;
            break;

        case RoadTypeInformation::LowSpeed:
            osiType = osi3::world::Road_Type_TYPE_LOWSPEED;
            break;

        case RoadTypeInformation::Motorway:
            osiType = osi3::world::Road_Type_TYPE_HIGHWAY;
            break;

        case RoadTypeInformation::Pedestrian:
            osiType = osi3::world::Road_Type_TYPE_PEDESTRIAN;
            break;

        case RoadTypeInformation::Rural:
            osiType = osi3::world::Road_Type_TYPE_RURAL;
            break;

        case RoadTypeInformation::Town:
            osiType = osi3::world::Road_Type_TYPE_TOWN;
            break;

        default:
            osiType = osi3::world::Road_Type_TYPE_HIGHWAY;
            break;
    }

    return osiType;
}

osi3::Lane_Classification_Type OpenDriveTypeMapper::OdToOsiLaneType(const RoadLaneType odLaneType)
{
    osi3::Lane_Classification_Type osiType;

    switch (odLaneType)
    {
        case RoadLaneType::Undefined:
            osiType = osi3::Lane_Classification_Type_TYPE_UNKNOWN;
            break;
        case RoadLaneType::Driving:
            osiType = osi3::Lane_Classification_Type_TYPE_DRIVING;
            break;
        default:
            osiType = osi3::Lane_Classification_Type_TYPE_OTHER;
            break;
    }
    return osiType;
}

static osi3::TrafficSignValue_Unit TrafficSignUnit(const RoadSignalUnit unit)
{
    switch(unit)
    {
        case RoadSignalUnit::Undefined:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_OTHER;
         case RoadSignalUnit::Meter:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER;
         case RoadSignalUnit::Kilometer:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER;
         case RoadSignalUnit::Feet:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_FEET;
         case RoadSignalUnit::LandMile:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE;
         case RoadSignalUnit::MilesPerHour:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE_PER_HOUR;
         case RoadSignalUnit::KilometersPerHour:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR;
         case RoadSignalUnit::MetricTons:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METRIC_TON;
         case RoadSignalUnit::Percent:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_PERCENTAGE;
         default:
            return osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_UNKNOWN;
    }
}

static std::pair<double, osi3::TrafficSignValue_Unit> ConvertOdValueAndUnitToOsi(const double odValue, const RoadSignalUnit odUnit)
{
    double osiValue = odValue;
    osi3::TrafficSignValue_Unit osiUnit = TrafficSignUnit(odUnit);

    if(osiUnit == osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_UNKNOWN)
    {
        if(odUnit == RoadSignalUnit::MetersPerSecond)
        {
            osiUnit = osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR;
            osiValue = odValue * 3.6;
        }
        else if(odUnit == RoadSignalUnit::Kilogram)
        {
            osiUnit = osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METRIC_TON;
            osiValue = odValue / 1000.0;
        }
    }

    return {osiValue, osiUnit};
}


osi3::LaneBoundary::Classification::Type OpenDriveTypeMapper::OdToOsiLaneMarkingType(RoadLaneRoadMarkType type, OWL::LaneMarkingSide side)
{
    switch (type)
    {
    case  RoadLaneRoadMarkType::None:
        return osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE;
    case  RoadLaneRoadMarkType::Solid:
        return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
    case  RoadLaneRoadMarkType::Broken:
        return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
    case  RoadLaneRoadMarkType::Solid_Solid:
        return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
    case  RoadLaneRoadMarkType::Solid_Broken:
        if (side == OWL::LaneMarkingSide::Right)
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
        }
        else
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
        }
    case  RoadLaneRoadMarkType::Broken_Solid:
        if (side == OWL::LaneMarkingSide::Right)
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE;
        }
        else
        {
            return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
        }
    case  RoadLaneRoadMarkType::Broken_Broken:
        return osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE;
    case  RoadLaneRoadMarkType::Botts_Dots:
        return osi3::LaneBoundary_Classification_Type_TYPE_BOTTS_DOTS;
    case  RoadLaneRoadMarkType::Grass:
        return osi3::LaneBoundary_Classification_Type_TYPE_GRASS_EDGE;
    case  RoadLaneRoadMarkType::Curb:
        return osi3::LaneBoundary_Classification_Type_TYPE_CURB;
    case RoadLaneRoadMarkType::Undefined:
        return osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE;
    default:
        throw std::invalid_argument("Invalid type");
    }
}

osi3::LaneBoundary::Classification::Color OpenDriveTypeMapper::OdToOsiLaneMarkingColor(RoadLaneRoadMarkColor color)
{
    switch (color)
    {
        case RoadLaneRoadMarkColor::Undefined:
            return osi3::LaneBoundary_Classification_Color_COLOR_OTHER;
        case RoadLaneRoadMarkColor::White:
            return osi3::LaneBoundary_Classification_Color_COLOR_WHITE;
        case RoadLaneRoadMarkColor::Blue:
            return osi3::LaneBoundary_Classification_Color_COLOR_BLUE;
        case RoadLaneRoadMarkColor::Green:
            return osi3::LaneBoundary_Classification_Color_COLOR_GREEN;
        case RoadLaneRoadMarkColor::Red:
            return osi3::LaneBoundary_Classification_Color_COLOR_RED;
        case RoadLaneRoadMarkColor::Yellow:
        case RoadLaneRoadMarkColor::Orange:
            return osi3::LaneBoundary_Classification_Color_COLOR_YELLOW;
        default:
            throw std::invalid_argument("Invalid color");
    }
}

LaneType OpenDriveTypeMapper::OdToOwlLaneType(const RoadLaneType laneType)
{
    switch (laneType)
    {
        case RoadLaneType::None:
            return LaneType::None;
        case RoadLaneType::Driving:
            return LaneType::Driving;
        case RoadLaneType::Stop:
            return LaneType::Stop;
        case RoadLaneType::Shoulder:
            return LaneType::Shoulder;
        case RoadLaneType::Biking:
            return LaneType::Biking;
        case RoadLaneType::Sidewalk:
            return LaneType::Sidewalk;
        case RoadLaneType::Border:
            return LaneType::Border;
        case RoadLaneType::Restricted:
            return LaneType::Restricted;
        case RoadLaneType::Parking:
            return LaneType::Parking;
        case RoadLaneType::Bidirectional:
            return LaneType::Bidirectional;
        case RoadLaneType::Median:
            return LaneType::Median;
        case RoadLaneType::Special1:
            return LaneType::Special1;
        case RoadLaneType::Special2:
            return LaneType::Special2;
        case RoadLaneType::Special3:
            return LaneType::Special3;
        case RoadLaneType::Roadworks:
            return LaneType::Roadworks;
        case RoadLaneType::Tram:
            return LaneType::Tram;
        case RoadLaneType::Rail:
            return LaneType::Rail;
        case RoadLaneType::Entry:
            return LaneType::Entry;
        case RoadLaneType::Exit:
            return LaneType::Exit;
        case RoadLaneType::OffRamp:
            return LaneType::OffRamp;
        case RoadLaneType::OnRamp:
            return LaneType::OnRamp;
        default:
            return LaneType::Undefined;
    }
}

LaneMarking::Color OpenDriveTypeMapper::OsiToOdLaneMarkingColor(const osi3::LaneBoundary_Classification_Color color)
{
    if(color == osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_WHITE)
    {
        return LaneMarking::Color::White;
    }
    if(color == osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_YELLOW)
    {
        return LaneMarking::Color::Yellow;
    }
    if(color == osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_GREEN)
    {
        return LaneMarking::Color::Green;
    }
    if(color == osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_RED)
    {
        return LaneMarking::Color::Red;
    }
    if(color == osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_BLUE)
    {
        return LaneMarking::Color::Blue;
    }

    throw std::invalid_argument("Type of lane marking color not supported.");
}

LaneMarking::Type OpenDriveTypeMapper::OsiToOdLaneMarkingType(const osi3::LaneBoundary_Classification_Type type)
{
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_NO_LINE)
    {
        return LaneMarking::Type::None;
    }
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_SOLID_LINE)
    {
        return LaneMarking::Type::Solid;
    }
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_DASHED_LINE)
    {
        return LaneMarking::Type::Broken;
    }
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_BOTTS_DOTS)
    {
        return LaneMarking::Type::Botts_Dots;
    }
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_GRASS_EDGE)
    {
        return LaneMarking::Type::Grass;
    }
    if (type == osi3::LaneBoundary_Classification_Type_TYPE_CURB)
    {
        return LaneMarking::Type::Curb;
    }

    throw std::invalid_argument("Type of lane marking not supported.");
}
