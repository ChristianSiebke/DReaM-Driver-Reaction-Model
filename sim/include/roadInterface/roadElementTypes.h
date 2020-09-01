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
//! @file  RoadElementTypes.h
//! @brief This file contains enumerations for different types of roads.
//-----------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <string>
#include <list>
#include <vector>

//-----------------------------------------------------------------------------
//! Road link connection orientation
//-----------------------------------------------------------------------------
enum class RoadLinkType
{
    Undefined = 0,
    Predecessor,
    Successor,
    Neighbor
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
//! Orientation between roads
//-----------------------------------------------------------------------------
enum class RoadLinkDirectionType
{
    Undefined = 0,
    Same,
    Opposite
};

//-----------------------------------------------------------------------------
//! Connection between roads
//-----------------------------------------------------------------------------
enum class RoadLinkSideType
{
    Undefined = 0,
    Left,
    Right
};

//-----------------------------------------------------------------------------
//! Type of lane
//-----------------------------------------------------------------------------
enum class RoadLaneType // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 93
{
    Undefined = 0,
    None,
    Driving,
    Stop,
    Shoulder,
    Biking,
    Sidewalk,
    Border,
    Restricted,
    Parking,
    Bidirectional, // full name: continuous two-way left turn lane
    Median,
    Special1,
    Special2,
    Special3,
    Roadworks,
    Tram,
    Rail,
    Entry,
    Exit,
    OffRamp,
    OnRamp
};

//-----------------------------------------------------------------------------
//! Type of lane line
//-----------------------------------------------------------------------------
enum class RoadLaneRoadMarkType // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 92
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


//-----------------------------------------------------------------------------
//! Lane description: left, right or center
//-----------------------------------------------------------------------------
enum class RoadLaneRoadDescriptionType // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 59
{
    Left,
    Right,
    Center
};

//-----------------------------------------------------------------------------
//! LaneChange of the lane line
//-----------------------------------------------------------------------------
enum class RoadLaneRoadMarkLaneChange // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 59
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
enum class RoadLaneRoadMarkColor // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 92
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
enum class RoadLaneRoadMarkWeight // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 92
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
enum class RoadSignalUnit // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 12
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
    truck,
    van,
    bus,
    trailer,
    bike,
    motorbike,
    tram,
    train,
    pedestrian,
    pole,
    tree,
    vegetation,
    barrier,
    building,
    parkingSpace,
    wind,
    patch,
    guardRail,
    roadSideMarkerPost
};

struct RoadObjectSpecification // http://www.opendrive.org/docs/OpenDRIVEFormatSpecRev1.4H.pdf page 65
{
    RoadObjectType type;
    std::string name {""};
    std::string id {""};
    double s {0};
    double t {0};
    double zOffset {0};
    double validLength {0};
    RoadElementOrientation orientation;
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




