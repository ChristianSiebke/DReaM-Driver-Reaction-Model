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

#include <optional>

#include "common/PerceptionData.h"
#include "common/WorldRepresentation.h"

namespace Common {

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
T ValueInBounds(const T &low, const T &value, const T &high)
{
    return (value < low) ? low : ((value < high) ? value : high);
}

template <typename T>
bool anyElementOfCollectionIsElementOfOtherCollection(T collectionA, T collectionB)
{
    return std::any_of(collectionA.begin(), collectionA.end(), [collectionB](const auto &elementA) {
        return std::any_of(collectionB.begin(), collectionB.end(), [elementA](const auto &elementB) {
            assert(elementA != nullptr && elementB != nullptr);
            return (elementA == elementB);
        });
    });
};

/*!
 * \brief creates point in distance
 *
 * @param[in]     distance
 * @param[in]     startingPoint
 * @param[in]     directionAngle    direction angle (rad)
 *
 * @return        point
 */
Common::Vector2d CreatPointInDistance(double distance, Common::Vector2d startingPoint, double directionAngle);

/*!
 * \brief return Agent by id
 *
 * @param[in]     agentId
 * @param[in]     agents
 *
 * @return        agent or nullptr if agent does not exist
 */
const AmbientAgentRepresentation *FindAgentById(int agentId, const AmbientAgentRepresentations &agents);

/*!
 * \brief return closest collision point (by time) of all current collision points to other agents
 *
 * @param[in]     interpretedAgents
 *
 * @return        collision point
 */
std::optional<CollisionPoint>
ClosestCollisionPointByTime(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents);

/*!
 * \brief return agent of closest conflict point (by distance)
 *
 * @param[in]     interpretedAgents
 *
 * @return        possible conflict agent
 */
std::optional<int> AgentWithClosestConflictPoint(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents);

/*!
 * \brief  check whether collision is imminent
 * @param[in]     worldInterpretation
 *
 * @return        true if collision is imminent
 */
bool CollisionImminent(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &interpretedAgents);

/*!
 * \brief  return id of agent in front of ego
 * @param[in]     worldInterpretation
 *
 * @return        id of leading agent
 */
std::optional<int> LeadingCarID(const std::unordered_map<int, std::unique_ptr<AgentInterpretation>> &worldInterpretation);

/**
 * @brief GetDistanceStoppingPoint - returns Distance to nearest Stopping Point
 * @param agent
 * @param worldInterpretation
 * @return Distance to nearest Stopping Point
 */
double GetDistanceStoppingPoint(const AgentRepresentation *agent, const AgentInterpretation *observedAgent,
                                const WorldInterpretation &worldInterpretation);

bool IsVehicle(const AgentRepresentation *agent);
double AngleBetween2d(const Vector2d &vectorA, const Vector2d &vectorB);
std::optional<Vector2d> IntersectionPoint(Vector2d p1, Vector2d p2, Vector2d q1, Vector2d q2);
ConflictSituation
DistanceToConflictArea(const std::pair<const MentalInfrastructure::ConflictArea *, const MentalInfrastructure::Lane *> &egoCA,
                       const std::pair<const MentalInfrastructure::ConflictArea *, const MentalInfrastructure::Lane *> &observedCA, const EgoAgentRepresentation *ego,
                       const AgentRepresentation &observedAgent);
double DistanceToConflictPoint(const AgentRepresentation *agent, const MentalInfrastructure::LanePoint &conflictAreaBorder, OwlId laneId);
bool AgentTouchesLane(const AgentRepresentation *agent, const MentalInfrastructure::Lane *lane);

} // namespace Common
