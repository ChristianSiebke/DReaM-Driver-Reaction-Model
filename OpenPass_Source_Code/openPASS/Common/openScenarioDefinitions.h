/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
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
    Action(const std::string& sequenceName):
        ScenarioActionInterface(),
        sequenceName{sequenceName}
    {}
    Action() = delete;
    Action(const Action&) = delete;
    Action(Action&&) = delete;
    Action& operator=(const Action&) = delete;
    Action& operator=(Action&&) = delete;

    /*!
     * ------------------------------------------------------------------------
     * \brief GetSequenceName returns the name of the sequence of which this
     *        action is a part.
     *
     * \returns a reference to the name of the sequence of which this action is
     *          a part.
     * ------------------------------------------------------------------------
     */
    const std::string& GetSequenceName() const
    {
        return sequenceName;
    }

private:
    const std::string sequenceName;
};

class GlobalAction : public Action
{
public:
    GlobalAction(const std::string& sequenceName):
        Action(sequenceName)
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
    GlobalEntityAction(const std::string& sequenceName,
                                      GlobalEntityActionType type,
                                      const std::string& name):
        GlobalAction(sequenceName),
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
    PrivateAction(const std::string& sequenceName):
        Action(sequenceName)
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
    PrivateLateralLaneChangeAction(const std::string& sequenceName,
                                                                PrivateLateralLaneChangeActionType type,
                                                                int value,
                                                                const std::string& object = ""):
        PrivateAction(sequenceName),
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

class UserDefinedAction : public Action
{
public:
    UserDefinedAction(const std::string& sequenceName):
        Action(sequenceName)
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
    UserDefinedCommandAction(const std::string& sequenceName,
                             const std::string& command):
        UserDefinedAction(sequenceName),
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
