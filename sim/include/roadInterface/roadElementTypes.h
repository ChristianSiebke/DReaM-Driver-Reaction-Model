/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2016, 2017, 2018 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  RoadElementTypes.h
//! @brief This file contains enumerations for different types of roads.
//-----------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <array>

//-----------------------------------------------------------------------------
//! Road link connection orientation
//-----------------------------------------------------------------------------
enum class RoadLinkType
{
    Undefined = 0,
    Predecessor,
    Successor
};

//-----------------------------------------------------------------------------
//! Road link connection type
//-----------------------------------------------------------------------------
enum class RoadLinkElementType
{
    Undefined = 0,
    Road,
    Junction
};

//-----------------------------------------------------------------------------
//! Contact point of connections
//-----------------------------------------------------------------------------
enum class ContactPointType
{
    Undefined = 0,
    Start,
    End
};

//-----------------------------------------------------------------------------
//! Type of lane
//-----------------------------------------------------------------------------
enum class RoadLaneType // https://releases.asam.net/OpenDRIVE/1.6.0/ASAM_OpenDRIVE_BS_V1-6-0.html#_lanes  Section 9.5.3 Lane type
{
    Undefined = 0,
    Shoulder,
    Border,
    Driving,
    Stop,
    None,
    Restricted,
    Parking,
    Median,
    Biking,
    Sidewalk,
    Curb,
    Exit,
    Entry,
    OnRamp,
    OffRamp,
    ConnectingRamp
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 17> RoadLaneTypeMapping{
    "Undefined",
    "Shoulder",
    "Border",
    "Driving",
    "Stop",
    "None",
    "Restricted",
    "Parking",
    "Median",
    "Biking",
    "Sidewalk",
    "Curb",
    "Exit",
    "Entry",
    "OnRamp",
    "OffRamp",
    "ConnectingRamp"};

/// @brief Convert RoadLaneType to cstr (constexpr)
constexpr const char *to_cstr(RoadLaneType roadLaneType)
{
    return RoadLaneTypeMapping[static_cast<size_t>(roadLaneType)];
}

/// @brief Convert RoadLaneType to std::string
inline std::string to_string(RoadLaneType roadLaneType) noexcept
{
    return std::string(to_cstr(roadLaneType));
}

} // namespace utils

//-----------------------------------------------------------------------------
//! Type of lane line
//-----------------------------------------------------------------------------
enum class RoadLaneRoadMarkType // https://www.asam.net/standards/detail/opendrive/
{
    Undefined = 0,
    None,
    Solid,
    Broken,
    Solid_Solid,  // (for double solid line)
    Solid_Broken,  //(from inside to outside , exception: center lane  - from left to right)
    Broken_Solid, //(from inside to outside, exception: center lane - from left to right)
    Broken_Broken, // (from inside to outside, exception: center lane- from left to right)
    Botts_Dots,
    Grass, //(meaning a grass edge)
    Curb
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 11> RoadLaneRoadMarkTypeMapping{
    "Undefined",
    "None",
    "Solid",
    "Broken",
    "Solid_Solid",
    "Solid_Broken",
    "Broken_Solid",
    "Broken_Broken",
    "Botts_Dots",
    "Grass",
    "Curb"};

constexpr const char *to_cstr(RoadLaneRoadMarkType roadLaneRoadMarkType)
{
    return RoadLaneRoadMarkTypeMapping[static_cast<size_t>(roadLaneRoadMarkType)];
}

inline std::string to_string(RoadLaneRoadMarkType roadLaneRoadMarkType) noexcept
{
    return std::string(to_cstr(roadLaneRoadMarkType));
}

} // namespace utils

//-----------------------------------------------------------------------------
//! Lane description: left, right or center
//-----------------------------------------------------------------------------
enum class RoadLaneRoadDescriptionType // https://www.asam.net/standards/detail/opendrive/
{
    Left,
    Right,
    Center
};

//-----------------------------------------------------------------------------
//! LaneChange of the lane line
//-----------------------------------------------------------------------------
enum class RoadLaneRoadMarkLaneChange // https://www.asam.net/standards/detail/opendrive/
{
    Undefined = 0,
    None,
    Both,
    Increase,
    Decrease
};

//-----------------------------------------------------------------------------
//! Color of the road mark
//-----------------------------------------------------------------------------
enum class RoadLaneRoadMarkColor // https://www.asam.net/standards/detail/opendrive/
{
    Undefined = 0,
    Blue,
    Green,
    Red,
    Yellow,
    White,
    Orange
};

//! Weight of the road mark
enum class RoadLaneRoadMarkWeight // https://www.asam.net/standards/detail/opendrive/
{
    Undefined = 0,
    Standard,
    Bold
};

enum class RoadElementOrientation
{
    both,
    positive,
    negative,
};

//-----------------------------------------------------------------------------
//! Units used by signals
//-----------------------------------------------------------------------------
enum class RoadSignalUnit // https://www.asam.net/standards/detail/opendrive/
{
    Undefined = 0,
    Meter,
    Kilometer,
    Feet,
    LandMile,
    MetersPerSecond,
    MilesPerHour,
    KilometersPerHour,
    Kilogram,
    MetricTons,
    Percent
};

struct RoadElementValidity
{
    bool all{false};
    std::vector<int> lanes;
};

///
/// \brief The RoadSignalSpecification struct
///
/// Raw specification of the fields of a signal following
/// OpenDRIVEFormatSpecRev1.4H.pdf Section 5.3.9.1
///
struct RoadSignalSpecification
{
    double s{0};
    double t{0};
    std::string id{};
    std::string name{};
    std::string dynamic{};
    std::string orientation{};
    double zOffset{0};
    std::string country{};
    std::string type{};
    std::string subtype{};
    double value{0};
    RoadSignalUnit unit{};
    double height{0};
    double width{0};
    double length{0};
    std::string text{};
    double hOffset{0};
    double pitch{0};
    double roll{0};
    double yaw{0};

    RoadElementValidity validity;
    std::list<std::string> dependencyIds {};
};

enum class RoadObjectType
{
    none = -1,
    obstacle,
    car,
    pole,
    tree,
    vegetation,
    barrier,
    building,
    parkingSpace,
    patch,
    railing,
    trafficIsland,
    crosswalk,
    streetlamp,
    gantry,
    soundBarrier,
    van,
    bus,
    trailer,
    bike,
    motorbike,
    tram,
    train,
    pedestrian,
    wind,
    roadMark
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 26> RoadObjectTypeMapping{
    "None",
    "Obstacle",
    "Car",
    "Pole",
    "Tree",
    "Vegetation",
    "Barrier",
    "Building",
    "ParkingSpace",
    "Patch",
    "Railing",
    "TrafficIsland",
    "Crosswalk",
    "StreetLamp",
    "Gantry",
    "SoundBarrier",
    "Van",
    "Bus",
    "Trailer",
    "Bike",
    "Motorbike",
    "Tram",
    "Train",
    "Pedestrian",
    "Wind",
    "RoadMark"};

constexpr const char *to_cstr(RoadObjectType roadObjectType)
{
    return RoadObjectTypeMapping[static_cast<size_t>(roadObjectType) -
                                 static_cast<size_t>(RoadObjectType::none) ];
}

inline std::string to_string(RoadObjectType roadObjectType) noexcept
{
    return std::string(to_cstr(roadObjectType));
}

} // namespace utils

struct RoadObjectSpecification // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 65
{
    RoadObjectType type{RoadObjectType::none};
    std::string name {""};
    std::string id {""};
    double s {0};
    double t {0};
    double zOffset {0};
    double validLength {0};
    RoadElementOrientation orientation{RoadElementOrientation::positive};
    double width {0};
    double length {0};
    double radius {0};
    double height {0};
    double hdg {0};
    double pitch {0};
    double roll {0};

    RoadElementValidity validity;

    bool checkStandardCompliance()
    {
        return s >= 0 &&
               validLength >= 0 &&
               length >= 0 &&
               width >= 0 &&
               radius >= 0;
    }

    bool checkSimulatorCompliance()
    {
        return length > 0 && // force physical dimensions
               width > 0 &&  // force physical dimensions
               radius == 0;  // do not support radius
    }
};

struct OptionalInterval
{
    bool isSet = {false};
    double start;
    double end;
};

struct ObjectRepeat
{
    double s;
    double length;
    double distance;
    OptionalInterval t;
    OptionalInterval width;
    OptionalInterval height;
    OptionalInterval zOffset;

    bool checkLimits()
    {
        return s >= 0 &&
               length >= 0 &&
               distance >= 0;
    }
};

enum class RoadTypeInformation // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 92
{
    Undefined = 0,
    Unknown,
    Rural,
    Motorway,
    Town,
    LowSpeed,
    Pedestrian,
    Bicycle
};


struct RoadTypeSpecification
{
    double s{0};
    RoadTypeInformation roadType;
};




