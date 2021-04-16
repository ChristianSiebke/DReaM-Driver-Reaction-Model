/*******************************************************************************
* Copyright (c) 2018, 2019, 2020, 2021 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  DataTypes.h
//! @brief This file provides the basic datatypes of the osi world layer (OWL)
//-----------------------------------------------------------------------------

#include <exception>
#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <qglobal.h>

#include "include/roadInterface/roadInterface.h"
#include "include/roadInterface/roadLaneInterface.h"
#include "include/roadInterface/roadLaneSectionInterface.h"

#include "OWL/DataTypes.h"
#include "OWL/LaneGeometryElement.h"
#include "OWL/LaneGeometryJoint.h"
#include "OWL/Primitives.h"

#include "osi3/osi_groundtruth.pb.h"
#include "WorldObjectAdapter.h"

namespace OWL {

template <typename OsiObject>
Primitive::Dimension GetDimensionFromOsiObject(const OsiObject& osiObject)
{
    const auto& d= osiObject->base().dimension();
    return { d.length(), d.width(), d.height() };
}

namespace Implementation {

Lane::Lane(osi3::Lane* osiLane, const Interfaces::Section* section) :
    osiLane(osiLane),
    section(section),
    leftLane(section ? new InvalidLane() : nullptr),
    rightLane(section ? new InvalidLane() : nullptr)
{
}

Lane::~Lane()
{
    for (auto laneGeometryElement : laneGeometryElements)
    {
        delete laneGeometryElement;
    }
    if (leftLaneIsDummy)
    {
        delete leftLane;
    }
    if (rightLaneIsDummy)
    {
        delete rightLane;
    }
}

void Lane::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newLane = target.add_lane();
    newLane->CopyFrom(*osiLane);
}

Id Lane::GetId() const
{
    return osiLane->id().value();
}

bool Lane::Exists() const
{
    return osiLane->id().value() != InvalidId;
}

const Interfaces::Section& Lane::GetSection() const
{
    return *section;
}

const Interfaces::Road& Lane::GetRoad() const
{
    return section->GetRoad();
}

int Lane::GetRightLaneCount() const
{
    int count = 0;
    const Interfaces::Lane* currentLane = &(GetRightLane());

    while (currentLane->Exists())
    {
        count++;
        currentLane = &(currentLane->GetRightLane());
    }

    return count;
}

double Lane::GetLength() const
{
    return length;
}


std::tuple<const Primitive::LaneGeometryJoint*, const Primitive::LaneGeometryJoint*> Lane::GetNeighbouringJoints(
        double distance) const
{
    const Primitive::LaneGeometryJoint* nextJoint = nullptr;
    const Primitive::LaneGeometryJoint* prevJoint = nullptr;

    const auto& nextJointIt = std::find_if(laneGeometryJoints.cbegin(),
                                           laneGeometryJoints.cend(),
                                           [distance](const Primitive::LaneGeometryJoint & joint)
    {
        return joint.sOffset > distance;
    });


    if (nextJointIt != laneGeometryJoints.cend())
    {
        nextJoint = &(*nextJointIt);
    }

    if (nextJointIt != laneGeometryJoints.cbegin())
    {
        const auto& prevJointIt = std::prev(nextJointIt);
        prevJoint = &(*prevJointIt);
    }

    return { prevJoint, nextJoint };
}

const Primitive::LaneGeometryJoint::Points  Lane::GetInterpolatedPointsAtDistance(double distance) const
{
    Primitive::LaneGeometryJoint::Points interpolatedPoints {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
    const Primitive::LaneGeometryJoint* prevJoint;
    const Primitive::LaneGeometryJoint* nextJoint;
    std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

    if (!prevJoint && !nextJoint)
    {
        return interpolatedPoints;
    }
    else
        if (prevJoint && !nextJoint)
        {
            return prevJoint->points;
        }
        else
            if (nextJoint && !prevJoint)
            {
                return nextJoint->points;
            }

    double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                    prevJoint->sOffset);

    interpolatedPoints.left = prevJoint->points.left * (1.0 - interpolationFactor) + nextJoint->points.left *
            interpolationFactor;
    interpolatedPoints.right = prevJoint->points.right * (1.0 - interpolationFactor) + nextJoint->points.right *
            interpolationFactor;
    interpolatedPoints.reference = prevJoint->points.reference * (1.0 - interpolationFactor) + nextJoint->points.reference *
            interpolationFactor;

    return interpolatedPoints;
}

double Lane::GetCurvature(double distance) const
{
    const Primitive::LaneGeometryJoint* prevJoint;
    const Primitive::LaneGeometryJoint* nextJoint;
    std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

    if (!prevJoint && !nextJoint)
    {
        return 0.0;
    }
    else
        if (prevJoint && !nextJoint)
        {
            return prevJoint->curvature;
        }
        else
            if (nextJoint && !prevJoint)
            {
                return 0.0;
            }

    double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                    prevJoint->sOffset);
    double interpolatedCurvature = (1 - interpolationFactor) * prevJoint->curvature + interpolationFactor *
            nextJoint->curvature;

    return interpolatedCurvature;
}

double Lane::GetWidth(double distance) const
{
    const Primitive::LaneGeometryJoint* prevJoint;
    const Primitive::LaneGeometryJoint* nextJoint;
    std::tie(prevJoint, nextJoint) = GetNeighbouringJoints(distance);

    if (!prevJoint && !nextJoint)
    {
        return 0.0;
    }
    else
        if (prevJoint && !nextJoint)
        {
            return (prevJoint->points.left - prevJoint->points.right).Length();
        }
        else
            if (nextJoint && !prevJoint)
            {
                return 0.0;
            }

    double interpolationFactor = (distance - prevJoint->sOffset) / (nextJoint->sOffset -
                                                                    prevJoint->sOffset);
    double nextWidth = (nextJoint->points.left - nextJoint->points.right).Length();
    double prevWidth = (prevJoint->points.left - prevJoint->points.right).Length();
    double interpolatedWidth = (1.0 - interpolationFactor) * prevWidth + interpolationFactor * nextWidth;

    return interpolatedWidth;
}

double Lane::GetDirection(double distance) const
{
    auto [prevJoint, nextJoint] = GetNeighbouringJoints(distance);

    if (!prevJoint)
    {
        return 0.0;
    }

    return prevJoint->sHdg;
}

const Interfaces::Lane& Lane::GetLeftLane() const
{
    return *leftLane;
}

const Interfaces::Lane& Lane::GetRightLane() const
{
    return *rightLane;
}

const std::vector<Id> Lane::GetLeftLaneBoundaries() const
{
    std::vector<Id> laneBoundaries;
    for(const auto& laneBoundary : osiLane->classification().left_lane_boundary_id())
    {
        laneBoundaries.push_back(laneBoundary.value());
    }
    return laneBoundaries;
}

const std::vector<Id> Lane::GetRightLaneBoundaries() const
{
    std::vector<Id> laneBoundaries;
    for(const auto& laneBoundary : osiLane->classification().right_lane_boundary_id())
    {
        laneBoundaries.push_back(laneBoundary.value());
    }
    return laneBoundaries;
}

double Lane::GetDistance(MeasurementPoint measurementPoint) const
{
    if (laneGeometryElements.empty())
    {
        throw std::runtime_error("Unexpected Lane without LaneGeometryElements");
    }

    if (measurementPoint == MeasurementPoint::RoadStart)
    {
        return laneGeometryElements.front()->joints.current.sOffset;
    }

    if (measurementPoint == MeasurementPoint::RoadEnd)
    {
        return laneGeometryElements.back()->joints.next.sOffset;
    }

    throw std::invalid_argument("measurement point not within valid bounds");
}

LaneType Lane::GetLaneType() const
{
    return laneType;
}

bool Lane::Covers(double distance) const
{
    if (GetDistance(MeasurementPoint::RoadStart) <= distance)
    {
        return next.empty() ?
                    GetDistance(MeasurementPoint::RoadEnd) > distance :
                    GetDistance(MeasurementPoint::RoadEnd) >= distance;

    }
    return false;
}

void Lane::SetLeftLane(const Interfaces::Lane& lane, bool transferLaneBoundary)
{
    if (leftLaneIsDummy)
    {
        leftLaneIsDummy = false;
        delete leftLane;
    }

    leftLane = &lane;
    osiLane->mutable_classification()->add_left_adjacent_lane_id()->set_value(lane.GetId());

    if (transferLaneBoundary)
    {
        for (const auto& laneBoundary : lane.GetRightLaneBoundaries())
        {
            osiLane->mutable_classification()->add_left_lane_boundary_id()->set_value(laneBoundary);
        }
    }
}

void Lane::SetRightLane(const Interfaces::Lane& lane, bool transferLaneBoundary)
{
    if (rightLaneIsDummy)
    {
        rightLaneIsDummy = false;
        delete rightLane;
    }

    rightLane = &lane;
    osiLane->mutable_classification()->add_right_adjacent_lane_id()->set_value(lane.GetId());
    if (transferLaneBoundary)
    {
        for (const auto& laneBoundary : lane.GetLeftLaneBoundaries())
        {
            osiLane->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
        }
    }
}

void Lane::SetLeftLaneBoundaries(const std::vector<Id> laneBoundaries)
{
    for(const auto& laneBoundary : laneBoundaries)
    {
        osiLane->mutable_classification()->add_left_lane_boundary_id()->set_value(laneBoundary);
    }
}

void Lane::SetRightLaneBoundaries(const std::vector<Id> laneBoundaries)
{
    for(const auto& laneBoundary : laneBoundaries)
    {
        osiLane->mutable_classification()->add_right_lane_boundary_id()->set_value(laneBoundary);
    }
}

void Lane::SetLaneType(LaneType specifiedType)
{
    laneType =  specifiedType;
}

const Interfaces::LaneAssignments& Lane::GetWorldObjects(bool direction) const
{
    return worldObjects.Get(direction);
}

const Interfaces::TrafficSigns &Lane::GetTrafficSigns() const
{
    return trafficSigns;
}

const Interfaces::RoadMarkings& Lane::GetRoadMarkings() const
{
    return roadMarkings;
}

void Lane::AddMovingObject(Interfaces::MovingObject& movingObject, const LaneOverlap& laneOverlap)
{
    worldObjects.Insert(laneOverlap, &movingObject);
}

void Lane::AddStationaryObject(Interfaces::StationaryObject& stationaryObject, const LaneOverlap& laneOverlap)
{
    worldObjects.Insert(laneOverlap, &stationaryObject);
    stationaryObjects.push_back({laneOverlap, &stationaryObject});
}

void Lane::AddWorldObject(Interfaces::WorldObject& worldObject, const LaneOverlap& laneOverlap)
{
    if (worldObject.Is<MovingObject>())
    {
        AddMovingObject(*worldObject.As<MovingObject>(), laneOverlap);
    }
    else
        if (worldObject.Is<StationaryObject>())
        {
            AddStationaryObject(*worldObject.As<StationaryObject>(), laneOverlap);
        }
}

void Lane::AddTrafficSign(Interfaces::TrafficSign& trafficSign)
{
    trafficSigns.push_back(&trafficSign);
}

void Lane::AddRoadMarking(Interfaces::RoadMarking& roadMarking)
{
    roadMarkings.push_back(&roadMarking);
}

void Lane::ClearMovingObjects()
{
    worldObjects.Clear();
    for (const auto& [laneOverlap, object] : stationaryObjects)
    {
        worldObjects.Insert(laneOverlap, object);
    }
}

void Lane::AddNext(const Interfaces::Lane* lane)
{
    next.push_back(lane->GetId());
    auto lanePairing = osiLane->mutable_classification()->add_lane_pairing();
    lanePairing->mutable_antecessor_lane_id()->set_value(GetId());
    lanePairing->mutable_successor_lane_id()->set_value(lane->GetId());
}

void Lane::AddPrevious(const Interfaces::Lane* lane)
{
    previous.push_back(lane->GetId());
    auto lanePairing = osiLane->mutable_classification()->add_lane_pairing();
    lanePairing->mutable_antecessor_lane_id()->set_value(lane->GetId());
    lanePairing->mutable_successor_lane_id()->set_value(GetId());
}

const Interfaces::LaneGeometryElements& Lane::GetLaneGeometryElements() const
{
    return laneGeometryElements;
}

void Lane::AddLaneGeometryJoint(const Common::Vector2d& pointLeft,
                                const Common::Vector2d& pointCenter,
                                const Common::Vector2d& pointRight,
                                double sOffset,
                                double curvature,
                                double heading)
{
    Primitive::LaneGeometryJoint newJoint;

    newJoint.points.left      = pointLeft;
    newJoint.points.reference = pointCenter;
    newJoint.points.right     = pointRight;
    newJoint.curvature        = curvature;
    newJoint.sHdg = heading;
    newJoint.sOffset = sOffset;

    if (laneGeometryJoints.empty())
    {
        laneGeometryJoints.push_back(newJoint);
        auto osiCenterpoint = osiLane->mutable_classification()->add_centerline();
        osiCenterpoint->set_x(pointCenter.x);
        osiCenterpoint->set_y(pointCenter.y);
        return;
    }

    const Primitive::LaneGeometryJoint& previousJoint = laneGeometryJoints.back();

    if (previousJoint.sOffset >= sOffset)
    {
        return; //Do not add the same point twice
    }

    length = sOffset - laneGeometryJoints.front().sOffset;
    Primitive::LaneGeometryElement* newElement = new Primitive::LaneGeometryElement(previousJoint, newJoint, this);
    laneGeometryElements.push_back(newElement);
    laneGeometryJoints.push_back(newJoint);
    auto osiCenterpoint = osiLane->mutable_classification()->add_centerline();
    osiCenterpoint->set_x(pointCenter.x);
    osiCenterpoint->set_y(pointCenter.y);
}

Section::Section(double sOffset) :
    sOffset(sOffset)
{}

void Section::AddNext(const Interfaces::Section &section)
{
    nextSections.push_back(&section);
}

void Section::AddPrevious(const Interfaces::Section &section)
{
    previousSections.push_back(&section);
}

void Section::AddLane(const Interfaces::Lane& lane)
{
    lanes.push_back(&lane);
}

void Section::SetRoad(Interfaces::Road* road)
{
    this->road = road;
}

const Interfaces::Road& Section::GetRoad() const
{
    return *road;
}

double Section::GetDistance(MeasurementPoint measurementPoint) const
{
    if (measurementPoint == MeasurementPoint::RoadStart)
    {
        return GetSOffset();
    }

    if (measurementPoint == MeasurementPoint::RoadEnd)
    {
        return GetSOffset() + GetLength();
    }

    throw std::invalid_argument("measurement point not within valid bounds");

}

bool Section::Covers(double distance) const
{
    if (GetDistance(MeasurementPoint::RoadStart) <= distance)
    {
        return nextSections.empty() ?
                    GetDistance(MeasurementPoint::RoadEnd) >= distance :
                    GetDistance(MeasurementPoint::RoadEnd) > distance;
    }
    return false;
}

bool Section::CoversInterval(double startDistance, double endDistance) const
{
    double sectionStart = GetDistance(MeasurementPoint::RoadStart);
    double sectionEnd = GetDistance(MeasurementPoint::RoadEnd);

    bool startDistanceSmallerSectionEnd = nextSections.empty() ?
                startDistance <= sectionEnd
              : startDistance < sectionEnd;
    bool endDistanceGreaterSectionStart = previousSections.empty() ? endDistance >= sectionStart
                                                                   : endDistance > sectionStart;

    return startDistanceSmallerSectionEnd && endDistanceGreaterSectionStart;
}

const Interfaces::Lanes& Section::GetLanes() const
{
    return lanes;
}

double Section::GetLength() const
{
    double sum = 0.0;

    if (lanes.empty())
    {
        return 0.0;
    }

    //return osiSection->length();
    for (const auto& lane : lanes)
    {
        sum += lane->GetLength();
    }

    return sum / lanes.size();
}

void Section::SetCenterLaneBoundary(std::vector<Id> laneBoundaryId)
{
    centerLaneBoundary = laneBoundaryId;
}

std::vector<Id> Section::GetCenterLaneBoundary() const
{
    return centerLaneBoundary;
}

double Section::GetSOffset() const
{
    return sOffset;
}


Road::Road(bool isInStreamDirection, const std::string& id) :
    isInStreamDirection(isInStreamDirection),
    id(id)
{
}

const std::string& Road::GetId() const
{
    return id;
}

const Interfaces::Sections& Road::GetSections() const
{
    return sections;
}

void Road::AddSection(Interfaces::Section& section)
{
    section.SetRoad(this);
    sections.push_back(&section);
}

double Road::GetLength() const
{
    double length = 0;
    for (const auto& section : sections)
    {
        length += section->GetLength();
    }
    return length;
}

const std::string& Road::GetSuccessor() const
{
    return successor;
}

const std::string& Road::GetPredecessor() const
{
    return predecessor;
}

void Road::SetSuccessor(const std::string& successor)
{
    this->successor = successor;
}

void Road::SetPredecessor(const std::string& predecessor)
{
    this->predecessor = predecessor;
}

bool Road::IsInStreamDirection() const
{
    return isInStreamDirection;
}

double Road::GetDistance(MeasurementPoint mp) const
{
    if (mp == MeasurementPoint::RoadStart)
    {
        return 0;
    }
    else
    {
        return GetLength();
    }
}

StationaryObject::StationaryObject(osi3::StationaryObject* osiStationaryObject, void* linkedObject) :
    osiObject{osiStationaryObject}
{
    this->linkedObject = linkedObject;
}

void StationaryObject::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newStationaryObject = target.add_stationary_object();
    newStationaryObject->CopyFrom(*osiObject);
}

Primitive::Dimension StationaryObject::GetDimension() const
{
    return GetDimensionFromOsiObject(osiObject);
}

Primitive::AbsOrientation StationaryObject::GetAbsOrientation() const
{
    osi3::Orientation3d osiOrientation = osiObject->base().orientation();
    Primitive::AbsOrientation orientation;

    orientation.yaw = osiOrientation.yaw();
    orientation.pitch = osiOrientation.pitch();
    orientation.roll = osiOrientation.roll();

    return orientation;
}

Primitive::AbsPosition StationaryObject::GetReferencePointPosition() const
{
    const osi3::Vector3d osiPosition = osiObject->base().position();
    Primitive::AbsPosition position;

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

double StationaryObject::GetAbsVelocityDouble() const
{
    return 0.0;
}

double StationaryObject::GetAbsAccelerationDouble() const
{
    return 0.0;
}

const ObjectPosition& StationaryObject::GetLocatedPosition() const
{
    return position;
}

Id StationaryObject::GetId() const
{
    return osiObject->id().value();
}

void StationaryObject::SetReferencePointPosition(const Primitive::AbsPosition& newPosition)
{
    osi3::Vector3d* osiPosition = osiObject->mutable_base()->mutable_position();

    osiPosition->set_x(newPosition.x);
    osiPosition->set_y(newPosition.y);
    osiPosition->set_z(newPosition.z);
}

void StationaryObject::SetDimension(const Primitive::Dimension& newDimension)
{
    osi3::Dimension3d* osiDimension = osiObject->mutable_base()->mutable_dimension();

    osiDimension->set_length(newDimension.length);
    osiDimension->set_width(newDimension.width);
    osiDimension->set_height(newDimension.height);
}

void StationaryObject::SetAbsOrientation(const Primitive::AbsOrientation& newOrientation)
{
    osi3::Orientation3d* osiOrientation = osiObject->mutable_base()->mutable_orientation();

    osiOrientation->set_yaw(newOrientation.yaw);
    osiOrientation->set_pitch(newOrientation.pitch);
    osiOrientation->set_roll(newOrientation.roll);
}

void StationaryObject::AddLaneAssignment(const Interfaces::Lane& lane)
{
    assignedLanes.push_back(&lane);
}

const Interfaces::Lanes& StationaryObject::GetLaneAssignments() const
{
    return assignedLanes;
}

void StationaryObject::ClearLaneAssignments()
{
    assignedLanes.clear();
}

double StationaryObject::GetDistance(MeasurementPoint measurementPoint, const std::string& roadId) const
{
    if (position.touchedRoads.count(roadId) == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    switch (measurementPoint)
    {
        case MeasurementPoint::RoadStart:
            return position.touchedRoads.at(roadId).sStart;
        case MeasurementPoint::RoadEnd:
            return position.touchedRoads.at(roadId).sEnd;
        default:
            throw std::invalid_argument("Invalid measurement point");
    }
}

void StationaryObject::SetLocatedPosition(const ObjectPosition& position)
{
    this->position = position;
}

MovingObject::MovingObject(osi3::MovingObject* osiMovingObject, void* linkedObject) :
    osiObject{osiMovingObject}
{
    this->linkedObject = linkedObject;
    SetIndicatorState(IndicatorState::IndicatorState_Off);
    SetBrakeLightState(false);
    SetHeadLight(false);
    SetHighBeamLight(false);
}

void MovingObject::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newMovingObject = target.add_moving_object();
    newMovingObject->CopyFrom(*osiObject);
}

Id MovingObject::GetId() const
{
    return osiObject->id().value();
}

Primitive::Dimension MovingObject::GetDimension() const
{
    return GetDimensionFromOsiObject(osiObject);
}

double MovingObject::GetDistanceReferencePointToLeadingEdge() const
{
    return osiObject->base().dimension().length() * 0.5 - osiObject->vehicle_attributes().bbcenter_to_rear().x();
}

double MovingObject::GetDistance(MeasurementPoint measurementPoint, const std::string& roadId) const
{
    if (position.touchedRoads.count(roadId) == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    switch (measurementPoint)
    {
        case MeasurementPoint::RoadStart:
            return position.touchedRoads.at(roadId).sStart;
        case MeasurementPoint::RoadEnd:
            return position.touchedRoads.at(roadId).sEnd;
        default:
            throw std::invalid_argument("Invalid measurement point");
    }
}

void MovingObject::SetDimension(const Primitive::Dimension& newDimension)
{
    osi3::Dimension3d* osiDimension = osiObject->mutable_base()->mutable_dimension();

    osiDimension->set_length(newDimension.length);
    osiDimension->set_width(newDimension.width);
    osiDimension->set_height(newDimension.height);
}

void MovingObject::SetLength(const double newLength)
{
    osi3::Dimension3d* osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_length(newLength);
}

void MovingObject::SetWidth(const double newWidth)
{
    osi3::Dimension3d* osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_width(newWidth);
}

void MovingObject::SetHeight(const double newHeight)
{
    osi3::Dimension3d* osiDimension = osiObject->mutable_base()->mutable_dimension();
    osiDimension->set_height(newHeight);
}

void MovingObject::SetBoundingBoxCenterToRear(const double distance)
{
    osiObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_x(distance);
}

Primitive::AbsPosition MovingObject::GetReferencePointPosition() const
{
    const osi3::Vector3d& osiPosition = osiObject->base().position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d& bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    Primitive::AbsPosition position;

    position.x = osiPosition.x() + std::cos(yaw) * bbCenterToRear.x();
    position.y = osiPosition.y() + std::sin(yaw) * bbCenterToRear.x();
    position.z = osiPosition.z();

    return position;
}

void MovingObject::SetReferencePointPosition(const Primitive::AbsPosition& newPosition)
{
    osi3::Vector3d* osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d& bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_x(newPosition.x - std::cos(yaw) * bbCenterToRear.x());
    osiPosition->set_y(newPosition.y - std::sin(yaw) * bbCenterToRear.x());
    osiPosition->set_z(newPosition.z);
    frontDistance.Invalidate();
    rearDistance.Invalidate();
}

void MovingObject::SetX(const double newX)
{
    osi3::Vector3d* osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d& bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_x(newX - std::cos(yaw) * bbCenterToRear.x());
}

void MovingObject::SetY(const double newY)
{
    osi3::Vector3d* osiPosition = osiObject->mutable_base()->mutable_position();
    const double yaw = osiObject->base().orientation().yaw();
    const osi3::Vector3d& bbCenterToRear = osiObject->vehicle_attributes().bbcenter_to_rear();

    osiPosition->set_y(newY - std::sin(yaw) * bbCenterToRear.x());
}

void MovingObject::SetZ(const double newZ)
{
    osi3::Vector3d* osiPosition = osiObject->mutable_base()->mutable_position();
    osiPosition->set_z(newZ);
}

const ObjectPosition& MovingObject::GetLocatedPosition() const
{
    return position;
}

void MovingObject::SetLocatedPosition(const ObjectPosition& position)
{
    this->position = position;
}

Primitive::AbsOrientation MovingObject::GetAbsOrientation() const
{
    osi3::Orientation3d osiOrientation = osiObject->base().orientation();
    Primitive::AbsOrientation orientation;

    orientation.yaw = osiOrientation.yaw();
    orientation.pitch = osiOrientation.pitch();
    orientation.roll = osiOrientation.roll();

    return orientation;
}

void MovingObject::SetAbsOrientation(const Primitive::AbsOrientation& newOrientation)
{
    osi3::Orientation3d* osiOrientation = osiObject->mutable_base()->mutable_orientation();
    const auto referencePosition = GetReferencePointPosition(); //AbsPosition needs to be evaluated with "old" yaw
    osiOrientation->set_yaw(newOrientation.yaw);
    osiOrientation->set_pitch(newOrientation.pitch);
    osiOrientation->set_roll(newOrientation.roll);
    frontDistance.Invalidate();
    rearDistance.Invalidate();
    SetReferencePointPosition(referencePosition); //Changing yaw also changes position of the boundingBox center
}

void MovingObject::SetYaw(const double newYaw)
{
    const auto referencePosition = GetReferencePointPosition();
    osi3::Orientation3d* osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_yaw(newYaw);
    frontDistance.Invalidate();
    rearDistance.Invalidate();
    SetReferencePointPosition(referencePosition); //Changing yaw also changes position of the boundingBox center
}

void MovingObject::SetPitch(const double newPitch)
{
    osi3::Orientation3d* osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_pitch(newPitch);
}

void MovingObject::SetRoll(const double newRoll)
{
    osi3::Orientation3d* osiOrientation = osiObject->mutable_base()->mutable_orientation();
    osiOrientation->set_roll(newRoll);
}

void MovingObject::SetIndicatorState(IndicatorState indicatorState)
{
    if (indicatorState == IndicatorState::IndicatorState_Off)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                    osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF);
        return;
    }
    else if (indicatorState == IndicatorState::IndicatorState_Left)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                    osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT);
        return;
    }
    else if (indicatorState == IndicatorState::IndicatorState_Right)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                    osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT);
        return;
    }
    else if (indicatorState == IndicatorState::IndicatorState_Warn)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_indicator_state(
                    osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING);
        return;
    }

    throw std::invalid_argument("Indicator state is not supported");
}

IndicatorState MovingObject::GetIndicatorState() const
{
    const auto& osiState = osiObject->vehicle_classification().light_state().indicator_state();

    if (osiState == osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF)
    {
        return IndicatorState::IndicatorState_Off;
    }
    else
        if (osiState == osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT)
        {
            return IndicatorState::IndicatorState_Left;
        }
        else
            if (osiState == osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT)
            {
                return IndicatorState::IndicatorState_Right;
            }
            else
                if (osiState == osi3::MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING)
                {
                    return IndicatorState::IndicatorState_Warn;
                }

    throw std::logic_error("Indicator state is not supported");
}

void MovingObject::SetBrakeLightState(bool brakeLightState)
{
    if (brakeLightState)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_brake_light_state(
                    osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL);
    }
    else
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_brake_light_state(
                    osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF);
    }
}

bool MovingObject::GetBrakeLightState() const
{
    const auto& osiState = osiObject->vehicle_classification().light_state().brake_light_state();

    if (osiState == osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL)
    {
        return true;
    }
    else
        if (osiState == osi3::MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF)
        {
            return false;
        }

    throw std::logic_error("BrakeLightState is not supported");
}

void MovingObject::SetHeadLight(bool headLight)
{
    if (headLight)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_head_light(
                    osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON);
    }
    else
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_head_light(
                    osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    }
}

bool MovingObject::GetHeadLight() const
{
    const auto& osiState = osiObject->vehicle_classification().light_state().head_light();

    if (osiState == osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON)
    {
        return true;
    }
    else
        if (osiState == osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF)
        {
            return false;
        }

    throw std::logic_error("HeadLightState is not supported");
}

void MovingObject::SetHighBeamLight(bool highbeamLight)
{
    if (highbeamLight)
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_high_beam(
                    osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON);
    }
    else
    {
        osiObject->mutable_vehicle_classification()->mutable_light_state()->set_high_beam(
                    osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);
    }
}

bool MovingObject::GetHighBeamLight() const
{
    const auto& osiState = osiObject->vehicle_classification().light_state().high_beam();

    if (osiState == osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON)
    {
        return true;
    }
    else
        if (osiState == osi3::MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF)
        {
            return false;
        }

    throw std::logic_error("HighBeamLightState is not supported");
}

void MovingObject::SetType(AgentVehicleType type)
{
    if(type == AgentVehicleType::Pedestrian)
    {
        osiObject->set_type(osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN);
    }
    else
    {
        osiObject->set_type(osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE);

        switch(type) 
        {
            case AgentVehicleType::Car:
            osiObject->mutable_vehicle_classification()->set_type(osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MEDIUM_CAR);
            break;
            
            case AgentVehicleType::Motorbike:
            osiObject->mutable_vehicle_classification()->set_type(osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MOTORBIKE);
            break;

            case AgentVehicleType::Bicycle:
            osiObject->mutable_vehicle_classification()->set_type(osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_BICYCLE);
            break;

            case AgentVehicleType::Truck:
            osiObject->mutable_vehicle_classification()->set_type(osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_HEAVY_TRUCK);
            break;

            default:
            throw(std::runtime_error("AgentVehicleType not supported"));
        }       
    }
}

Primitive::LaneOrientation MovingObject::GetLaneOrientation() const
{
    throw std::logic_error("MovingObject::GetLaneOrientation not implemented");
}

Primitive::AbsVelocity MovingObject::GetAbsVelocity() const
{
    const osi3::Vector3d osiVelocity = osiObject->base().velocity();
    Primitive::AbsVelocity velocity;

    velocity.vx = osiVelocity.x();
    velocity.vy = osiVelocity.y();
    velocity.vz = osiVelocity.z();

    return velocity;
}

double MovingObject::GetAbsVelocityDouble() const
{
    Primitive::AbsVelocity velocity = GetAbsVelocity();
    Primitive::AbsOrientation orientation = GetAbsOrientation();
    double sign = 1.0;

    double velocityAngle = std::atan2(velocity.vy, velocity.vx);

    if (std::abs(velocity.vx) == 0.0 && std::abs(velocity.vy) == 0.0)
    {
        velocityAngle = 0.0;
    }

    double angleBetween = velocityAngle - orientation.yaw;

    if (std::abs(angleBetween) > M_PI_2 && std::abs(angleBetween) < 3 * M_PI_2)
    {
        sign = -1.0;
    }

    return std::hypot(velocity.vx, velocity.vy) * sign;
}

void MovingObject::SetAbsVelocity(const Primitive::AbsVelocity& newVelocity)
{
    osi3::Vector3d* osiVelocity = osiObject->mutable_base()->mutable_velocity();

    osiVelocity->set_x(newVelocity.vx);
    osiVelocity->set_y(newVelocity.vy);
    osiVelocity->set_z(newVelocity.vz);
}

void MovingObject::SetAbsVelocity(const double newVelocity)
{
    osi3::Vector3d* osiVelocity = osiObject->mutable_base()->mutable_velocity();

    double yaw = GetAbsOrientation().yaw;
    double cos_val = std::cos(yaw);
    double sin_val = std::sin(yaw);

    osiVelocity->set_x(newVelocity * cos_val);
    osiVelocity->set_y(newVelocity * sin_val);
    osiVelocity->set_z(0.0);
}

Primitive::AbsAcceleration MovingObject::GetAbsAcceleration() const
{
    const osi3::Vector3d osiAcceleration = osiObject->base().acceleration();
    Primitive::AbsAcceleration acceleration;

    acceleration.ax = osiAcceleration.x();
    acceleration.ay = osiAcceleration.y();
    acceleration.az = osiAcceleration.z();

    return acceleration;
}

double MovingObject::GetAbsAccelerationDouble() const
{
    Primitive::AbsAcceleration acceleration = GetAbsAcceleration();
    Primitive::AbsOrientation orientation = GetAbsOrientation();
    double sign = 1.0;

    double accAngle = std::atan2(acceleration.ay, acceleration.ax);

    if (std::abs(acceleration.ax) == 0.0 && std::abs(acceleration.ay) == 0.0)
    {
        accAngle = 0.0;
    }

    double angleBetween = accAngle - orientation.yaw;

    if ((std::abs(angleBetween) - M_PI_2) > 0)
    {
        sign = -1.0;
    }

    return std::hypot(acceleration.ax, acceleration.ay) * sign;
}

void MovingObject::SetAbsAcceleration(const Primitive::AbsAcceleration& newAcceleration)
{
    osi3::Vector3d* osiAcceleration = osiObject->mutable_base()->mutable_acceleration();

    osiAcceleration->set_x(newAcceleration.ax);
    osiAcceleration->set_y(newAcceleration.ay);
    osiAcceleration->set_z(newAcceleration.az);
}

void MovingObject::SetAbsAcceleration(const double newAcceleration)
{
    osi3::Vector3d* osiAcceleration = osiObject->mutable_base()->mutable_acceleration();

    double yaw = GetAbsOrientation().yaw;
    double cos_val = std::cos(yaw);
    double sin_val = std::sin(yaw);

    osiAcceleration->set_x(newAcceleration * cos_val);
    osiAcceleration->set_y(newAcceleration * sin_val);
    osiAcceleration->set_z(0.0);
}

Primitive::AbsOrientationRate MovingObject::GetAbsOrientationRate() const
{
    const osi3::Orientation3d osiOrientationRate = osiObject->base().orientation_rate();

    return Primitive::AbsOrientationRate
    {
        osiOrientationRate.yaw(),
                osiOrientationRate.pitch(),
                osiOrientationRate.roll()
    };
}

void MovingObject::SetAbsOrientationRate(const Primitive::AbsOrientationRate& newOrientationRate)
{
    osi3::Orientation3d* osiOrientationRate = osiObject->mutable_base()->mutable_orientation_rate();

    osiOrientationRate->set_yaw(newOrientationRate.yawRate);
    osiOrientationRate->set_pitch(newOrientationRate.pitchRate);
    osiOrientationRate->set_roll(newOrientationRate.rollRate);
}

void MovingObject::AddLaneAssignment(const Interfaces::Lane& lane)
{
    osi3::Identifier* assignedId = osiObject->add_assigned_lane_id();
    assignedId->set_value(lane.GetId());
    assignedLanes.push_back(&lane);
}

const Interfaces::Lanes& MovingObject::GetLaneAssignments() const
{
    return assignedLanes;
}

void MovingObject::ClearLaneAssignments()
{
    osiObject->mutable_assigned_lane_id()->Clear();
    assignedLanes.clear();
}

Angle Vehicle::GetSteeringWheelAngle()
{
    throw std::logic_error("Vehicle::GetSteeringWheelAngle not implemented");
}

void Vehicle::SetSteeringWheelAngle(const Angle newValue)
{
    Q_UNUSED(newValue);
    throw std::logic_error("Vehicle::SetSteeringWheelAngle not implemented");
}

InvalidLane::~InvalidLane()
{
    if (osiLane)
    {
        delete osiLane;
    }
}


TrafficSign::TrafficSign(osi3::TrafficSign* osiObject) : osiSign{osiObject}
{
}

bool TrafficSign::SetSpecification(RoadSignalInterface* signal, Position position)
{
    bool mapping_succeeded = true;

    const auto baseStationary = osiSign->mutable_main_sign()->mutable_base();

    baseStationary->mutable_position()->set_x(position.xPos);
    baseStationary->mutable_position()->set_y(position.yPos);
    baseStationary->mutable_position()->set_z(signal->GetZOffset() + 0.5 * signal->GetHeight());
    baseStationary->mutable_dimension()->set_width(signal->GetWidth());
    baseStationary->mutable_dimension()->set_height(signal->GetHeight());
    double yaw = position.yawAngle + signal->GetHOffset() + (signal->GetOrientation() ? 0 : M_PI);
    yaw = CommonHelper::SetAngleToValidRange(yaw);
    baseStationary->mutable_orientation()->set_yaw(yaw);

    const auto mutableOsiClassification = osiSign->mutable_main_sign()->mutable_classification();
    const std::string odType = signal->GetType();

    if(OpenDriveTypeMapper::trafficSignsTypeOnly.find(odType) != OpenDriveTypeMapper::trafficSignsTypeOnly.end())
    {
        mutableOsiClassification->set_type(OpenDriveTypeMapper::trafficSignsTypeOnly.at(odType));
    }
    else if(OpenDriveTypeMapper::trafficSignsWithText.find(odType) != OpenDriveTypeMapper::trafficSignsWithText.end())
    {
        mutableOsiClassification->set_type(OpenDriveTypeMapper::trafficSignsWithText.at(odType));
        mutableOsiClassification->mutable_value()->set_text(signal->GetText());
    }
    else if(OpenDriveTypeMapper::trafficSignsPredefinedValueAndUnit.find(odType) != OpenDriveTypeMapper::trafficSignsPredefinedValueAndUnit.end())
    {
        const auto &specification = OpenDriveTypeMapper::trafficSignsPredefinedValueAndUnit.at(odType);

        mutableOsiClassification->mutable_value()->set_value_unit(specification.first);

        const auto subType = signal->GetSubType();
        if(specification.second.find(subType) != specification.second.end())
        {
            const auto &valueAndType = specification.second.at(subType);

            mutableOsiClassification->mutable_value()->set_value(valueAndType.first);
            mutableOsiClassification->set_type(valueAndType.second);
        }
        else
        {
            mutableOsiClassification->set_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
            mapping_succeeded = false;
        }
    }
    else if(OpenDriveTypeMapper::trafficSignsSubTypeDefinesValue.find(odType) != OpenDriveTypeMapper::trafficSignsSubTypeDefinesValue.end())
    {
        const auto &valueAndUnit = OpenDriveTypeMapper::trafficSignsSubTypeDefinesValue.at(odType);

        mutableOsiClassification->set_type(valueAndUnit.first);

        const double value = std::stoi(signal->GetSubType());
        mutableOsiClassification->mutable_value()->set_value(value);
        mutableOsiClassification->mutable_value()->set_value_unit(valueAndUnit.second);
    }
    else
    {
        mutableOsiClassification->set_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
        mapping_succeeded = false;
    }

    return mapping_succeeded;
}

bool TrafficSign::AddSupplementarySign(RoadSignalInterface* odSignal, Position position)
{
    auto *supplementarySign = osiSign->add_supplementary_sign();

    const auto baseStationary = supplementarySign->mutable_base();

    baseStationary->mutable_position()->set_x(position.xPos);
    baseStationary->mutable_position()->set_y(position.yPos);
    baseStationary->mutable_position()->set_z(odSignal->GetZOffset() + 0.5 * odSignal->GetHeight());
    baseStationary->mutable_dimension()->set_width(odSignal->GetWidth());
    baseStationary->mutable_dimension()->set_height(odSignal->GetHeight());
    double yaw = position.yawAngle + odSignal->GetHOffset() + (odSignal->GetOrientation() ? 0 : M_PI);
    yaw = CommonHelper::SetAngleToValidRange(yaw);
    baseStationary->mutable_orientation()->set_yaw(yaw);

    if (odSignal->GetType() == "1004")
    {
        supplementarySign->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE);
        auto osiValue = supplementarySign->mutable_classification()->mutable_value()->Add();

        if (odSignal->GetSubType() == "30")
        {
            osiValue->set_value(odSignal->GetValue());
            osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER);
            osiValue->set_text(std::to_string(odSignal->GetValue()) + " m");
            return true;
        }
        else if (odSignal->GetSubType() == "31")
        {
            osiValue->set_value(odSignal->GetValue());
            osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER);
            osiValue->set_text(std::to_string(odSignal->GetValue()) + " km");
            return true;
        }
        else if (odSignal->GetSubType() == "32")
        {
            osiValue->set_value(100.0);
            osiValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER);
            osiValue->set_text("STOP 100 m");
            return true;
        }
        else
        {
            supplementarySign->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_OTHER);
            return false;
        }
    }
    else
    {
        supplementarySign->mutable_classification()->set_type(osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_OTHER);
        return false;
    }
}

std::pair<double, CommonTrafficSign::Unit> TrafficSign::GetValueAndUnitInSI(const double osiValue, const osi3::TrafficSignValue_Unit osiUnit) const
{
    if (OpenDriveTypeMapper::unitConversionMap.find(osiUnit) != OpenDriveTypeMapper::unitConversionMap.end())
    {
        const auto& conversionParameters = OpenDriveTypeMapper::unitConversionMap.at(osiUnit);

        return {osiValue * conversionParameters.first, conversionParameters.second};
    }
    else
    {
        return {osiValue, CommonTrafficSign::Unit::None};
    }
}


CommonTrafficSign::Entity TrafficSign::GetSpecification(const double relativeDistance) const
{
    CommonTrafficSign::Entity specification;

    specification.distanceToStartOfRoad = s;
    specification.relativeDistance = relativeDistance;

    const auto osiType = osiSign->main_sign().classification().type();

    if (OpenDriveTypeMapper::typeConversionMap.find(osiType) != OpenDriveTypeMapper::typeConversionMap.end())
    {
        specification.type = OpenDriveTypeMapper::typeConversionMap.at(osiType);
    }

    const auto& osiMainSign = osiSign->main_sign().classification().value();
    const auto valueAndUnit = GetValueAndUnitInSI(osiMainSign.value(), osiMainSign.value_unit());

    specification.value = valueAndUnit.first;
    specification.unit = valueAndUnit.second;
    specification.text = osiSign->main_sign().classification().value().text();

    std::transform(osiSign->supplementary_sign().cbegin(),
                   osiSign->supplementary_sign().cend(),
                   std::back_inserter(specification.supplementarySigns),
                   [this, relativeDistance](const auto& osiSupplementarySign) {
        CommonTrafficSign::Entity supplementarySignSpecification;
        supplementarySignSpecification.distanceToStartOfRoad = s;
        supplementarySignSpecification.relativeDistance = s - relativeDistance;

        if (osiSupplementarySign.classification().type() == osi3::TrafficSign_SupplementarySign_Classification_Type::TrafficSign_SupplementarySign_Classification_Type_TYPE_SPACE)
        {
            const auto& osiSupplementarySignValueStruct = osiSupplementarySign.classification().value().Get(0);
            const auto supplementarySignValueAndUnit = GetValueAndUnitInSI(osiSupplementarySignValueStruct.value(), osiSupplementarySignValueStruct.value_unit());
            supplementarySignSpecification.type = CommonTrafficSign::Type::DistanceIndication;
            supplementarySignSpecification.value = supplementarySignValueAndUnit.first;
            supplementarySignSpecification.unit = supplementarySignValueAndUnit.second;
            supplementarySignSpecification.text = osiSupplementarySignValueStruct.text();
        }

        return supplementarySignSpecification;
    });

    return specification;
}

Primitive::AbsPosition TrafficSign::GetReferencePointPosition() const
{
    const osi3::Vector3d osiPosition = osiSign->main_sign().base().position();
    Primitive::AbsPosition position;

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

Primitive::Dimension TrafficSign::GetDimension() const
{
    return GetDimensionFromOsiObject(&osiSign->main_sign());
}

bool TrafficSign::IsValidForLane(OWL::Id laneId) const
{
    auto assignedLanes = osiSign->main_sign().classification().assigned_lane_id();
    for (auto lane : assignedLanes)
    {
        if (lane.value() == laneId)
        {
            return true;
        }
    }
    return false;
}

void TrafficSign::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newTrafficSign = target.add_traffic_sign();
    newTrafficSign->CopyFrom(*osiSign);
}

RoadMarking::RoadMarking(osi3::RoadMarking* osiObject) : osiSign{osiObject}
{
}

bool RoadMarking::SetSpecification(RoadSignalInterface* signal, Position position)
{
    bool mapping_succeeded = true;

    const auto baseStationary = osiSign->mutable_base();

    baseStationary->mutable_position()->set_x(position.xPos);
    baseStationary->mutable_position()->set_y(position.yPos);
    baseStationary->mutable_position()->set_z(0.0);
    baseStationary->mutable_dimension()->set_width(signal->GetWidth());
    baseStationary->mutable_dimension()->set_length(0.5);
    double yaw = position.yawAngle + signal->GetHOffset() + (signal->GetOrientation() ? 0 : M_PI);
    yaw = CommonHelper::SetAngleToValidRange(yaw);
    baseStationary->mutable_orientation()->set_yaw(yaw);

    const auto mutableOsiClassification = osiSign->mutable_classification();
    const std::string odType = signal->GetType();

    mutableOsiClassification->set_type(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN);
    mutableOsiClassification->set_monochrome_color(osi3::RoadMarking_Classification_Color::RoadMarking_Classification_Color_COLOR_WHITE);
    if(OpenDriveTypeMapper::roadMarkings.find(odType) != OpenDriveTypeMapper::roadMarkings.end())
    {
        mutableOsiClassification->set_traffic_main_sign_type(OpenDriveTypeMapper::roadMarkings.at(odType));
    }
    else
    {
        mutableOsiClassification->set_traffic_main_sign_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
        mapping_succeeded = false;
    }

    return mapping_succeeded;
}

bool RoadMarking::SetSpecification(RoadObjectInterface* object, Position position)
{
    bool mapping_succeeded = true;

    const auto baseStationary = osiSign->mutable_base();

    baseStationary->mutable_position()->set_x(position.xPos);
    baseStationary->mutable_position()->set_y(position.yPos);
    baseStationary->mutable_position()->set_z(0.0);
    baseStationary->mutable_dimension()->set_width(object->GetWidth());
    baseStationary->mutable_dimension()->set_length(object->GetLength());
    double yaw = object->GetHdg();
    yaw = CommonHelper::SetAngleToValidRange(yaw);
    baseStationary->mutable_orientation()->set_yaw(yaw);

    const auto mutableOsiClassification = osiSign->mutable_classification();

    mutableOsiClassification->set_type(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN);
    mutableOsiClassification->set_monochrome_color(osi3::RoadMarking_Classification_Color::RoadMarking_Classification_Color_COLOR_WHITE);
    if(object->GetType() == RoadObjectType::crosswalk)
    {
        mutableOsiClassification->set_traffic_main_sign_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_ZEBRA_CROSSING);
    }
    else
    {
        mutableOsiClassification->set_traffic_main_sign_type(osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_OTHER);
        mapping_succeeded = false;
    }

    return mapping_succeeded;
}

CommonTrafficSign::Entity RoadMarking::GetSpecification(const double relativeDistance) const
{
    CommonTrafficSign::Entity specification;

    specification.distanceToStartOfRoad = s;
    specification.relativeDistance = relativeDistance;

    const auto osiType = osiSign->classification().traffic_main_sign_type();

    if (OpenDriveTypeMapper::typeConversionMap.find(osiType) != OpenDriveTypeMapper::typeConversionMap.end())
    {
        specification.type = OpenDriveTypeMapper::typeConversionMap.at(osiType);
    }

    specification.text = osiSign->classification().value().text();

    return specification;
}

Primitive::AbsPosition RoadMarking::GetReferencePointPosition() const
{
    const osi3::Vector3d osiPosition = osiSign->base().position();
    Primitive::AbsPosition position;

    position.x = osiPosition.x();
    position.y = osiPosition.y();
    position.z = osiPosition.z();

    return position;
}

Primitive::Dimension RoadMarking::GetDimension() const
{
    return GetDimensionFromOsiObject(osiSign);
}

bool RoadMarking::IsValidForLane(OWL::Id laneId) const
{
    auto assignedLanes = osiSign->classification().assigned_lane_id();
    for (auto lane : assignedLanes)
    {
        if (lane.value() == laneId)
        {
            return true;
        }
    }
    return false;
}

void RoadMarking::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newRoadMarking = target.add_road_marking();
    newRoadMarking->CopyFrom(*osiSign);
}

LaneBoundary::LaneBoundary(osi3::LaneBoundary *osiLaneBoundary, double width, double sStart, double sEnd, LaneMarkingSide side) :
    osiLaneBoundary(osiLaneBoundary),
    sStart(sStart),
    sEnd(sEnd),
    width(width),
    side(side)
{
}

Id LaneBoundary::GetId() const
{
    return osiLaneBoundary->id().value();
}

double LaneBoundary::GetWidth() const
{
    return width;
}

double LaneBoundary::GetSStart() const
{
    return sStart;
}

double LaneBoundary::GetSEnd() const
{
    return sEnd;
}

LaneMarking::Type LaneBoundary::GetType() const
{
    return OpenDriveTypeMapper::OsiToOdLaneMarkingType(osiLaneBoundary->classification().type());
}

LaneMarking::Color LaneBoundary::GetColor() const
{
    return OpenDriveTypeMapper::OsiToOdLaneMarkingColor(osiLaneBoundary->classification().color());
}

LaneMarkingSide LaneBoundary::GetSide() const
{
    return side;
}

void LaneBoundary::AddBoundaryPoint(const Common::Vector2d &point, double heading)
{
    constexpr double doubleLineDistance = 0.15;
    auto boundaryPoint = osiLaneBoundary->add_boundary_line();
    switch (side)
    {
    case LaneMarkingSide::Single :
        boundaryPoint->mutable_position()->set_x(point.x);
        boundaryPoint->mutable_position()->set_y(point.y);
        break;
    case LaneMarkingSide::Left :
        boundaryPoint->mutable_position()->set_x(point.x - doubleLineDistance * std::sin(heading));
        boundaryPoint->mutable_position()->set_y(point.y + doubleLineDistance * std::cos(heading));
        break;
    case LaneMarkingSide::Right :
        boundaryPoint->mutable_position()->set_x(point.x + doubleLineDistance * std::sin(heading));
        boundaryPoint->mutable_position()->set_y(point.y - doubleLineDistance * std::cos(heading));
        break;
    }
    boundaryPoint->set_width(width);
}

void LaneBoundary::CopyToGroundTruth(osi3::GroundTruth& target) const
{
    auto newLaneBoundary = target.add_lane_boundary();
    newLaneBoundary->CopyFrom(*osiLaneBoundary);
}

Lane::LaneAssignmentCollector::LaneAssignmentCollector() noexcept
{
    downstreamOrderAssignments.reserve(INITIAL_COLLECTION_SIZE);
    upstreamOrderAssignments.reserve(INITIAL_COLLECTION_SIZE);
}

void Lane::LaneAssignmentCollector::Insert(const LaneOverlap& laneOverlap, const OWL::Interfaces::WorldObject* object)
{
    downstreamOrderAssignments.emplace_back(laneOverlap, object);
    upstreamOrderAssignments.emplace_back(laneOverlap, object);
    dirty = true;
}

void Lane::LaneAssignmentCollector::Clear()
{
    downstreamOrderAssignments.clear();
    upstreamOrderAssignments.clear();
    dirty = false;
}

void Lane::LaneAssignmentCollector::Sort() const
{  
    std::sort(downstreamOrderAssignments.begin(), downstreamOrderAssignments.end(),
              [](const auto& lhs, const auto& rhs)
    {
        return lhs.first.s_min < rhs.first.s_min || 
               (lhs.first.s_min == rhs.first.s_min && lhs.first.s_max < rhs.first.s_max);
    });

    std::sort(upstreamOrderAssignments.begin(), upstreamOrderAssignments.end(),
              [](const auto& lhs, const auto& rhs)
    {
        return lhs.first.s_max > rhs.first.s_max || 
               (lhs.first.s_max == rhs.first.s_max && lhs.first.s_min > rhs.first.s_min);
    });

    dirty = false;
}

const Interfaces::LaneAssignments& Lane::LaneAssignmentCollector::Get(bool downstream) const
{
    if(dirty)
    {
        Sort();
    }

    if(downstream)
    {
        return downstreamOrderAssignments;
    }
    else
    {
        return upstreamOrderAssignments;
    }
}

// namespace Implementation

}

} // namespace OWL
