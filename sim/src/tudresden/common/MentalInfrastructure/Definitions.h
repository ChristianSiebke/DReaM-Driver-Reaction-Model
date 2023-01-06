#pragma once

#include <unordered_map>

#include "common/worldDefinitions.h"

namespace MentalInfrastructure {

/**
 * @brief All types of traffic signs.
 * Partially mirrors CommonTrafficSign::Type, use trafficSignMapping for conversion
 */
enum class TrafficSignType {
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

const std::unordered_map<CommonTrafficSign::Type, MentalInfrastructure::TrafficSignType> trafficSignMapping = {
    {CommonTrafficSign::Type::Undefined, MentalInfrastructure::TrafficSignType::Undefined},
    {CommonTrafficSign::Type::Stop, MentalInfrastructure::TrafficSignType::Stop},
    {CommonTrafficSign::Type::GiveWay, MentalInfrastructure::TrafficSignType::GiveWay},
    {CommonTrafficSign::Type::RightOfWayBegin, MentalInfrastructure::TrafficSignType::RightOfWayBegin},
    {CommonTrafficSign::Type::RightOfWayEnd, MentalInfrastructure::TrafficSignType::RightOfWayEnd},
    {CommonTrafficSign::Type::RightOfWayNextIntersection, MentalInfrastructure::TrafficSignType::RightOfWayNextIntersection},
    {CommonTrafficSign::Type::DoNotEnter, MentalInfrastructure::TrafficSignType::DoNotEnter},
    {CommonTrafficSign::Type::MaximumSpeedLimit, MentalInfrastructure::TrafficSignType::MaximumSpeedLimit},
    {CommonTrafficSign::Type::MinimumSpeedLimit, MentalInfrastructure::TrafficSignType::MinimumSpeedLimit},
    {CommonTrafficSign::Type::EndOfMaximumSpeedLimit, MentalInfrastructure::TrafficSignType::EndOfMaximumSpeedLimit},
    {CommonTrafficSign::Type::EndOfMinimumSpeedLimit, MentalInfrastructure::TrafficSignType::EndOfMinimumSpeedLimit},
    {CommonTrafficSign::Type::EndOffAllSpeedLimitsAndOvertakingRestrictions,
     MentalInfrastructure::TrafficSignType::EndOffAllSpeedLimitsAndOvertakingRestrictions},
    {CommonTrafficSign::Type::TownBegin, MentalInfrastructure::TrafficSignType::TownBegin},
    {CommonTrafficSign::Type::TownEnd, MentalInfrastructure::TrafficSignType::TownEnd},
    {CommonTrafficSign::Type::SpeedLimitZoneBegin, MentalInfrastructure::TrafficSignType::Zone30Begin},
    {CommonTrafficSign::Type::SpeedLimitZoneEnd, MentalInfrastructure::TrafficSignType::Zone30End},
    {CommonTrafficSign::Type::TrafficCalmedDistrictBegin, MentalInfrastructure::TrafficSignType::TrafficCalmedDistrictBegin},
    {CommonTrafficSign::Type::TrafficCalmedDistrictEnd, MentalInfrastructure::TrafficSignType::TrafficCalmedDistrictEnd},
};

/**
 * @brief All possible states of a traffic light.
 * Mirrors CommonTrafficLight::State
 */
enum class TrafficLightState { Off, Green, Yellow, Red, RedYellow, YellowFlashing };

/**
 * @brief All types of traffic lights.
 * Mirrors CommonTrafficLight::Type
 */
enum class TrafficLightType {
    Undefined,
    ThreeLights,
    ThreeLightsLeft,
    ThreeLightsRight,
    ThreeLightsStraight,
    ThreeLightsLeftStraight,
    ThreeLightsRightStraight,
    TwoLights,
    TwoLightsPedestrian,
    TwoLightsBicycle,
    TwoLightsPedestrianBicycle,
};

enum class LaneType {
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
    ConnectingRamp,
    Tram,
    RoadWorks,
    Bidirectional
};

} // namespace MentalInfrastructure