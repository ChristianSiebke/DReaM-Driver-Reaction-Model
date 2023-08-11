/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Lane.h"

#include <algorithm>

#include "common/Helper.h"
namespace MentalInfrastructure {

const LanePoint *Lane::GetLastPoint() const
{
    if (lanePointsReference.size() == 0)
        return nullptr;

    return &lanePointsReference.back();
}

const LanePoint *Lane::GetFirstPoint() const
{
    if (lanePointsReference.size() == 0)
        return nullptr;

    return &lanePointsReference.front();
}

void Lane::AddReferencePoint(double x, double y, double hdg, double so, bool inDirection)
{
    if (inDirection)
    {
        LanePoint newPoint(x, y, hdg, so);
        lanePointsReference.push_back(newPoint);
    }
    else
    {
        double new_hdg;
        if (hdg < M_PI)
        {
            new_hdg = hdg + M_PI;
        }
        else
        {
            new_hdg = hdg - M_PI;
        }

        LanePoint newPoint(x, y, new_hdg, this->length - so);
        lanePointsReference.push_front(newPoint);
    }
}
void Lane::AddRightPoint(double x, double y, double hdg, double so, bool inDirection)
{
    if (inDirection)
    {
        LanePoint newPoint(x, y, hdg, so);
        lanePointsRightSide.push_back(newPoint);
    }
    else
    {
        double new_hdg;
        if (hdg < M_PI)
        {
            new_hdg = hdg + M_PI;
        }
        else
        {
            new_hdg = hdg - M_PI;
        }

        LanePoint newPoint(x, y, new_hdg, this->length - so);
        lanePointsRightSide.push_front(newPoint);
    }
}

void Lane::AddLeftPoint(double x, double y, double hdg, double so, bool inDirection)
{
    if (inDirection)
    {
        LanePoint newPoint(x, y, hdg, so);
        lanePointsLeftSide.push_back(newPoint);
    }
    else
    {
        double new_hdg;
        if (hdg < M_PI)
        {
            new_hdg = hdg + M_PI;
        }
        else
        {
            new_hdg = hdg - M_PI;
        }

        LanePoint newPoint(x, y, new_hdg, this->length - so);
        lanePointsLeftSide.push_front(newPoint);
    }
}

void Lane::AddPredecessor(const Lane *lane)
{
    if (std::find(predecessors.begin(), predecessors.end(), lane) == predecessors.end())
    {
        predecessors.push_back(lane);
    }
}

void Lane::AddSuccessor(const Lane *lane)
{
    if (std::find(successors.begin(), successors.end(), lane) == successors.end())
    {
        successors.push_back(lane);
    }
}

LanePoint Lane::InterpolatePoint(double sLaneCoordinate) const
{
    if (lanePointsReference.empty())
    {
        auto message = __FILE__ " Line: " + std::to_string(__LINE__) + "Lane does not have any points -->Can not interpolate point";
        throw std::runtime_error(message);
    }
    if (SLaneCoordinateOutOfLane(sLaneCoordinate))
    {
        auto message = __FILE__ " Line: " + std::to_string(__LINE__) + "sLaneCoordniate out of lane --> Can not interpolate point";
        throw std::runtime_error(message);
    }
    auto upperPointIter = std::upper_bound(lanePointsReference.begin(), lanePointsReference.end(), sLaneCoordinate,
                                           [](auto sLaneCoordniate, LanePoint point) -> bool { return (sLaneCoordniate < point.sOffset); });
    if (upperPointIter == lanePointsReference.end())
    {
        return lanePointsReference.back();
    }
    if (upperPointIter == lanePointsReference.begin()) {
        return lanePointsReference.front();
    }

    auto lowerPointIter = std::prev(upperPointIter, 1);
    double upperDistanceTo_S_Coordinate = (*upperPointIter).sOffset - sLaneCoordinate;
    double lowerDistanceTo_S_Coordinate = sLaneCoordinate - (*lowerPointIter).sOffset;
    double x, y, hdg;
    // inverse distance weighting interpolation (power of 1)
    if (lowerDistanceTo_S_Coordinate > 0 && upperDistanceTo_S_Coordinate > 0)
    {
        x = (((*lowerPointIter).x / lowerDistanceTo_S_Coordinate) + ((*upperPointIter).x / upperDistanceTo_S_Coordinate)) /
            ((1 / lowerDistanceTo_S_Coordinate) + (1 / upperDistanceTo_S_Coordinate));
        y = (((*lowerPointIter).y / lowerDistanceTo_S_Coordinate) + ((*upperPointIter).y / upperDistanceTo_S_Coordinate)) /
            ((1 / lowerDistanceTo_S_Coordinate) + (1 / upperDistanceTo_S_Coordinate));

        auto a = (*lowerPointIter).hdg;
        auto b = (*upperPointIter).hdg;

        double dif = std::fmod(b - a + M_PI, 2 * M_PI);
        if (dif < 0)
            dif += (2 * M_PI);
        dif = dif - M_PI;
        hdg = a + (dif * (lowerDistanceTo_S_Coordinate / (upperDistanceTo_S_Coordinate + lowerDistanceTo_S_Coordinate)));
    }
    else if (std::abs(lowerDistanceTo_S_Coordinate) < 0.001)
    {
        x = (*lowerPointIter).x;
        y = (*lowerPointIter).y;
        hdg = (*lowerPointIter).hdg;
    }
    else
    {
        auto message = __FILE__ " Line: " + std::to_string(__LINE__) + "Can not calculate  point";
        throw std::runtime_error(message);
    }
    LanePoint detectedPoint(x, y, hdg, sLaneCoordinate);
    return detectedPoint;
}

bool Lane::SLaneCoordinateOutOfLane(double sLaneCoordinate) const {
    return GetLastPoint()->sOffset - sLaneCoordinate < -0.01 || -0.01 > sLaneCoordinate - GetFirstPoint()->sOffset;
};

std::optional<const ConflictArea *> Lane::GetConflictAreaWithLane(const Lane *lane) const {
    if (!lane) {
        return std::nullopt;
    }
    auto iter = conflictAreas.find(lane);
    if (iter != conflictAreas.end())
    {
        return &iter->second;
    }
    else
    {
        return std::nullopt;
    }
}
std::optional<NextDirectionLanes> Lane::NextLanes(bool movingInLaneDirection) const {
    NextDirectionLanes result;
    auto nextLanes = this->GetSuccessors();

    if (!movingInLaneDirection) {
        nextLanes = this->GetPredecessors();
    }
    if (nextLanes.empty()) {
        return std::nullopt;
    }
    if (nextLanes.size() == 1) {
        result.straightLanes.push_back(nextLanes.front());
        return result;
    }

    // calculate direction vector of current lane at its end
    auto pointsCurrentLane = this->GetLanePoints();
    auto pointCL = std::prev((pointsCurrentLane).end(), 2);
    Common::Vector2d currentDirection(this->GetLastPoint()->x - pointCL->x, this->GetLastPoint()->y - pointCL->y);
    if (!movingInLaneDirection) {
        pointCL = std::next((pointsCurrentLane).begin(), 1);
        currentDirection = {this->GetFirstPoint()->x - pointCL->x, this->GetFirstPoint()->y - pointCL->y};
    }

    for (auto nextLane : nextLanes) {
        auto pointsNextLane = nextLane->GetLanePoints();
        // calculate direction vector of successor lane
        auto pointNL = std::prev(pointsNextLane.end(), 2);
        Common::Vector2d successorDirection(nextLane->GetLastPoint()->x - pointNL->x, nextLane->GetLastPoint()->y - pointNL->y);
        if (!movingInLaneDirection) {
            pointNL = std::next(pointsNextLane.begin(), 1);
            successorDirection = {nextLane->GetFirstPoint()->x - pointNL->x, nextLane->GetFirstPoint()->y - pointNL->y};
        }

        auto angleDeg = AngleBetween2d(currentDirection, successorDirection) * (180 / M_PI);
        if (parallelEpsilonDeg >= angleDeg || parallelEpsilonDeg >= std::fabs(180 - angleDeg)) {
            result.straightLanes.push_back(nextLane);
        }
        else if (parallelEpsilonDeg < angleDeg && currentDirection.Cross(successorDirection) > 0) {
            result.leftLanes.push_back(nextLane);
        }
        else if (parallelEpsilonDeg < angleDeg && currentDirection.Cross(successorDirection) < 0) {
            result.rightLanes.push_back(nextLane);
        }
    }
    return result;
}

const MentalInfrastructure::Lane *Lane::NextLane(IndicatorState indicatorState, bool movingInLaneDirection)const {
    try {
        auto nextLanePointers = this->GetSuccessors();
        if (!movingInLaneDirection) {
            nextLanePointers = this->GetPredecessors();
        }

        if (nextLanePointers.size() == 1) {
            return nextLanePointers.front();
        }
        // return FIRST lane pointing in indicator direction
        if (auto nextLanes = this->NextLanes(movingInLaneDirection)) {
            if (!nextLanes->straightLanes.empty() &&
                (indicatorState == IndicatorState::IndicatorState_Off || indicatorState == IndicatorState::IndicatorState_Warn)) {
                return nextLanes->straightLanes.front();
            }
            else if (!nextLanes->leftLanes.empty() && indicatorState == IndicatorState::IndicatorState_Left) {
                return nextLanes->leftLanes.front();
            }
            else if (!nextLanes->rightLanes.empty() && indicatorState == IndicatorState::IndicatorState_Right) {
                return nextLanes->rightLanes.front();
            }
            else {
                // the state of the indicator does not match to the directions of the
                // successor lanes (possibly the indicator is not yet set)
                if (!nextLanes->straightLanes.empty()) {
                    // assume agent goes straight
                    return nextLanes->straightLanes.front();
                }
                return nullptr;
            }
        }
        else {
            // no next lane exist or next lane is invalide
            return nullptr;
        }
    }
    catch (...) {
        throw std::runtime_error(__FILE__ " Line: " + std::to_string(__LINE__) + " successorLane can not be determined ");
    }
}

bool Lane::IsJunctionLane() const {
    return this->GetRoad()->IsOnJunction();
}
} // namespace MentalInfrastructure
