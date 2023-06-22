/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once
#include <memory>
#include <unordered_map>

#include "common/globalDefinitions.h"
#include "common/vector2d.h"

constexpr double maxDouble = std::numeric_limits<double>::max();
constexpr int maxInt = std::numeric_limits<int>::max();
constexpr unsigned int maxNumberLanesExtrapolation = 4;

using OdId = std::string;
using OwlId = uint64_t;
using DReaMId = uint64_t;

constexpr OwlId OwlInvalidId = std::numeric_limits<uint64_t>::max();

//-----------------------------------------------------------------------------
//! @brief speed units as in OpenDrive v1.4 // TUD CG 26.06.2019
//-----------------------------------------------------------------------------
enum class SpeedUnit { MetersPerSecond, KilometersPerHour, MilesPerHour };

//-----------------------------------------------------------------------------
//! @brief speed units as in OpenDrive v1.4 // TUD CG 26.06.2019
//-----------------------------------------------------------------------------
enum class DistanceUnit {
    Meter,
    Kilometer,
    Feet,
    Mile // land mile
};

namespace DReaMDefinitions {
enum class AgentVehicleType { NONE = -2, Undefined = -1, Car = 0, Pedestrian, Motorbike, Bicycle, Truck };

}

// fixation area of interests
enum class ScanAOI { NONE, Right, Straight, Left, InnerRVM, OuterLeftRVM, OuterRightRVM, Dashboard, Other };
enum class ControlAOI { NONE, Right, Left, Oncoming };
enum class GazeType { NONE, ScanGlance, ObserveGlance, ControlGlance };
enum class TrafficDensity { NONE, LOW, MODERATE, HIGH };

enum class TJunctionLayout { LeftRight, LeftStraight, StraightRight };

struct Distribution {
    double mean;
    double std_deviation;
    double min;
    double max;

    Distribution() {
    }
    Distribution(double m, double stdd, double min, double max) : mean(m), std_deviation(stdd), min(min), max(max) {
    }
};

struct DriverGaze {
    double direction;
    double openingAngle;
    Distribution fixationDuration;
};

struct MirrorGaze {
    double direction;
    double openingAngle;
    Distribution fixationDuration;

    Common::Vector2d pos;
};

struct FixationTarget {
    Common::Vector2d fixationPoint{-999, -999};
    int fixationAgent{-999};
};

struct GazeState {
    // includes gaze type and fixated AOI
    std::pair<GazeType, int> fixationState{GazeType::NONE, static_cast<int>(ScanAOI::NONE)};
    FixationTarget target;
    double ufovAngle{-999};
    double openingAngle{-999};
    double viewDistance{100}; // TODO calculate
    int fixationDuration{-999};

    bool godMode = false;

    bool mirrorGaze = false;
    Common::Vector2d mirrorPos{0, 0};
};

struct SpeedLimit {
    double maxAllowedSpeed{0};
    SpeedUnit unit{SpeedUnit::MetersPerSecond};

    void convert(SpeedUnit newUnit) {
        if (unit == newUnit) {
            return;
        }

        double factor1; // from old unit to m/s
        double factor2; // from m/s to new unit

        switch (unit) {
        case SpeedUnit::MetersPerSecond:
            factor1 = 1;
            break;
        case SpeedUnit::KilometersPerHour:
            factor1 = 1 / 3.6;
            break;
        case SpeedUnit::MilesPerHour:
            factor1 = 1.609344 / 3.6;
            break;
        }

        switch (newUnit) {
        case SpeedUnit::MetersPerSecond:
            factor2 = 1;
            break;
        case SpeedUnit::KilometersPerHour:
            factor2 = 3.6;
            break;
        case SpeedUnit::MilesPerHour:
            factor2 = 3.6 / 1.609344;
            break;
        }

        maxAllowedSpeed *= factor1 * factor2;
        unit = newUnit;
    }
};

namespace MentalInfrastructure {
class Road;
class Lane;
} // namespace MentalInfrastructure

// the tolerance where two vectors/ angles are pointing in same direction (in degree)
constexpr double parallelEpsilonDeg = 20;

// phase distances in meter
constexpr double ApproachDistance = 75;
constexpr double DecelerationONEDistance = 50;
constexpr double DecelerationTWODistance = 25;

struct RightOfWay {
    RightOfWay() {
    }
    RightOfWay(bool egoAgent, bool observedAgent) : ego{egoAgent}, observed{observedAgent} {
    }
    bool ego;
    bool observed;
};

enum class IntersectionSpot { NONE = 0, IntersectionEntry, IntersectionExit };

enum class CrossingPhase {
    NONE = 0,
    Approach,
    Deceleration_ONE,
    Deceleration_TWO,
    Crossing_Left_ONE,
    Crossing_Left_TWO,
    Crossing_Straight,
    Crossing_Right,
    Exit
};

enum class CrossingType { Random = -1, NA, Left, Straight, Right };

enum class StoppingPointType {
    NONE,
    Pedestrian_Right,
    Pedestrian_Left,
    Pedestrian_Crossing_ONE,
    Pedestrian_Crossing_TWO,
    Vehicle_Left,
    Vehicle_Crossroad,
};

struct StoppingPoint {
    StoppingPointType type;

    const MentalInfrastructure::Road *road = nullptr;
    const MentalInfrastructure::Lane *lane = nullptr;

    // will be set to 0.0 if stopping point is irrelevant
    double sOffset = maxDouble;

    // will be set to -1.0 if stopping point is irrelevant
    double distanceToEgoFront = maxDouble;

    // VA 06.04.2020
    // nearest position on the road for the stopping point
    double posX = maxDouble;
    double posY = maxDouble;
};

using StoppingPointMap = std::unordered_map<StoppingPointType, StoppingPoint>;

struct CrossingInfo {
    CrossingType type = CrossingType::NA;
    CrossingPhase phase = CrossingPhase::NONE;

    // the id of the junction that the stopping point belongs to
    OdId junctionOdId{""};
    StoppingPointMap egoStoppingPoints{};
};

/**
 * @brief Internal enum, describes what kind of conflict situations could happen.
 *
 * ------- Junction Conflicts ----------
 * JUNCTION_A      ego agent and other agent are on the same junction
 * JUNCTION_B      ego agent is on an junction and other agent moves towards it
 * JUNCTION_C      other agent is on an junction and ego agent moves towards it
 * JUNCTION_D      both agents move towards the same junction
 */
enum class JunctionSituation { JUNCTION_A = 0, JUNCTION_B, JUNCTION_C, JUNCTION_D };

struct CollisionPoint {
    CollisionPoint() {
    }
    ~CollisionPoint() = default;

    int oAgentID = maxInt;
    bool collisionImminent = false;
    double distanceCP = maxDouble;
    double timeToCollision = maxDouble;
};

namespace DReaMRoute {
struct Waypoint {
    OdId roadId{};
    const MentalInfrastructure::Lane *lane;
    double s{};
    bool operator==(const Waypoint &rhs) const {
        return roadId == rhs.roadId && lane == rhs.lane && s == rhs.s;
    }
    bool operator!=(const Waypoint &rhs) const {
        return roadId != rhs.roadId || lane != rhs.lane || s != rhs.s;
    }
};
using Waypoints = std::vector<Waypoint>;

} // namespace DReaMRoute
