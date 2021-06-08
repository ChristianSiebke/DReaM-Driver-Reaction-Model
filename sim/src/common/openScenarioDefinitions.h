/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "commonHelper.h"

#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace openScenario {

//! Value of a ParameterDeclaration or ParameterAssignment element
using ParameterValue = std::variant<bool, int, double, std::string>;

//! Content of a ParameterDeclarations or ParameterAssignments element
using Parameters = std::map<std::string, ParameterValue>;

//! Attribute in a catalog that is not yet resolved
template <typename T>
struct ParameterizedAttribute
{
    std::string name; //!< Name of the parameter in OpenSCENARIO
    T defaultValue;   //!< Value defined in the catalog (may be overwritten in the CatalogReference later)

    ParameterizedAttribute() = default;

    ParameterizedAttribute(const T& value) :
        name{""},
        defaultValue{value} {}

    ParameterizedAttribute(const std::string& name, const T& defaultValue) :
        name{name},
        defaultValue{defaultValue} {}
};

enum class Shape
{
    Linear = 0,
    Step
};

enum class SpeedTargetValueType
{
    Delta = 0,
    Factor
};

// This is a custom solution and not according to the openScenario standard
struct StochasticAttribute
{
    double value = -999;
    double mean = -999;
    double stdDeviation = -999;
    double lowerBoundary = -999;
    double upperBoundary = -999;
};

struct ActorInformation
{
    bool actorIsTriggeringEntity{false};
    std::optional<std::vector<std::string>> actors{};
};

//! OSC Orientation
enum class OrientationType
{
    Undefined = 0,
    Relative,
    Absolute
};

//! OSC Orientation
struct Orientation
{
    std::optional<OrientationType> type{};
    std::optional<double> h{};
    std::optional<double> p{};
    std::optional<double> r{};
};

//! OSC Position
struct LanePosition
{
    std::optional<Orientation> orientation{};
    std::string roadId{};
    int laneId{};
    std::optional<double> offset{};
    double s{};

    std::optional<StochasticAttribute> stochasticOffset;
    std::optional<StochasticAttribute> stochasticS;
};

//! OSC RelativeLanePosition
struct RelativeLanePosition
{
    std::string entityRef{};
    int dLane{};
    double ds{};
    std::optional<double> offset{};
    std::optional<Orientation> orientation{};
};

//! OSC RoadPosition
struct RoadPosition
{
    std::optional<Orientation> orientation{};
    std::string roadId{};
    double s{};
    double t{};
};

//! OSC WorldPosition
struct WorldPosition
{
    double x{};
    double y{};
    std::optional<double> z;
    std::optional<double> h;
    std::optional<double> p;
    std::optional<double> r;
};

//! OSC RelativeObjectPosition
struct RelativeObjectPosition {
    std::optional<Orientation> orientation{};
    std::string entityRef{};
    double dx{};
    double dy{};
    std::optional<double> dz{};
};

//! OSC RelativeWorldPosition
struct RelativeWorldPosition {
    std::optional<Orientation> orientation{};
    std::string entityRef{};
    double dx{};
    double dy{};
    std::optional<double> dz{};
};

//! One of different variants of Position in OpenSCENARIO
using Position = std::variant<LanePosition,
                              RelativeLanePosition,
                              RoadPosition,
                              WorldPosition,
                              RelativeObjectPosition,
                              RelativeWorldPosition>;

// Action
// GlobalAction
enum EntityActionType
{
    Delete = 0,
    Add
};

struct EntityAction
{
    std::string entityRef{};
    EntityActionType type{};
};

struct Sun
{
    double intensity{0.};
    double azimuth{0.};
    double elevation{0.};
};

struct Fog
{
    double visualRange{0.};
};

struct Precipitation
{
    enum Type
    {
        dry,
        rain,
        snow
    } type;

    double intensity;
};

struct Weather
{
    enum CloudState
    {
        skyOff,
        free,
        cloudy,
        overcast,
        rainy
    } cloudState;

    Sun sun;
    Fog fog;
    Precipitation precipitation;
};

struct EnvironmentAction
{
    Weather weather;
};

using GlobalAction = std::variant<EntityAction, EnvironmentAction>;

// PrivateAction
struct TrajectoryPoint
{
    double time{};
    double x{};
    double y{};
    double yaw{};

    bool operator== (const TrajectoryPoint& other) const
    {
        return CommonHelper::DoubleEquality(other.time, time)
                && CommonHelper::DoubleEquality(other.x, x)
                && CommonHelper::DoubleEquality(other.y, y)
                && CommonHelper::DoubleEquality(other.yaw, yaw);
    }

    friend std::ostream& operator<<(std::ostream& os, const TrajectoryPoint& point)
    {
        os << "t = " << point.time
           << " : (" << point.x
           << ", " << point.y
           << ") yaw = " << point.yaw;

        return os;
    }
};

//! OSC TimeReference (for FollowTrajectoryAction)
struct TrajectoryTimeReference
{
    std::string domainAbsoluteRelative;
    double scale;
    double offset;
};

//! OSC Trajectory
struct Trajectory
{
    std::vector<TrajectoryPoint> points;
    std::string name;
    std::optional<TrajectoryTimeReference> timeReference;

    friend std::ostream& operator<<(std::ostream& os, const Trajectory& trajectory)
    {
        os << "Trajectory \"" << trajectory.name << "\"\n";
        for (const auto& point : trajectory.points)
        {
            os << point << "\n";
        }

        return os;
    }
};

//! OSC AssignRouteAction
using AssignRouteAction = std::vector<RoadPosition>;

//! OSC FollowTrajectoryAction
struct FollowTrajectoryAction
{
    Trajectory trajectory{};
};

//! OSC AcquirePositionAction
struct AcquirePositionAction
{
    Position position{};
};

//! Some variant of a RoutingAction in OpenSCENARIO
using RoutingAction = std::variant<AssignRouteAction, FollowTrajectoryAction, AcquirePositionAction>;

struct VisibilityAction
{
    bool graphics {false};
    bool traffic {false};
    bool sensors {false};
};

struct LaneChangeParameter
{
    enum class Type
    {
        Absolute,
        Relative
    };

    enum class DynamicsType
    {
        Time,
        Distance
    };

    Type type{}; //! Whether the target is absolute or relative
    int value{};    //! Value of the target element
    std::string object{}; //! Name of the reference object if relative
    double dynamicsTarget{}; //! Time or distance defined by Dynamics element
    DynamicsType dynamicsType{}; //! Whether time or distance was defined by Dynamics element
};

struct LaneChangeAction
{
    LaneChangeParameter laneChangeParameter{};
};

using LateralAction = std::variant<LaneChangeAction>;

struct TransitionDynamics
{
    Shape shape{};
    double value{};
    std::string dimension{};
};

struct AbsoluteTargetSpeed
{
    double value{};
};

struct RelativeTargetSpeed
{
    std::string entityRef{};
    double value{};
    SpeedTargetValueType speedTargetValueType{};
    bool continuous{};
};

using SpeedActionTarget = std::variant<AbsoluteTargetSpeed, RelativeTargetSpeed>;

struct SpeedAction
{
    TransitionDynamics transitionDynamics {};
    SpeedActionTarget target {};

    std::optional<StochasticAttribute> stochasticValue {};
    std::optional<StochasticAttribute> stochasticDynamics {};
};

using LongitudinalAction = std::variant<SpeedAction>;

using TeleportAction = Position;

using PrivateAction = std::variant<LateralAction, LongitudinalAction, RoutingAction, TeleportAction, VisibilityAction>;

// UserDefinedAction
struct CustomCommandAction
{
    std::string command;
};

using UserDefinedAction = std::variant<CustomCommandAction>;

using Action = std::variant<GlobalAction, PrivateAction, UserDefinedAction>;

struct ManipulatorInformation
{
    ManipulatorInformation(const Action action, const std::string eventName):
        action(action),
        eventName(eventName)
    {}

    const Action action;
    const std::string eventName;
};
} // namespace openScenario
