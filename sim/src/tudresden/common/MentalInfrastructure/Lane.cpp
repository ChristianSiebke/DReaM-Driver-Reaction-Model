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
        throw std::out_of_range(message);
    }
    if (SLaneCoordinateOutOfLane(sLaneCoordinate))
    {
        auto message = __FILE__ " Line: " + std::to_string(__LINE__) + "sLaneCoordniate out of lane --> Can not interpolate point";
        throw std::out_of_range(message);
    }
    auto upperPointIter = std::upper_bound(lanePointsReference.begin(), lanePointsReference.end(), sLaneCoordinate,
                                           [](auto sLaneCoordniate, LanePoint point) -> bool { return (sLaneCoordniate < point.sOffset); });
    if (upperPointIter == lanePointsReference.end())
    {
        return lanePointsReference.back();
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
        hdg = (((*lowerPointIter).hdg / lowerDistanceTo_S_Coordinate) + ((*upperPointIter).hdg / upperDistanceTo_S_Coordinate)) /
              ((1 / lowerDistanceTo_S_Coordinate) + (1 / upperDistanceTo_S_Coordinate));
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
        throw std::out_of_range(message);
    }
    LanePoint detectedPoint(x, y, hdg, sLaneCoordinate);
    return detectedPoint;
}

bool Lane::SLaneCoordinateOutOfLane(double sLaneCoordniate) const
{
    return sLaneCoordniate > GetLastPoint()->sOffset || sLaneCoordniate < GetFirstPoint()->sOffset;
};

std::optional<ConflictArea> Lane::GetConflictAreaWithLane(const Lane *lane) const
{
    auto iter = conflictAreas.find(lane);
    if (iter != conflictAreas.end())
    {
        return iter->second;
    }
    else
    {
        return std::nullopt;
    }
}

} // namespace MentalInfrastructure
