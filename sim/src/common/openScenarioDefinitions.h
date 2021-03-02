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
    std::string name; //!Name of the parameter in OpenSCENARIO
    T defaultValue; //!Value defined in the catalog (may later be overwriten in the CatalogReference)

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

// OSCOrientation
enum class OrientationType
{
    Undefined = 0,
    Relative,
    Absolute
};

// OSCOrientation
struct Orientation
{
    std::optional<OrientationType> type{};
    std::optional<double> h{};
    std::optional<double> p{};
    std::optional<double> r{};
};

// OSCPosition
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

struct RelativeLanePosition
{
    std::string entityRef{};
    int dLane{};
    double ds{};
    std::optional<double> offset{};
    std::optional<Orientation> orientation{};
};

struct RoadPosition
{
    std::optional<Orientation> orientation{};
    std::string roadId{};
    double s{};
    double t{};
};

struct WorldPosition
{
    double x{};
    double y{};
    std::optional<double> z;
    std::optional<double> h;
    std::optional<double> p;
    std::optional<double> r;
};

using Position = std::variant<LanePosition,
                              RelativeLanePosition,
                              RoadPosition,
                              WorldPosition>;

struct TrafficSignalControllerPhase
{
    std::string name;
    double duration;
    std::map<std::string, std::string> states;
};

struct TrafficSignalController
{
    std::string name;
    double delay;
    std::vector<TrafficSignalControllerPhase> phases;
};

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

using GlobalAction = std::variant<EntityAction>;

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

struct Trajectory
{
    std::vector<TrajectoryPoint> points;
    std::string name;

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

using AssignRouteAction = std::vector<RoadPosition>;

struct FollowTrajectoryAction
{
    Trajectory trajectory{};
};

using RoutingAction = std::variant<AssignRouteAction, FollowTrajectoryAction>;

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
