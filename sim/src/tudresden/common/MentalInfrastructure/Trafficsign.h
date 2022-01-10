/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#pragma once

#include "Common/commonTools.h"
#include "Road.h"
#include "unordered_map"

namespace MentalInfrastructure {

enum class TrafficSignType
{
    // enum values do NOT represent any actual traffic sign catalog values
    // default value
    Undefined = 0,
    // values 1-100 for any signs with right of way information
    Stop = 1,
    GiveWay = 2,
    RightOfWayBegin = 3,
    RightOfWayEnd = 4,
    RightOfWayNextIntersection = 5,
    DoNotEnter = 6,
    // values 101-200 for any signs with speed limits
    MaximumSpeedLimit = 101,
    MinimumSpeedLimit = 102,
    EndOfMaximumSpeedLimit = 103,
    EndOfMinimumSpeedLimit = 104,
    EndOffAllSpeedLimitsAndOvertakingRestrictions = 105,
    TownBegin = 106,
    TownEnd = 107,
    Zone30Begin = 108,
    Zone30End = 109,
    TrafficCalmedDistrictBegin = 110,
    TrafficCalmedDistrictEnd = 111,
};

class TrafficSign
{
public:
    TrafficSign()
    {
    }
    TrafficSign(Id id, std::string odId, const MentalInfrastructure::Road *road, double v, double t, double s, Common::Vector2d pos,
                CommonTrafficSign::Type commonType) :
        id(id), odId(odId), road(road), value(v), t(t), s(s), position(pos)
    {
        type = (trafficSignMapping.at(commonType));
        priority = Priority(type);
    }
    ~TrafficSign() = default;

    Id GetId() const
    {
        return id;
    }

    std::string GetOpenDriveId() const
    {
        return odId;
    }

    const Common::Vector2d GetPosition() const
    {
        return position;
    }

    double GetValue() const
    {
        return value;
    }

    double GetT() const
    {
        return t;
    }

    double GetS() const
    {
        return s;
    }

    TrafficSignType GetType() const
    {
        return type;
    }
    int GetPriority() const
    {
        return priority;
    }

    const MentalInfrastructure::Road *GetRoad() const
    {
        return road;
    }

private:
    int Priority(TrafficSignType sign);

    Id id;
    std::string odId;
    const MentalInfrastructure::Road *road;
    TrafficSignType type;
    int priority;
    double value;
    double t;
    double s;
    Common::Vector2d position;

    std::unordered_map<CommonTrafficSign::Type, TrafficSignType> trafficSignMapping = {
        {CommonTrafficSign::Type::Undefined, TrafficSignType::Undefined},
        {CommonTrafficSign::Type::Stop, TrafficSignType::Stop},
        {CommonTrafficSign::Type::GiveWay, TrafficSignType::GiveWay},
        {CommonTrafficSign::Type::RightOfWayBegin, TrafficSignType::RightOfWayBegin},
        {CommonTrafficSign::Type::RightOfWayEnd, TrafficSignType::RightOfWayEnd},
        {CommonTrafficSign::Type::RightOfWayNextIntersection, TrafficSignType::RightOfWayNextIntersection},
        {CommonTrafficSign::Type::DoNotEnter, TrafficSignType::DoNotEnter},
        {CommonTrafficSign::Type::MaximumSpeedLimit, TrafficSignType::MaximumSpeedLimit},
        {CommonTrafficSign::Type::MinimumSpeedLimit, TrafficSignType::MinimumSpeedLimit},
        {CommonTrafficSign::Type::EndOfMaximumSpeedLimit, TrafficSignType::EndOfMaximumSpeedLimit},
        {CommonTrafficSign::Type::EndOfMinimumSpeedLimit, TrafficSignType::EndOfMinimumSpeedLimit},
        {CommonTrafficSign::Type::EndOffAllSpeedLimitsAndOvertakingRestrictions,
         TrafficSignType::EndOffAllSpeedLimitsAndOvertakingRestrictions},
        {CommonTrafficSign::Type::TownBegin, TrafficSignType::TownBegin},
        {CommonTrafficSign::Type::TownEnd, TrafficSignType::TownEnd},
        {CommonTrafficSign::Type::SpeedLimitZoneBegin, TrafficSignType::Zone30Begin},
        {CommonTrafficSign::Type::SpeedLimitZoneEnd, TrafficSignType::Zone30End},
        {CommonTrafficSign::Type::TrafficCalmedDistrictBegin, TrafficSignType::TrafficCalmedDistrictBegin},
        {CommonTrafficSign::Type::TrafficCalmedDistrictEnd, TrafficSignType::TrafficCalmedDistrictEnd},
    };
};
} // namespace MentalInfrastructure
