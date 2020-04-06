/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016, 2017, 2018, 2019 ITK Engineering GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <map>
#include <unordered_map>

#include <boost/algorithm/string/trim.hpp>

#include "globalDefinitions.h"
#include "vector2d.h"

#include "Interfaces/agentInterface.h"
#include "Interfaces/worldObjectInterface.h"

#include "Common/boostGeometryCommon.h"

namespace CommonHelper
{

[[maybe_unused]] static inline constexpr bool CheckPointValid(const Common::Vector2d *point)
{
    return ((point != nullptr) && (point->x != INFINITY) && (point->y != INFINITY));
}

//! Calculate the absolute angle between two pcm points.
//!
//! @param[in]    firstPoint     firstPoint
//! @param[in]    secondPoint    secondPoint
//! @return       distance between two pcm points
//-----------------------------------------------------------------------------
[[maybe_unused]] static double CalcAngleBetweenPoints(const Common::Vector2d& firstPoint, const Common::Vector2d& secondPoint)
{
    if ((!CheckPointValid(&firstPoint)) || (!CheckPointValid(&secondPoint)))
    {
        return INFINITY;
    }

    double angle = (secondPoint - firstPoint).Angle();

    return angle;
}

//! Transform a pcm point to a vector in the coordination system of a
//! source point in a direction.
//!
//! @param[in]    point     point
//! @return                 vector
[[maybe_unused]] static Common::Vector2d TransformPointToSourcePointCoord(const Common::Vector2d *point,
                                                         const Common::Vector2d *sourcePoint,
                                                         double direction)
{
    Common::Vector2d newPoint = *point; //(point->x, point->y);
    newPoint.Translate((*sourcePoint) * (-1));
    newPoint.Rotate(direction * (-1));

    return newPoint;
}

//-----------------------------------------------------------------------------
//! @brief Round doubles.
//!
//! Rounds doubles to a given amount of decimals.
//!
//! @param[in] value            Value which is rounded
//! @param[in] decimals         Amount of decimals.
//!
//! @return                     Rounded value.
//-----------------------------------------------------------------------------
static double roundDoubleWithDecimals(double value, int decimals)
{
    return std::floor((value * (std::pow(10, decimals))) + 0.5)/(std::pow(10.0, decimals));
}

[[maybe_unused]] static std::optional<Common::Vector2d> CalculateIntersection(const Common::Vector2d& firstStartPoint, const Common::Vector2d& firstAxis,
                                                      const Common::Vector2d& secondStartPoint, const Common::Vector2d& secondAxis)
{
    //Solve linear equation firstStartPoint + lambda * firstAxis = secondStart + kappa * secondAxis
    double determinant = - firstAxis.x * secondAxis.y + firstAxis.y * secondAxis.x; //Determinant of matrix (firstAxis -secondAxis)
    if (std::abs(determinant) < EPSILON)
    {
        return std::nullopt;
    }
    double lambda = (- (secondStartPoint.x - firstStartPoint.x) * secondAxis.y
                     + (secondStartPoint.y - firstStartPoint.y) * secondAxis.x)
                    / determinant;
    double intersectionPointX = firstStartPoint.x + lambda * firstAxis.x;
    double intersectionPointY = firstStartPoint.y + lambda * firstAxis.y;
    return std::make_optional<Common::Vector2d>(intersectionPointX, intersectionPointY);
}

//-----------------------------------------------------------------------------
//! @brief Tokenizes string by delimiter.
//!
//! @param [in] str              String to be tokenized
//! @param [in] delimiter        Delimiter by which string gets tokenized
//!
//! @return                     Vector of tokens
//-----------------------------------------------------------------------------
[[maybe_unused]] static std::vector<std::string> TokenizeString(const std::string& str, const char delimiter)
{
    std::stringstream stream(str);

    std::string intermediateString;
    std::vector<std::string> tokens;
    while (getline(stream, intermediateString, delimiter))
    {
        boost::algorithm::trim(intermediateString);
        tokens.push_back(intermediateString);
    }

    return tokens;
}
}; // namespace CommonHelper

//-----------------------------------------------------------------------------
//! @brief Containing general static functions to evaluate traffic situations
//-----------------------------------------------------------------------------
class TrafficHelperFunctions
{
public:
    //-----------------------------------------------------------------------------
    //! @brief Calculates the net time gap between two (moving) objects
    //!
    //! Time gap is the time until two objects collide if the front object stands
    //! still and the velocity of the rear object remains unchanged.
    //!
    //! Net in this context means that the carlengths are already substracted from
    //! the caller, i.e. netDistance = |sDist| - c
    //!   where c REK-usually \in {carlengthEffective, carlength}
    //! More sophisticated would be c = dist(refPointRear, FrontEdgeRear) +
    //!  dist(refPointFront, RearEdgeFront) + MinGap
    //! where MinGap = 0 for true TTC, >0 for estimated TTC with a safety gap
    //!
    //! Default values of the function:
    //! -1 | netDistance < 0
    //! 0  | netDistance = 0
    //! netDistance | vRear <= 1
    //!
    //! Java: computeNetGap
    //!
    //! @param [in]   vRear         velocity of the rear object
    //! @param [in]   netDistance   distance between the objects
    //!
    //! @return time gap between the two objects
    //-----------------------------------------------------------------------------
    static double CalculateNetTimeGap(
        double vRear,
        double netDistance)
    {
        if (netDistance < 0.0)
        {
            return -1.0;
        }

        return (netDistance <= 0.0) ? 0.0 : (vRear <= 1) ? netDistance : netDistance / vRear;
    }


    //-----------------------------------------------------------------------------
    //! @brief Limits a value between a lower and an upper limit
    //!
    //! Returns
    //! low | value < low
    //! value  | low <= value < high
    //! high | value >= high
    //!
    //! @param [in]   low       lower limit
    //! @param [in]   value     the value to be bounded
    //! @param [in]   high      higher limit
    //!
    //! @return bounded value
    //-----------------------------------------------------------------------------
    template <typename T>
    static T ValueInBounds(
        const T &low,
        const T &value,
        const T &high)
    {
        return (value < low) ? low : ((value < high) ? value : high);
    }

    //-----------------------------------------------------------------------------
    //! @brief Calculates if two agents will collide during braking
    //!
    //! /details    validates if cars crash during brake phase
    //!             if velocity and acceleration of agents will be the same.
    //!
    //! @param [in]   sDelta     distance between both agents
    //! @param [in]   vEgo       velocity ego
    //! @param [in]   aEgo       acceleration ego
    //! @param [in]   vFront     velocity front
    //! @param [in]   aFront     acceleration front
    //!
    //! @return true if collision would happen
    //-----------------------------------------------------------------------------
    static bool WillCrashDuringBrake(double sDelta, double vEgo, double aEgo, double vFront, double aFront)
    {
        //Calculate the intersection of the vehicles trajactory (quadratic functions)

        auto stopTime = - vEgo / aEgo;   //timepoint where ego stops
        bool frontStopsEarlier = false;
        double frontTravelDistance;

        if (aFront < 0)
        {
            auto stopTimeFront = - vFront / aFront;
            if (stopTimeFront < stopTime) //special case: FrontVehicle stops earlier than Ego
            {
                frontStopsEarlier = true;
                frontTravelDistance = - 0.5 * vFront * vFront / aFront;
            }
        }

        // acceleration equal -> difference of trajectories is linear
        if (aEgo == aFront)
        {

            // velocity is equal: distance constant -> no crash
            if(vEgo == vFront)
            {
                return false;
            }

            // intersection of lines
            auto intersectionTime = sDelta / (vEgo - vFront);
            if (0 <= intersectionTime && intersectionTime <= stopTime)
            {
                if (frontStopsEarlier)
                {
                    auto  travelDistance = - 0.5 * stopTime * stopTime * aEgo;
                    if (travelDistance < sDelta + frontTravelDistance)
                    {
                        return false;
                    }
                }

                return true;
            }
        }

        // intersection of quadratic functions
        auto discriminant = (vEgo - vFront) * (vEgo - vFront) + 2 * (aEgo - aFront) * sDelta;

        if (discriminant < 0)
        {
            return false;
        }


        auto intersectionPoint1 = ((vFront - vEgo) + std::sqrt(discriminant)) / (aEgo - aFront);
        auto intersectionPoint2 = ((vFront - vEgo) - std::sqrt(discriminant)) / (aEgo - aFront);

        if ((0 <= intersectionPoint1 && intersectionPoint1 <= stopTime) ||
            (0 <= intersectionPoint2 && intersectionPoint2 <= stopTime))
        {
            if (frontStopsEarlier)
            {
                auto travelDistance = - 0.5 * stopTime * stopTime * aEgo;
                if (travelDistance < sDelta + frontTravelDistance)
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    //! @brief Calculates if two agents will collide during braking
    //!
    //! \details validates if two cars crash within time to brake
    //!             if velocity and acceleration of agents will be the same.
    //!             Triggers WillCrashDuringBrake
    //!
    //! @param [in]   sDelta       distance between both agents
    //! @param [in]   vEgo         velocity rear agent (ego)
    //! @param [in]   aEgo         acceleration rear agent (ego)
    //! @param [in]   vFront       velocity front
    //! @param [in]   aFront       acceleratin front
    //! @param [in]   assumedTtb   assumed time to brake
    //!
    //! @return true if collision would happen
    //-----------------------------------------------------------------------------
    static bool WillCrash(double sDelta, double vEgo, double assumedBrakeAccelerationEgo, double vFront, double aFront, double assumedTtb)
    {
        auto sEgoAtTtb = vEgo * assumedTtb;

        // estimate values for front vehicle at t=ttb
        auto stopTimeFront = aFront < 0 ? -vFront/aFront : std::numeric_limits<double>::max();
        auto tFront = std::min(stopTimeFront, assumedTtb);
        auto sFrontAtTtb = sDelta + vFront*tFront + aFront*tFront*tFront/2;
        auto vFrontAtTtb = stopTimeFront < assumedTtb ? 0 : vFront + aFront*assumedTtb;
        auto aFrontAtTtb = stopTimeFront < assumedTtb ? 0 : aFront;

        if(sFrontAtTtb <= sEgoAtTtb) {
            return true;
        }

        return WillCrashDuringBrake(sFrontAtTtb - sEgoAtTtb, vEgo, assumedBrakeAccelerationEgo, vFrontAtTtb, aFrontAtTtb);
    }
};

namespace helper::vector {

/// \brief  convenience function to convert a vector into a string using a constom delimiter
/// \remark Please refer to the standard, how decimals are converted
/// \param  values     vector of strings
/// \param  delimiter  string in between the individual values
/// \return delimiter seperated list as string
template <typename T>
std::string to_string(const std::vector<T>& values, const std::string& delimiter = ",")
{
    if (values.empty())
    {
        return "";
    }

    std::ostringstream oss;
    std::copy(values.begin(), values.end(), std::ostream_iterator<T>(oss, delimiter.c_str()));

    return {oss.str(), 0, oss.str().length() - delimiter.size()};
}
} // namespace helper::vector

namespace helper::map {
/// @brief queries a map for a given key and returns the value if available
template <typename KeyType, typename ValueType>
std::optional<ValueType> query(const std::map<KeyType, ValueType>& the_map, KeyType the_value)
{
    const auto& iter = the_map.find(the_value);
    return iter == the_map.end() ? std::nullopt : std::make_optional(iter->second);
}

template <typename ValueType>
std::optional<ValueType> query(const std::map<std::string, ValueType>& the_map, std::string the_value)
{
    const auto& iter = the_map.find(the_value);
    return iter == the_map.end() ? std::nullopt : std::make_optional(iter->second);
}

template <typename KeyType, typename ValueType>
std::optional<ValueType> query(const std::unordered_map<KeyType, ValueType>& the_map, KeyType the_value)
{
    const auto& iter = the_map.find(the_value);
    return iter == the_map.end() ? std::nullopt : std::make_optional(iter->second);
}

template <typename ValueType>
std::optional<ValueType> query(const std::unordered_map<std::string, ValueType>& the_map, std::string the_value)
{
    const auto& iter = the_map.find(the_value);
    return iter == the_map.end() ? std::nullopt : std::make_optional(iter->second);
}
} // namespace helper::map

//-----------------------------------------------------------------------------
//! @brief Containing functions to calculate time to collision of two objects
//-----------------------------------------------------------------------------
class TtcCalculations
{
public:
    //-----------------------------------------------------------------------------
    //! @brief Structure to hold all necessary world object parameters for TTC calculation
    //-----------------------------------------------------------------------------
    struct TtcParameters{
        double length;
        double width;
        double frontLength; // distance from reference point of object to leading edge of object
        double backLength;  // distance from reference point to back edge of object
        point_t position;
        double velocityX;
        double velocityY;
        double accelerationX;
        double accelerationY;
        double yaw;
        double yawRate;
        double yawAcceleration;
    };
    //-----------------------------------------------------------------------------
    //! @brief Calculates the time to collision between two (moving) objects
    //!
    //! Time to collision (TTC) is the time until two objects collide if their
    //! accelerations remain unchanged.
    //!
    //!
    //!
    //! @param [in]   vRear         velocity of the rear object
    //! @param [in]   vFront        velocity of the front object
    //! @param [in]   netDistance   distance between the objects
    //!
    //! @return time to collsion between the two objects
    //-----------------------------------------------------------------------------
    static double CalculateObjectTTC(const AgentInterface &agent, const WorldObjectInterface &detectedObject, double maxTtc,
                                     double collisionDetectionLongitudinalBoundary, double collisionDetectionLateralBoundary, double cycleTime)
    {
        TtcParameters agentParameters = GetTTCObjectParameters(&agent, collisionDetectionLongitudinalBoundary, collisionDetectionLateralBoundary);
        TtcParameters objectParameters = GetTTCObjectParameters(&detectedObject, collisionDetectionLongitudinalBoundary, collisionDetectionLateralBoundary);

        return CalculateObjectTTC(agentParameters, objectParameters, maxTtc, cycleTime);
    }

    static double CalculateObjectTTC(TtcParameters agentParameters, TtcParameters objectParameters, double maxTtc, double cycleTime)
    {
        double timestep = cycleTime / 1000.0;
        double ttc = 0.0;

        while (ttc <= maxTtc)
        {
            ttc += timestep;
            // Propagate parameters 1 timestep ahead
            PropagateParametersForward(agentParameters, timestep);
            PropagateParametersForward(objectParameters, timestep);
            // Calculate the new bounding boxes
            polygon_t agentBoundingBox = CalculateBoundingBox(agentParameters);
            polygon_t objectBoundingBox = CalculateBoundingBox(objectParameters);
            // Check for collision
            if (bg::intersects(agentBoundingBox, objectBoundingBox))
            {
                return ttc;
            }

        }
        // No collision detected
        return std::numeric_limits<double>::max();
    }
private:
    //-----------------------------------------------------------------------------
    //! @brief Retrieves the parameters to calculate the TTC for a world object
    //!
    //! @param [in]   object                                    world object
    //! @param [in]   collisionDetectionLongitudinalBoundary    longitudinal padding for collision detection
    //! @param [in]   collisionDetectionLateralBoundary         lateral padding for collision detection
    //!
    //! @return parameters needed for TTC calculation
    //-----------------------------------------------------------------------------
    static TtcParameters GetTTCObjectParameters(const WorldObjectInterface *object, double collisionDetectionLongitudinalBoundary, double collisionDetectionLateralBoundary)
    {
        TtcParameters parameters;

        // Initial bounding box in local coordinate system
        parameters.length = object->GetLength() + collisionDetectionLongitudinalBoundary;
        parameters.width  = object->GetWidth() + collisionDetectionLateralBoundary;
        parameters.frontLength = object->GetDistanceReferencePointToLeadingEdge() + 0.5 * collisionDetectionLongitudinalBoundary; //returns the distance from reference point of object to leading edge of object
        parameters.backLength = parameters.length - parameters.frontLength; // distance from reference point to back edge of object

        // inital object values at current position
        parameters.position = { object->GetPositionX(), object->GetPositionY() };
        parameters.velocityX = object->GetVelocity(VelocityScope::DirectionX);
        parameters.velocityY = object->GetVelocity(VelocityScope::DirectionY);
        parameters.accelerationX = object->GetAcceleration() * std::cos(object->GetYaw());
        parameters.accelerationY = object->GetAcceleration() * std::sin(object->GetYaw());
        parameters.yaw = object->GetYaw();
        parameters.yawRate = 0.0;
        parameters.yawAcceleration = 0.0;

        if (const AgentInterface* objectAsAgent = dynamic_cast<const AgentInterface*>(object)) // cast returns nullptr if unsuccessful
        {
            // object is movable -> get acceleration, yawRate and yawAcceleration
            parameters.yawRate = objectAsAgent->GetYawRate();
            // Not implemented yet:
            //parameters.yawAcceleration =  objectAsAgent->GetYawAcceleration();
        }

        return parameters;
    }

    //-----------------------------------------------------------------------------
    //! @brief Calculates the bounding box of world objects in world coordinates
    //!
    //! @param [in]   parameters    current state of the world objects reference point
    //!
    //! @return bounding box in world coordinates
    //-----------------------------------------------------------------------------
    static polygon_t CalculateBoundingBox(const TtcParameters parameters)
    {

        // construct corner points from reference point position and current yaw angle
        polygon_t box;

        bg::append(box, point_t{ parameters.position.get<0>() - std::cos(parameters.yaw) * parameters.backLength  + std::sin(parameters.yaw) * 0.5 * parameters.width		,		parameters.position.get<1>() - std::sin(parameters.yaw) * parameters.backLength  - std::cos(parameters.yaw) * 0.5 * parameters.width }); // back right corner
        bg::append(box, point_t{ parameters.position.get<0>() - std::cos(parameters.yaw) * parameters.backLength  - std::sin(parameters.yaw) * 0.5 * parameters.width		,		parameters.position.get<1>() - std::sin(parameters.yaw) * parameters.backLength  + std::cos(parameters.yaw) * 0.5 * parameters.width }); // back left corner
        bg::append(box, point_t{ parameters.position.get<0>() + std::cos(parameters.yaw) * parameters.frontLength - std::sin(parameters.yaw) * 0.5 * parameters.width		,		parameters.position.get<1>() + std::sin(parameters.yaw) * parameters.frontLength + std::cos(parameters.yaw) * 0.5 * parameters.width }); // front left corner
        bg::append(box, point_t{ parameters.position.get<0>() + std::cos(parameters.yaw) * parameters.frontLength + std::sin(parameters.yaw) * 0.5 * parameters.width		,		parameters.position.get<1>() + std::sin(parameters.yaw) * parameters.frontLength - std::cos(parameters.yaw) * 0.5 * parameters.width }); // front right corner
        bg::append(box, point_t{ parameters.position.get<0>() - std::cos(parameters.yaw) * parameters.backLength  + std::sin(parameters.yaw) * 0.5 * parameters.width		,		parameters.position.get<1>() - std::sin(parameters.yaw) * parameters.backLength  - std::cos(parameters.yaw) * 0.5 * parameters.width }); // back right corner
        return box;
    }
    //-----------------------------------------------------------------------------
    //! @brief Propagates a world object forward by one timestep
    //!
    //! @param [in]   parameters    current state of the world objects reference point
    //! @param [in]   timestep      time resolution of the TTC simulation
    //!
    //-----------------------------------------------------------------------------
    static void PropagateParametersForward(TtcParameters &parameters, const double timestep)
    {
        parameters.position.set<0>(parameters.position.get<0>() + 0.5 * parameters.accelerationX * timestep * timestep + parameters.velocityX * timestep); // x-coordinate
        parameters.position.set<1>(parameters.position.get<1>() + 0.5 * parameters.accelerationY * timestep * timestep + parameters.velocityY * timestep); // y-coordinate
        double deltaYaw = 0.5 * parameters.yawAcceleration * timestep * timestep + parameters.yawRate * timestep;
        parameters.yaw += deltaYaw;
        parameters.velocityX = std::cos(deltaYaw) * parameters.velocityX - std::sin(deltaYaw) * parameters.velocityY;
        parameters.velocityY = std::sin(deltaYaw) * parameters.velocityX + std::cos(deltaYaw) * parameters.velocityY;
        parameters.accelerationX = std::cos(deltaYaw) * parameters.accelerationX - std::sin(deltaYaw) * parameters.accelerationY;
        parameters.accelerationY = std::sin(deltaYaw) * parameters.accelerationX + std::cos(deltaYaw) * parameters.accelerationY;
        parameters.velocityX += parameters.accelerationX * timestep;
        parameters.velocityY += parameters.accelerationY * timestep;
        parameters.yawRate += parameters.yawAcceleration * timestep;
    }
};
