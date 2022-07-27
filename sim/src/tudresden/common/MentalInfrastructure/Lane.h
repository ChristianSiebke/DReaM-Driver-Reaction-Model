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
#include <list>
#include <unordered_map>

#include "Element.h"
#include "common/vector2d.h"
#include "common/worldDefinitions.h"

namespace MentalInfrastructure {

class Road;
///
/// \brief Struct to encapsulate a sampled point along the length of a lane.
///
struct LanePoint
{
    LanePoint()
    {
    }
    LanePoint(double xPos, double yPos, double hdgValue, double sValue)
    {
        x = xPos;
        y = yPos;
        hdg = hdgValue;
        sOffset = sValue;
    }

    double x;
    double y;
    double hdg;
    double sOffset;
};

struct ConflictArea
{
    OdId junction;
    OdId road;
    OdId lane;
    LanePoint start;
    LanePoint end;
};

///
/// \brief Represents a lane in the road network. Lanes always belong to a Section on a road.
///
class Lane : public Element
{
public:
    Lane(OdId openDriveId, DReaMId dreamId, OwlId owlId, double length, LaneType type, bool inRoadDirection) :
        Element(openDriveId, dreamId), owlId(owlId), length(length), type(type), inRoadDirection(inRoadDirection) {
    }
    ~Lane() override
    {
    }

    OwlId GetOwlId() const {
        return owlId;
    }

    ///
    /// \brief Returns the length of the lane.
    ///
    double GetLength() const {
        return length;
    }

    ///
    /// \brief Returns the width of the lane.
    ///
    double GetWidth() const {
        return width;
    }

    ///
    /// \brief Sets the width of the lane.
    /// TODO currently the width is only given by one value but lanes can have changing width along their runlength.
    /// \param the new width
    ///
    void SetWidth(double w) {
        width = w;
    }

    ///
    /// \brief Returns, whether the direction of lane matches the roads direction.
    ///
    bool IsInRoadDirection() const
    {
        return inRoadDirection;
    }

    ///
    /// \brief Returns the type of the lane.
    ///
    LaneType GetType() const
    {
        return type;
    }

    ///
    /// \brief Sets the Road that this lane is on.
    /// \param r the new section
    ///
    void SetRoad(const Road *r) {
        road = r;
    }

    ///
    /// \brief Returns the road this lane is on.
    ///
    const Road *GetRoad() const {
        return road;
    }

    ///
    /// \brief Returns a list of all lane points making up the Reference lane.
    ///
    const std::list<LanePoint> &GetLanePoints() const
    {
        return lanePointsReference;
    }

    ///
    /// \brief Returns a list of all lane points making up the left side of this lane.
    ///
    const std::list<LanePoint> &GetLeftSidePoints() const
    {
        return lanePointsLeftSide;
    }

    ///
    /// \brief Returns a list of all lane points making up the right side of this lane.
    ///
    const std::list<LanePoint> &GetRightSidePoints() const
    {
        return lanePointsRightSide;
    }

    ///
    /// \brief Returns the last LanePoint of this lane.
    ///
    const LanePoint *GetLastPoint() const;

    ///
    /// \brief Returns the first LanePoint of this Lane.
    ///
    const LanePoint *GetFirstPoint() const;

    ///
    /// \brief Adds a left point for the lane.
    /// \param x x-coordinate of the point
    /// \param y y-coordinate of the point
    /// \param hdg heading at the point
    /// \param so s-Offset at the point
    /// \param inDirection true if Lane in directio of Road
    ///
    void AddLeftPoint(double x, double y, double hdg, double so, bool inDirection);

    ///
    /// \brief Adds a right point for the lane.
    /// \param x x-coordinate of the point
    /// \param y y-coordinate of the point
    /// \param hdg heading at the point
    /// \param so s-Offset at the point
    /// \param inDirection true if Lane in directio of Road
    ///
    void AddRightPoint(double x, double y, double hdg, double so, bool inDirection);

    ///
    /// \brief Adds a reference point for the lane.
    /// \param x x-coordinate of the point
    /// \param y y-coordinate of the point
    /// \param hdg heading at the point
    /// \param so s-Offset at the point
    /// \param inDirection true if Lane in directio of Road
    ///
    void AddReferencePoint(double x, double y, double hdg, double so, bool inDirection);

    ///
    /// \brief Adds a predecessor lane for this lane.
    ///
    void AddPredecessor(const Lane *lane);

    ///
    /// \brief Returns all predecessors for this lane.
    ///
    const std::vector<const Lane *> &GetPredecessors() const
    {
        return predecessors;
    }

    ///
    /// \brief Returns whether the lane has any predecessors.
    ///
    bool HasPredecessors() const
    {
        return predecessors.size() != 0;
    }

    ///
    /// \brief Adds a successor lane for this lane.
    ///
    void AddSuccessor(const Lane *lane);

    ///
    /// \brief Returns all successors for this lane.
    ///
    const std::vector<const Lane *> &GetSuccessors() const
    {
        return successors;
    }

    ///
    /// \brief Returns whether the lane has any successors.
    ///
    bool HasSuccessors() const
    {
        return predecessors.size() != 0;
    }

    ///
    /// \brief Interpolates a LanePoint at the given s-coordinate.
    /// \param sLaneCoordinate the s-coordinate to interpolate the point at
    /// \return the resulting LanePoint
    ///
    LanePoint InterpolatePoint(double sCoordinate) const;

    ///
    ///\brief return the conflict area with a chosen lane
    ///
    ///\param  lane
    ///\return conflict area
    ///
    std::optional<ConflictArea> GetConflictAreaWithLane(const Lane *lane) const;

    ///
    /// \brief Adds a conflict area for this lane.
    ///
    void AddConflictArea(std::pair<const Lane *, ConflictArea> conflictArea)
    {
        conflictAreas.insert(conflictArea);
    }

    ///
    /// \brief Adds left neighbor lane to lane
    ///
    void AddLeftLane(const Lane *inleftLane) {
        leftLane = inleftLane;
    }

    ///
    /// \brief Adds right neighbor lane to lane
    ///
    void AddRightLane(const Lane *inrightLane) {
        rightLane = inrightLane;
    }

    ///
    /// \brief Get left neighbor lane to lane
    ///
    const Lane *GetLeftLane() const {
        return leftLane;
    }

    ///
    /// \brief Get right neighbor lane to lane
    ///
    const Lane *GetRightLane() const {
        return rightLane;
    }

private:
    bool SLaneCoordinateOutOfLane(double sLane_coordniate) const;

    OwlId owlId;
    double length;
    double width;
    LaneType type;
    bool inRoadDirection;

    const Road *road;
    std::list<LanePoint> lanePointsLeftSide;
    std::list<LanePoint> lanePointsReference;
    std::list<LanePoint> lanePointsRightSide;

    std::unordered_map<const Lane *, ConflictArea> conflictAreas;

    std::vector<const Lane *> predecessors;
    std::vector<const Lane *> successors;
    const Lane *leftLane = nullptr;
    const Lane *rightLane = nullptr;

    SpeedLimit speedLimit{maxDouble};
};
} // namespace MentalInfrastructure
