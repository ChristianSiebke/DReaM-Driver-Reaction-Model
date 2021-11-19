/********************************************************************************
 * Copyright (c) 2018-2019 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2016-2021 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  globalDefinitions.h
//! @brief This file contains several classes for global purposes
//-----------------------------------------------------------------------------

#pragma once

#include "common/opMath.h"
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <vector>

// the following is a temporary workaround until the contribution is merged into osi
#if defined(_WIN32) && !defined(NODLL)
#define OSIIMPORT __declspec(dllimport)
#define OSIEXPORT __declspec(dllexport)

#elif (defined(__GNUC__) && __GNUC__ >= 4 || defined(__clang__))
#define OSIEXPORT __attribute__((visibility("default")))
#define OSIIMPORT OSIEXPORT

#else
#define OSIIMPORT
#define OSIEXPORT
#endif

#if defined(open_simulation_interface_EXPORTS)
#define OSI_EXPORT OSIEXPORT
#else
#define OSI_EXPORT OSIIMPORT
#endif

//-----------------------------------------------------------------------------
//! @brief Containing the three possible states regarding lane change
//-----------------------------------------------------------------------------
enum class LaneChangeState
{
    NoLaneChange = 0,
    LaneChangeLeft,
    LaneChangeRight
};

//-----------------------------------------------------------------------------
//! weekday type
//-----------------------------------------------------------------------------
enum class Weekday
{
    Undefined = 0,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};

//-----------------------------------------------------------------------------
//! @brief Areas of interest for surrounding data
//-----------------------------------------------------------------------------
enum class AreaOfInterest
{
    LEFT_FRONT = 0,
    RIGHT_FRONT,
    LEFT_REAR,
    RIGHT_REAR,
    EGO_FRONT,
    EGO_REAR,
    LEFT_SIDE,
    RIGHT_SIDE,
    //  DOWNSTREAM
    NumberOfAreaOfInterests
};

//-----------------------------------------------------------------------------
//! Agent category classification
//-----------------------------------------------------------------------------
enum class AgentCategory
{
    Ego = 0,
    Scenario,
    Common,
    Any
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 4> AgentCategoryMapping{
    "Ego",
    "Scenario",
    "Common",
    "Any"};

constexpr const char *to_cstr(AgentCategory agentCategory)
{
    return AgentCategoryMapping[static_cast<size_t>(agentCategory)];
}

inline std::string to_string(AgentCategory agentCategory) noexcept
{
    return std::string(to_cstr(agentCategory));
}

} // namespace util

//-----------------------------------------------------------------------------
//! Agent type classification
//-----------------------------------------------------------------------------
enum class AgentVehicleType
{
    NONE = -2,
    Undefined = -1,
    Car = 0,
    Pedestrian,
    Motorbike,
    Bicycle,
    Truck
};

namespace openpass::utils {

/// @brief constexpr map for transforming the a corresponding enumeration into
///        a string representation: try to_cstr(EnumType) or to_string(EnumType)
static constexpr std::array<const char *, 7> AgentVehicleTypeMapping{
    "NONE",
    "Undefined",
    "Car",
    "Pedestrian",
    "Motorbike",
    "Bicycle",
    "Truck"};


constexpr const char *to_cstr(AgentVehicleType agentVehicleType)
{
    return AgentVehicleTypeMapping[static_cast<size_t>(agentVehicleType) -
                                   static_cast<size_t>(AgentVehicleType::NONE)];
}

inline std::string to_string(AgentVehicleType agentVehicleType) noexcept
{
    return std::string(to_cstr(agentVehicleType));
}

} // namespace util


/// @brief convert a vehicle type name to VehicleType enum
inline AgentVehicleType GetAgentVehicleType(const std::string &strVehicleType)
{
    if (0 == strVehicleType.compare("Car"))
    {
        return AgentVehicleType::Car;
    }
    else if (0 == strVehicleType.compare("Pedestrian"))
    {
        return AgentVehicleType::Pedestrian;
    }
    else if (0 == strVehicleType.compare("Motorbike"))
    {
        return AgentVehicleType::Motorbike;
    }
    else if (0 == strVehicleType.compare("Bicycle"))
    {
        return AgentVehicleType::Bicycle;
    }
    else if (0 == strVehicleType.compare("Truck"))
    {
        return AgentVehicleType::Truck;
    }
    return AgentVehicleType::Undefined;
}

// convert a AgentVehicleType to VehicleType string
inline std::string GetAgentVehicleTypeStr(const AgentVehicleType &vehicleType)
{
    return (vehicleType == AgentVehicleType::Car) ? "Car" : (vehicleType == AgentVehicleType::Pedestrian) ? "Pedestrian" : (vehicleType == AgentVehicleType::Motorbike) ? "Motorbike" : (vehicleType == AgentVehicleType::Bicycle) ? "Bicycle" : (vehicleType == AgentVehicleType::Truck) ? "Truck" : "unknown type";
}

// convert a string of type code to VehicleType string
inline std::string GetAgentVehicleTypeStr(const std::string &vehicleTypeCode)
{
    try
    {
        AgentVehicleType vehicleType = static_cast<AgentVehicleType>(std::stoi(vehicleTypeCode));
        return GetAgentVehicleTypeStr(vehicleType);
    }
    catch (...)
    {
        return "unknown type";
    }
}

//! State of indicator lever
enum class IndicatorLever
{
    IndicatorLever_Off = 0,
    IndicatorLever_Left = 1,
    IndicatorLever_Right = -1
};

enum class IndicatorState
{
    IndicatorState_Off = 0,
    IndicatorState_Left = 1,
    IndicatorState_Right = 2,
    IndicatorState_Warn = 3
};

struct Position
{
    Position()
    {
    }
    Position(double x,
             double y,
             double yaw,
             double curvature) :
        xPos(x),
        yPos(y),
        yawAngle(yaw),
        curvature(curvature)
    {
    }

    double xPos{0};
    double yPos{0};
    double yawAngle{0};
    double curvature{0};
};

//! Enum of potential types of marks.
enum class MarkType
{
    NONE,
    CONTINUOUS,
    INTERRUPTED_LONG,
    INTERRUPTED_SHORT,
    ROADSIDE,
    NumberOfMarkTypes
};

//! Enum of potential types of objects.
enum class ObjectType
{
    NONE,
    OBJECT,
    VIEWOBJECT,
    NumberOfObjectTypes
};

enum ObjectTypeOSI : int
{
    None = 0x00, // default at initialization
    Vehicle = 0x01,
    Object = 0x02,
    Any = Vehicle | Object
};

using CollisionPartner = std::pair<ObjectTypeOSI, int>;

enum class LightState
{
    Off = 0,
    LowBeam,
    FogLight,
    HighBeam,
    Flash,
    NumberOfLightStates
};

//! Possibile direction of view angle with agent in center.
enum class AgentViewDirection
{
    none,
    front,
    left,
    back,
    right,
    NumberOfCarViewDirections
};

enum class Side
{
    Left,
    Right
};

struct VehicleModelParameters
{
    AgentVehicleType vehicleType;

    struct BoundingBoxCenter
    {
        double x;
        double y;
        double z;
    } boundingBoxCenter;

    struct BoundingBoxDimensions
    {
        double width;
        double length;
        double height;
    } boundingBoxDimensions;

    struct Performance
    {
        double maxSpeed;
        double maxAcceleration;
        double maxDeceleration;
    } performance;

    struct Axle
    {
        double maxSteering;
        double wheelDiameter;
        double trackWidth;
        double positionX;
        double positionZ;
    };
    Axle frontAxle;
    Axle rearAxle;

    std::map<std::string, double> properties;
};

enum class AdasType
{
    Safety = 0,
    Comfort,
    Undefined
};

const std::map<AdasType, std::string> adasTypeToString = {{AdasType::Safety, "Safety"},
                                                          {AdasType::Comfort, "Comfort"},
                                                          {AdasType::Undefined, "Undefined"}};

enum class ComponentType
{
    Driver = 0,
    TrajectoryFollower,
    VehicleComponent,
    Undefined
};

enum class LaneCategory
{
    Undefined = 0,
    RegularLane,
    RightMostLane
};

//! Defines which traffic rules are in effect
struct TrafficRules
{
    double openSpeedLimit; //!< maximum allowed speed if not restricted by signs
    bool keepToOuterLanes; //!< if true, vehicles must use the outermost free lane for driving
    bool dontOvertakeOnOuterLanes; //!< if true, it is prohibited to overtake another vehicle, that is driving further left (or right for lefthand traffic)
    bool formRescueLane; //!< if true, vehicles driving in a traffic jam must form a corridor for emergency vehicles
    bool zipperMerge; //!< if true all merging shall be performed using zipper merge
};

//-----------------------------------------------------------------------------
//! Representation of an agent as defined in the run configuration.
//-----------------------------------------------------------------------------
class AgentSpawnItem
{
public:
    AgentSpawnItem(int id, int reference) :
        id(id),
        reference(reference)
    {
    }
    AgentSpawnItem(const AgentSpawnItem &) = delete;
    AgentSpawnItem(AgentSpawnItem &&) = delete;
    AgentSpawnItem &operator=(const AgentSpawnItem &) = delete;
    AgentSpawnItem &operator=(AgentSpawnItem &&) = delete;
    virtual ~AgentSpawnItem() = default;

    int GetId() const
    {
        return id;
    }

    int GetReference() const
    {
        return reference;
    }

    AgentVehicleType GetVehicleType() const
    {
        return vehicleType;
    }

    double GetWidth() const
    {
        return width;
    }

    double GetLength() const
    {
        return length;
    }

    double GetDistanceCOGtoFrontAxle() const
    {
        return distanceCOGtoFrontAxle;
    }

    double GetWeight() const
    {
        return weight;
    }

    double GetHeightCOG() const
    {
        return heightCOG;
    }

    double GetWheelbase() const
    {
        return wheelbase;
    }

    double GetMomentInertiaRoll() const
    {
        return momentInertiaRoll;
    }

    double GetMomentInertiaPitch() const
    {
        return momentInertiaPitch;
    }

    double GetMomentInertiaYaw() const
    {
        return momentInertiaYaw;
    }

    double GetFrictionCoeff() const
    {
        return frictionCoeff;
    }

    double GetTrackWidth() const
    {
        return trackWidth;
    }

    double GetDistanceCOGtoLeadingEdge() const
    {
        return distanceCOGtoLeadingEdge;
    }

    void SetVehicleType(AgentVehicleType vehicleType)
    {
        this->vehicleType = vehicleType;
    }

    void SetWidth(double width)
    {
        this->width = width;
    }

    void SetLength(double length)
    {
        this->length = length;
    }

    void SetDistanceCOGtoFrontAxle(double distanceCOGtoFrontAxle)
    {
        this->distanceCOGtoFrontAxle = distanceCOGtoFrontAxle;
    }

    void SetWeight(double weight)
    {
        this->weight = weight;
    }

    void SetHeightCOG(double heightCOG)
    {
        this->heightCOG = heightCOG;
    }

    void SetWheelbase(double wheelbase)
    {
        this->wheelbase = wheelbase;
    }

    void SetMomentInertiaRoll(double momentInertiaRoll)
    {
        this->momentInertiaRoll = momentInertiaRoll;
    }

    void SetMomentInertiaPitch(double momentInertiaPitch)
    {
        this->momentInertiaPitch = momentInertiaPitch;
    }

    void SetMomentInertiaYaw(double momentInertiaYaw)
    {
        this->momentInertiaYaw = momentInertiaYaw;
    }

    void SetFrictionCoeff(double frictionCoeff)
    {
        this->frictionCoeff = frictionCoeff;
    }

    void SetTrackWidth(double trackWidth)
    {
        this->trackWidth = trackWidth;
    }

    void SetDistanceCOGtoLeadingEdge(double distanceCOGtoLeadingEdge)
    {
        this->distanceCOGtoLeadingEdge = distanceCOGtoLeadingEdge;
    }

private:
    int id;
    int reference;
    AgentVehicleType vehicleType;
    double positionX;
    double positionY;
    double width;
    double length;
    double velocityX;
    double velocityY;
    double distanceCOGtoFrontAxle;
    double weight;
    double heightCOG;
    double wheelbase;
    double momentInertiaRoll;
    double momentInertiaPitch;
    double momentInertiaYaw;
    double frictionCoeff;
    double trackWidth;
    double distanceCOGtoLeadingEdge;
    double accelerationX;
    double accelerationY;
    double yawAngle;
};

struct PostCrashVelocity
{
    bool isActive = false;//!< activity flag
    double velocityAbsolute = 0.0;//!< post crash velocity, absolute [m/s]
    double velocityDirection = 0.0;//!< post crash velocity direction [rad]
    double yawVelocity = 0.0;//!< post crash yaw velocity [rad/s]
};

/*!
 * For definitions see http://indexsmart.mirasmart.com/26esv/PDFfiles/26ESV-000177.pdf
 */
struct CollisionAngles{
    double OYA = 0.0; //!< opponent yaw angle
    double HCPAo = 0.0; //!< original host collision point angle
    double OCPAo = 0.0; //!< original opponent collision point angle
    double HCPA = 0.0; //!< transformed host collision point angle
    double OCPA = 0.0; //!< transformed opponent collision point angle
};

class AgentInterface;
using AgentInterfaces = std::vector<const AgentInterface*>;
