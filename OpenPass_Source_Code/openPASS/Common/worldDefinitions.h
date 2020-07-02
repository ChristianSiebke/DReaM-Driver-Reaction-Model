/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <limits>

#include "globalDefinitions.h"
#include <optional>
#include <set>
#include "boost/graph/adjacency_list.hpp"

constexpr double EQUALITY_BOUND = 1e-3;

enum class RoadNetworkElementType
{
    Road,
    Junction,
    None
};

struct RoadNetworkElement
{
    RoadNetworkElementType type;
    std::string id;

    RoadNetworkElement() = default;
    RoadNetworkElement(RoadNetworkElementType type, std::string id) :
        type(type),
        id(id)
    {}
};

struct JunctionConnection
{
    std::string connectingRoadId;
    std::string outgoingRoadId;
    bool outgoingStreamDirection;
};

struct JunctionConnectorPriority
{
    JunctionConnectorPriority(std::string high, std::string low) :
        high(high),
        low(low)
    {}
    std::string high;
    std::string low;
};

//!Rank of one junction connection w.r.t. another
enum class IntersectingConnectionRank
{
    Undefined,
    Higher,
    Lower
};

//! Information regarding a connecting road intersecting another
struct IntersectingConnection
{
    std::string id;
    IntersectingConnectionRank rank;
    bool operator== (const IntersectingConnection& other) const
    {
        return (id == other.id) && (rank == other.rank);
    }
};

enum class LaneType
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
    Bidirectional,
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

namespace RelativeWorldView {
struct Lane
{
    int relativeId;
    bool inDrivingDirection;
    LaneType type;
    std::optional<int> predecessor;
    std::optional<int> successor;

    bool operator==(const Lane& other) const
    {
        return relativeId == other.relativeId
                && inDrivingDirection == other.inDrivingDirection
                && type == other.type
                && predecessor == other.predecessor
                && successor == other.successor;
    }
};

struct LanesInterval
{
    double startS;
    double endS;
    std::vector<Lane> lanes;
};

using Lanes = std::vector<LanesInterval>;

struct Junction
{
    double startS;
    double endS;
    std::string connectingRoadId;
};

using Junctions = std::vector<Junction>;
} //end of namespace RelativeWorldView


//! Position of a point on a specific lane
struct RoadPosition
{
    RoadPosition() = default;
    RoadPosition(double s,
                 double t,
                 double hdg):
        s(s),
        t(t),
        hdg(hdg) {}

    double s {0};
    double t {0};
    double hdg {0};

    bool operator==(const RoadPosition& other) const
    {
        return std::abs(s - other.s) < EQUALITY_BOUND
                && std::abs(t - other.t) < EQUALITY_BOUND
                && std::abs(hdg - other.hdg) < EQUALITY_BOUND;
    }
};

//! Position of a point in the road network
struct GlobalRoadPosition
{
    GlobalRoadPosition() = default;
    GlobalRoadPosition(std::string roadId, int laneId, double s, double t, double hdg) :
        roadId{roadId},
        laneId{laneId},
        roadPosition(s, t, hdg)
    {}

    std::string roadId {};
    int laneId {-999};
    RoadPosition roadPosition {};
};

//! This struct describes how much space an agent has to next lane boundary on both sides
struct Remainder
{
    Remainder() = default;
    Remainder(double left, double right) : left{left}, right{right}
    {}

    double left {0.0};
    double right {0.0};
};

//! Interval on a specific road
struct RoadInterval
{
    std::vector<int> lanes;
    double sStart {std::numeric_limits<double>::max()};
    double sEnd {0.0};
    Remainder remainder;
};

//! Position of an object in the road network
struct ObjectPosition
{
    std::map<const std::string, GlobalRoadPosition> referencePoint{};    //! position of the reference point mapped by roadId
    std::map<const std::string, GlobalRoadPosition> mainLocatePoint{};   //! position of the mainLocatePoint (middle of agent front) mapped by roadId
    std::map<std::string, RoadInterval> touchedRoads{}; //! all roads the object is on (fully or partially), key is the roadId
};

//! This represents one node on the road network graph of the world or in the routing graph of an agent.
struct RouteElement
{
    std::string roadId;
    bool inOdDirection{false};

    RouteElement() = default;

    RouteElement (std::string roadId, bool inOdDirection) :
        roadId(roadId),
        inOdDirection(inOdDirection)
    {}

    bool operator==(const RouteElement& other) const
    {
        return roadId == other.roadId
                && inOdDirection == other.inOdDirection;
    }

    bool operator<(const RouteElement& other) const
    {
        return inOdDirection < other.inOdDirection
                || (inOdDirection == other.inOdDirection && roadId < other.roadId);
    }

    typedef boost::vertex_property_tag kind;
};

using RouteElementProperty = boost::property<RouteElement, RouteElement>;

//! Directed graph representing the road network.
//! For each road there is one vertex for each possible driving direction.
//! An edges between two vertices means, that an agent driving on the first road in this direction can after the road ends
//! continue its way on the second road in the given direction.
using RoadGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, RouteElementProperty>;
using RoadGraphVertex = boost::graph_traits<RoadGraph>::vertex_descriptor;
using RoadGraphEdge = boost::graph_traits<RoadGraph>::edge_descriptor;
using RoadGraphVertexMapping = std::map<RouteElement, RoadGraphVertex>;

template <typename T>
using RouteQueryResult = std::map<RoadGraphVertex, T>;

//! @brief Describes the obstruction of an opposing object within the driving lanes
///
class Obstruction
{
public:
    bool valid {false};   ///!< @brief True, if obstruction could be calculated
    double left {0.0};    ///!< @brief How far do I need to drive to the left to pass the object
    double right {0.0};   ///!< @brief How far do I need to drive to the right to pass the object
    double mainLaneLocator {0.0}; ///!< @brief Lateral distance of the mainLaneLocator
    bool isOverlapping {false}; ///!< @brief True, if object obstructs

    Obstruction(double left, double right, double mainLaneLocator) :
        valid{true},
        left{left},
        right{right},
        mainLaneLocator{mainLaneLocator},
        isOverlapping{ left > 0.0 && right < 0.0 }
    {
    }

    Obstruction() = default;

    /// \return Invalid obstruction object
    static Obstruction Invalid()
    {
        return {};
    }

    /// \return Default for no opponent
    static Obstruction NoOpponent()
    {
        return Obstruction(0.0, 0.0, 0.0);
    }
};

//! Longitudinal distance (i.e. along s) between two objects on the same road
struct LongitudinalDistance
{
    std::optional<double> netDistance;     //! distance between boundaries
    std::optional<double> referencePoint;  //! distance between reference points
};


enum class MeasurementPoint
{
    Front,
    Reference,
    Rear
};

namespace CommonTrafficSign {
enum Type
{
    Undefined = 0,
    GiveWay = 205,
    Stop = 206,
    DoNotEnter = 267,
    EnvironmentalZoneBegin = 2701, // 270.1
    EnvironmentalZoneEnd = 2702, // 270.2
    MaximumSpeedLimit = 274,
    SpeedLimitZoneBegin = 2741, //274.1
    SpeedLimitZoneEnd = 2742, // 274.2
    MinimumSpeedLimit = 275,
    OvertakingBanBegin = 276,
    OvertakingBanTrucksBegin = 277,
    EndOfMaximumSpeedLimit = 278,
    EndOfMinimumSpeedLimit = 279,
    OvertakingBanEnd = 280,
    OvertakingBanTrucksEnd = 281,
    EndOffAllSpeedLimitsAndOvertakingRestrictions = 282,
    RightOfWayNextIntersection = 301,
    RightOfWayBegin = 306,
    RightOfWayEnd = 307,
    TownBegin = 310,
    TownEnd = 311,
    TrafficCalmedDistrictBegin = 3251, // 325.1
    TrafficCalmedDistrictEnd = 3252, // 325.2
    HighWayBegin = 3301, // 330.1
    HighWayEnd = 3302, // 330.2
    HighWayExit = 333,
    HighwayExitPole = 450, // 450-(50/51/52),
    AnnounceHighwayExit = 448,
    AnnounceRightLaneEnd = 5311, // 531-(10/11/12/13)
    AnnounceLeftLaneEnd = 5312, // 531-(20/21/22/23)
    DistanceIndication = 1004 // 1004-(30/31/32/33)
};

enum Unit
{
    None = 0,
    Kilogram,
    MeterPerSecond,
    Meter,
    Percentage,
    Second
};

struct Entity
{
    Type type {Type::Undefined};
    Unit unit {Unit::None};
    double distanceToStartOfRoad {0};
    double relativeDistance {0};
    double value {0};
    std::string text {""};
    std::list<Entity> supplementarySigns{};
};
}

namespace LaneMarking
{
enum class Type
{
    None,
    Solid,
    Broken,
    Solid_Solid,
    Solid_Broken,
    Broken_Solid,
    Broken_Broken,
    Grass,
    Botts_Dots,
    Curb
};

enum class Color
{
    White,
    Yellow,
    Red,
    Blue,
    Green
};

struct Entity
{
    Type type{Type::None};
    Color color{Color::White};
    double relativeStartDistance{0.0};
    double width{0.0};
};
}
