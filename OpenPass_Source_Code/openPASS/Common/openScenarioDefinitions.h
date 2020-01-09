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

#include "Interfaces/scenarioActionInterface.h"

#include <optional>
#include <string>
#include <ostream>
#include <vector>

namespace openScenario {

struct ActorInformation
{
    std::optional<bool> triggeringAgentsAsActors{};
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
struct RoadPosition
{
    std::optional<Orientation> orientation{};
    std::string roadId{};
    double s{};
    double t{};
};

struct LanePosition
{
    std::optional<Orientation> orientation{};
    std::string roadId{};
    int laneId{};
    std::optional<double> offset{};
    double s{};
};

// All action classes
class Action : public ScenarioActionInterface
{
public:
    /*!
     * -----------------------------------------------------------------------------
     * \brief Each action is part of an event. The event name specifies to which event the action belongs.
     * -----------------------------------------------------------------------------
     */
    Action(const std::string& eventName):
        eventName{eventName}
    {}
    Action() = delete;
    Action(const Action&) = delete;
    Action(Action&&) = delete;
    Action& operator=(const Action&) = delete;
    Action& operator=(Action&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetEventName returns the name of the event of which this
     *        action is a part.
     *
     * \returns a reference to the name of the event of which this action is
     *          a part.
     * ------------------------------------------------------------------------
     */
    const std::string& GetEventName() const
    {
        return eventName;
    }

private:
    const std::string eventName;
};

class GlobalAction : public Action
{
public:
    GlobalAction(const std::string& eventName):
        Action(eventName)
    {
    }
    GlobalAction() = delete;
    GlobalAction(const GlobalAction&) = delete;
    GlobalAction(GlobalAction&&) = delete;
    GlobalAction& operator=(const GlobalAction&) = delete;
    GlobalAction& operator=(GlobalAction&&) = delete;
};

enum GlobalEntityActionType
{
    Delete = 0,
    Add
};

class GlobalEntityAction : public GlobalAction
{
public:
    GlobalEntityAction(const std::string& eventName,
                                      GlobalEntityActionType type,
                                      const std::string& name):
        GlobalAction(eventName),
        type{type},
        name{name}
    {}
    GlobalEntityAction() = delete;
    GlobalEntityAction(const GlobalEntityAction&) = delete;
    GlobalEntityAction(GlobalEntityAction&&) = delete;
    GlobalEntityAction& operator=(const GlobalEntityAction&) = delete;
    GlobalEntityAction& operator=(GlobalEntityAction&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetType returns the type of Global Entity Action this Action is.
     *
     * \returns GlobalEntityActionType::Delete or GlobalEntityActionType::Add.
     * ------------------------------------------------------------------------
     */
    GlobalEntityActionType GetType() const
    {
        return type;
    }

    /*!
     * ------------------------------------------------------------------------
     * \brief GetName returns the name of the entity specified by this action.
     *
     * \returns the name of the entity specified by this action.
     * ------------------------------------------------------------------------
     */
    const std::string& GetName() const
    {
        return name;
    }

private:
    const GlobalEntityActionType type;
    const std::string name;
    // we'll ignore the "Add" element's "Position" child, even though required by standard
};

class PrivateAction : public Action
{
public:
    PrivateAction(const std::string& eventName):
        Action(eventName)
    {}
    PrivateAction() = delete;
    PrivateAction(const PrivateAction&) = delete;
    PrivateAction(PrivateAction&&) = delete;
    PrivateAction& operator=(const PrivateAction&) = delete;
    PrivateAction& operator=(PrivateAction&&) = delete;
};

enum PrivateLateralLaneChangeActionType
{
    Absolute,
    Relative
};

class PrivateLateralLaneChangeAction : public PrivateAction
{
public:
    PrivateLateralLaneChangeAction(const std::string& eventName,
                                   PrivateLateralLaneChangeActionType type,
                                   int value,
                                   const std::string& object = ""):
        PrivateAction(eventName),
        type{type},
        value{value},
        object{object}
    {}
    PrivateLateralLaneChangeAction() = delete;
    PrivateLateralLaneChangeAction(const PrivateLateralLaneChangeAction&) = delete;
    PrivateLateralLaneChangeAction(PrivateLateralLaneChangeAction&&) = delete;
    PrivateLateralLaneChangeAction& operator=(const PrivateLateralLaneChangeAction&) = delete;
    PrivateLateralLaneChangeAction& operator=(PrivateLateralLaneChangeAction&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetType returns the type of Private Lateral LaneChange Action
     *        this Action is.
     *
     * \returns PrivateLateralLaneChangeActionType::Absolute or
     *          PrivateLateralLaneChangeActionType::Relative.
     * ------------------------------------------------------------------------
     */
    PrivateLateralLaneChangeActionType GetType() const
    {
        return type;
    }

    /*!
     * ------------------------------------------------------------------------
     * \brief GetValue returns the lane offset value specified as a target by
     *        this Action.
     *
     * \returns the lane offset value specified as a target by this Action.
     * ------------------------------------------------------------------------
     */
    int GetValue() const
    {
        return value;
    }

    /*!
     * ------------------------------------------------------------------------
     * \brief GetObject returns the name of the entity from which relative
     *        offsets are specified.
     *
     * \returns the name of the entity from which relative offsets are
     *          specified. Returns "" if no such entity was specified.
     * ------------------------------------------------------------------------
     */
    const std::string& GetObject() const
    {
        return object;
    }

private:
    const PrivateLateralLaneChangeActionType type;
    const int value;
    const std::string object; // optional
    // we'll ignore the LaneChange element's "Dynamics" tag for now
    // we'll ignore the LaneChange element's "targetLaneOffset" attribute for now
};

struct TrajectoryPoint
{
    double time;
    double x;
    double y;
    double yaw;

    bool operator== (const TrajectoryPoint& other) const
    {
        return other.time == time
                && other.x == x
                && other.y == y
                && other.yaw == yaw;
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

class PrivateFollowTrajectoryAction : public PrivateAction
{
public:
    PrivateFollowTrajectoryAction(const std::string& eventName,
                                  const Trajectory& trajectory):
        PrivateAction(eventName),
        trajectory(trajectory)
    {}
    PrivateFollowTrajectoryAction() = delete;
    PrivateFollowTrajectoryAction(const PrivateAction&) = delete;
    PrivateFollowTrajectoryAction(PrivateAction&&) = delete;
    PrivateFollowTrajectoryAction& operator=(const PrivateAction&) = delete;
    PrivateFollowTrajectoryAction& operator=(PrivateAction&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetTrajectory returns the trajectory of this Action.
     *
     * \returns trajectory of this Action
     * ------------------------------------------------------------------------
     */
    const openScenario::Trajectory& GetTrajectory() const
    {
        return trajectory;
    }

private:
    const Trajectory trajectory;
};

class UserDefinedAction : public Action
{
public:
    UserDefinedAction(const std::string& eventName):
        Action(eventName)
    {}
    UserDefinedAction() = delete;
    UserDefinedAction(const UserDefinedAction&) = delete;
    UserDefinedAction(UserDefinedAction&&) = delete;
    UserDefinedAction& operator=(const UserDefinedAction&) = delete;
    UserDefinedAction& operator=(UserDefinedAction&&) = delete;
};

class UserDefinedCommandAction : public UserDefinedAction
{
public:
    UserDefinedCommandAction(const std::string& eventName,
                             const std::string& command):
        UserDefinedAction(eventName),
        command{command}
    {}
    UserDefinedCommandAction() = delete;
    UserDefinedCommandAction(const UserDefinedCommandAction&) = delete;
    UserDefinedCommandAction(UserDefinedCommandAction&&) = delete;
    UserDefinedCommandAction& operator=(const UserDefinedCommandAction&) = delete;
    UserDefinedCommandAction& operator=(UserDefinedCommandAction&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetCommand returns the command string specified for this Action
     *
     * \returns the command string specified for this Action
     * ------------------------------------------------------------------------
     */
    const std::string& GetCommand() const
    {
        return command;
    }

private:
    const std::string command;
};

} // namespace openScenario
