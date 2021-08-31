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

#pragma once

#include <list>
#include <memory>
#include <vector>
#include <iterator>
#include <tuple>

#include "common/globalDefinitions.h"

#include "include/worldObjectInterface.h"
#include "include/roadInterface/roadInterface.h"
#include "include/roadInterface/roadLaneInterface.h"
#include "include/roadInterface/roadLaneSectionInterface.h"
#include "include/roadInterface/roadSignalInterface.h"

#include "OWL/OpenDriveTypeMapper.h"
#include "OWL/LaneGeometryElement.h"
#include "OWL/LaneGeometryJoint.h"
#include "OWL/Primitives.h"

#include "osi3/osi_common.pb.h"
#include "osi3/osi_groundtruth.pb.h"
#include "osi3/osi_trafficsign.pb.h"

namespace OWL {

using Id = uint64_t;
constexpr Id InvalidId = std::numeric_limits<::google::protobuf::uint64>::max();

using OdId = int64_t;

using Angle = float;

using Priorities = std::vector<std::pair<std::string, std::string>>;

constexpr double EVENTHORIZON = 2000;

enum class MeasurementPoint
{
    RoadStart,
    RoadEnd
};

enum class LaneMarkingSide
{
    Left,
    Right,
    Single
};

//! This struct describes the intersection of an object with a lane
struct LaneOverlap
{
    double s_min {std::numeric_limits<double>::max()};
    double s_max {0.0};
    double min_delta_left {std::numeric_limits<double>::max()};
    double min_delta_right {std::numeric_limits<double>::max()};
};

struct IntersectionInfo
{
    std::string intersectingRoad;
    IntersectingConnectionRank relativeRank;

    //! For each pair of lanes on the own road (first id) and the intersecting road (second id)
    //! this contains the start s and end s of the intersection on the own lane
    std::map<std::pair<Id, Id>, std::pair<double, double>> sOffsets;
};


template<typename T>
struct Lazy
{
private:
    T value{};
    bool valid{false};

public:
    void Update(T newValue)
    {
        value = newValue;
        valid = true;
    }

    void Invalidate()
    {
        valid = false;
    }

    T Get() const
    {
        if (valid)
        {
            return value;
        }
        else
        {
            throw std::logic_error("retrieved invalidated lazy");
        }
    }

    bool IsValid() const
    {
        return valid;
    }
};

namespace Interfaces {
class Lane;
class LaneBoundary;
class Section;
class Road;
class Junction;
class WorldObject;
class StationaryObject;
class MovingObject;
class TrafficSign;
class TrafficLight;
class RoadMarking;

using LaneGeometryElements = std::vector<Primitive::LaneGeometryElement*>;
using LaneGeometryJoints   = std::vector<Primitive::LaneGeometryJoint>;
using Lanes                = std::list<const Lane*>;
using Sections             = std::list<const Section*>;
using Roads                = std::list<const Road*>;
using WorldObjects         = std::list<WorldObject*>;
using MovingObjects        = std::list<MovingObject*>;
using StationaryObjects    = std::list<StationaryObject*>;
using TrafficSigns         = std::list<TrafficSign*>;
using TrafficLights        = std::list<TrafficLight*>;
using RoadMarkings         = std::list<RoadMarking*>;
using LaneAssignment       = std::pair<LaneOverlap, const Interfaces::WorldObject*>;
using LaneAssignments      = std::vector<LaneAssignment>;

//! This class represents a single lane inside a section
class Lane
{
public:
    virtual ~Lane() = default;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;

    //! Returns the OSI Id
    virtual Id GetId() const = 0;

    //! Returns false of this lane is invalid, otherwise returns true
    virtual bool Exists() const = 0;

    //!Returns the section that this lane is part of
    virtual const Section& GetSection() const = 0;

    //!Returns the road that this lane is part of
    virtual const Road& GetRoad() const = 0;

    //!Returns all lane geometry elements of the lane
    virtual const LaneGeometryElements& GetLaneGeometryElements() const = 0;

    //!Returns the length of the lane
    virtual double GetLength() const = 0;

    //!Returns the number of lanes to the right of this lane
    virtual int GetRightLaneCount() const = 0;

    //!Returns the type of the lane
    virtual LaneType GetLaneType() const = 0;

    //!Returns the curvature of the lane at the specified distance
    //!
    //! @param distance s coordinate
    virtual double GetCurvature(double distance) const = 0;

    //!Returns the width of the lane at the specified distance
    //!
    //! @param distance s coordinate
    virtual double GetWidth(double distance) const = 0;

    //!Returns the direction of the lane at the specified distance
    //!
    //! @param distance s coordinate
    virtual double GetDirection(double distance) const = 0;

    //!Returns the left, right and reference point at the specified s coordinate interpolated between the geometry joints
    //!
    //! @param distance s coordinate
    virtual const Primitive::LaneGeometryJoint::Points GetInterpolatedPointsAtDistance(double distance) const = 0;

    //!Returns the ids of all successors of this lane
    virtual const std::vector<Id>& GetNext() const = 0;

    //!Returns the ids of all predecessors of this lane
    virtual const std::vector<Id>& GetPrevious() const = 0;

    //!Returns the (possibly invalid) lane to the right of this lane
    virtual const Lane& GetLeftLane() const = 0;

    //!Returns the (possibly invalid) lane to the right of this lane
    virtual const Lane& GetRightLane() const = 0;

    //! Returns the ids of all left lane boundaries
    virtual const std::vector<Id> GetLeftLaneBoundaries() const = 0;

    //! Returns the ids of all right lane boundaries
    virtual const std::vector<Id> GetRightLaneBoundaries() const = 0;

    //!Returns the s coordinate of the lane start if measure point is road start
    //! or the s coordinate of the lane end if measure point is road end
    virtual double GetDistance(MeasurementPoint measurementPoint) const = 0;

    //!Returns true if the specified distance is valid on this lane, otherwise returns false
    virtual bool Covers(double distance) const = 0;

    //!Sets the sucessor of the lane. Throws an error if the lane already has a sucessor
    virtual void AddNext(const Lane* lane) = 0;

    //!Sets the predecessor of the lane. Throws an error if the lane already has a predecessor
    virtual void AddPrevious(const Lane* lane) = 0;

    //!Adds a new lane geometry joint to the end of the current list of joints
    //!
    //! @param pointLeft    left point of the new joint
    //! @param pointCenter  reference point of the new joint
    //! @param pointRight   right point of the new joint
    //! @param sOffset      s offset of the new joint
    //! @param curvature    curvature of the lane at sOffset
    //! @param heading      heading of the lane at sOffset
    virtual void AddLaneGeometryJoint(const Common::Vector2d& pointLeft,
                                      const Common::Vector2d& pointCenter,
                                      const Common::Vector2d& pointRight,
                                      double sOffset,
                                      double curvature,
                                      double heading) = 0;

    //!Sets type of the lane
    virtual void SetLaneType(LaneType specifiedType) = 0;

    //!Sets lane to the left of this lane
    virtual void SetLeftLane(const Interfaces::Lane& lane, bool transferLaneBoundary) = 0;

    //!Sets lane to the right of this lane
    virtual void SetRightLane(const Interfaces::Lane& lane, bool transferLaneBoundary) = 0;

    //! Adds the ids to the list of left lane boundaries
    //! \param laneBoundaries   ids to add
    //!
    virtual void SetLeftLaneBoundaries(const std::vector<OWL::Id> laneBoundaries) = 0;

    //! Adds the ids to the list of right lane boundaries
    //! \param laneBoundaries   ids to add
    //!
    virtual void SetRightLaneBoundaries(const std::vector<OWL::Id> laneBoundaries) = 0;

    //!Returns a map of all WorldObjects that are currently in this lane
    virtual const LaneAssignments& GetWorldObjects(bool direction) const = 0;

    //!Returns a vector of all traffic signs that are assigned to this lane
    virtual const Interfaces::TrafficSigns& GetTrafficSigns() const = 0;

    //!Returns a vector of all road markings that are assigned to this lane
    virtual const Interfaces::RoadMarkings& GetRoadMarkings() const = 0;

    //!Returns a vector of all traffic lights that are assigned to this lane
    virtual const Interfaces::TrafficLights& GetTrafficLights() const = 0;

    //!Adds a MovingObject to the list of objects currently in this lane
    virtual void AddMovingObject(OWL::Interfaces::MovingObject& movingObject, const LaneOverlap& laneOverlap) = 0;

    //!Adds a StationaryObject to the list of objects currently in this lane
    virtual void AddStationaryObject(OWL::Interfaces::StationaryObject& stationaryObject, const LaneOverlap& laneOverlap) = 0;

    //!Adds a WorldObject to the list of objects currently in this lane
    virtual void AddWorldObject(Interfaces::WorldObject& worldObject, const LaneOverlap& laneOverlap) = 0;

    //!Assigns a traffic sign to this lane
    virtual void AddTrafficSign (Interfaces::TrafficSign &trafficSign) = 0;

    //!Assigns a road marking to this lane
    virtual void AddRoadMarking (Interfaces::RoadMarking &roadMarking) = 0;

    //!Assigns a traffic light to this lane
    virtual void AddTrafficLight (Interfaces::TrafficLight &trafficLight) = 0;

    //!Removes all MovingObjects from the list of objects currently in this lane while keeping StationaryObjects
    virtual void ClearMovingObjects() = 0;
};

//! This class represents the boundary between two lanes
class LaneBoundary
{
public:
    virtual ~LaneBoundary() = default;

    //! Returns the OSI Id
    virtual Id GetId() const = 0;

    //! Returns the width of the boundary
    virtual double GetWidth() const = 0;

    //! Returns the s coordinate, where this boundary starts
    virtual double GetSStart() const = 0;

    //! Returns the s coordinate, where this boundary ends
    virtual double GetSEnd() const = 0;

    //! Returns the type of the boundary
    virtual LaneMarking::Type GetType() const = 0;

    //! Return the color of the boundary
    virtual LaneMarking::Color GetColor() const = 0;

    //! Returns the side (Left/Right) of this boundary if it is part of a double line, or Single otherwise
    virtual LaneMarkingSide GetSide() const = 0;

    //! Adds a new point the end of the list of boundary points
    //!
    //! \param point    point to add
    //! \param heading  heading of the lane
    //!
    virtual void AddBoundaryPoint(const Common::Vector2d& point, double heading) = 0;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;
};

//!This class represents one section of a road.
class Section
{
public:
    virtual ~Section() = default;

    //!Sets the sucessor of the section. Throws an error if the section already has a sucessor
    virtual void AddNext(const Interfaces::Section& section) = 0;

    //!Sets the predecessor of the section. Throws an error if the section already has a predecessor
    virtual void AddPrevious(const Interfaces::Section& section) = 0;

    //!Adds a lane to this section
    virtual void AddLane(const Interfaces::Lane& lane) = 0;

    //!Sets the road that this section is part of
    virtual void SetRoad(Interfaces::Road* road) = 0;

    //!Returns a vector of all lanes of the section
    virtual const Lanes& GetLanes() const = 0;

    //!Returns the road that this section is part of
    virtual const Interfaces::Road& GetRoad() const = 0;

    //!Returns the s coordinate of the section start if measure point is road start
    //! or the s coordinate of the section end if measure point is road end
    virtual double GetDistance(MeasurementPoint measurementPoint) const = 0;

    //!Returns true if the specified distance is valid on this sections, otherwise returns false
    virtual bool Covers(double distance) const = 0;

    //!Returns true at least one s coordinate between startDistance and endDistance is valid on this section
    virtual bool CoversInterval(double startDistance, double endDistance) const = 0;

    //!Returns the s coordinate of the section start
    virtual double GetSOffset() const = 0;

    //!Returns the length of the section
    virtual double GetLength() const = 0;

    //! Sets the ids of the lane boundaries of the center lane
    //! \param laneBoundaryId   ids of center lane boundaries
    //!
    virtual void SetCenterLaneBoundary(std::vector<Id> laneBoundaryId) = 0;

    //! Returns the ids of the lane boundaries of the center lane
    virtual std::vector<Id> GetCenterLaneBoundary() const = 0;
};

//!This class represents a road in the world
class Road
{
public:
    virtual ~Road() = default;

    //!Returns the Id of the road
    virtual const std::string& GetId() const = 0;

    //!Adds a section to this road
    virtual void AddSection(Interfaces::Section& section) = 0;

    //!Returns a vector of all sections that this road consists of
    virtual const Sections& GetSections() const = 0;

    //!Returns the length of the road
    virtual double GetLength() const = 0;

    //!Returns the id of the successor of the road (i.e. next road or junction)
    virtual const std::string& GetSuccessor() const = 0;

    //!Returns the id of the predecessor of the road (i.e. previous road or junction)
    virtual const std::string& GetPredecessor() const = 0;

    //! Sets the successor of the road
    //! \param successor    id of successor (road or junction)
    virtual void SetSuccessor(const std::string& successor) = 0;

    //! Sets the predecessor of the road
    //! \param predecessor  id of predecessor (road or junction)
    virtual void SetPredecessor(const std::string& predecessor) = 0;

    //!Returns true if the direction of the road is the same as the direction of the road stream,
    //! false otherwise
    virtual bool IsInStreamDirection() const = 0;

    //! Returns the distance to a point on the road, relative to the road itself
    //! \param mp the point on the road to measure
    virtual double GetDistance(MeasurementPoint mp) const = 0;
};

//!This class represents a junction in the world
class Junction
{
public:
    virtual ~Junction() = default;

    //!Returns the Id of the Junction
    virtual const std::string& GetId() const = 0;

    //!Adds a connecting road to this road
    virtual void AddConnectingRoad(const Interfaces::Road* connectingRoad) = 0;

    //!Returns a vector of all connecting roads that this road consists of
    virtual const Roads& GetConnectingRoads() const = 0;

    //!Adds a priority entry for this junction
    virtual void AddPriority(const std::string& high, const std::string& low) = 0;

    //!Returns all priorities of connectors in this junction
    virtual const Priorities& GetPriorities() const = 0;

    //!Adds intersection info for the road with roadId to the junction
    virtual void AddIntersectionInfo(const std::string& roadId, const IntersectionInfo& intersectionInfo) = 0;

    //!Gets all of this junction's roads' intersection info
    virtual const std::map<std::string, std::vector<IntersectionInfo>>& GetIntersections() const = 0;
};

//!Common base for all objects in the world that can be on the road and have a position and a dimension
//! WorldObjects are either MovingObjects or StationaryObjects
class WorldObject
{
public:
    virtual ~WorldObject() = default;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;

    //!Returns the OSI Id of the object
    virtual Id GetId() const = 0;

    //!Returns the dimension of the object
    virtual Primitive::Dimension GetDimension() const = 0;

    //!Returns the position of the reference point of the object in absolute coordinates (i. e. world coordinates)
    virtual Primitive::AbsPosition GetReferencePointPosition() const = 0;

    //!Returns the orientation of the object in absolute coordinates (i. e. world coordinates)
    virtual Primitive::AbsOrientation GetAbsOrientation() const = 0;

    //!Returns the absolute value of the velocity if the object is moving in road direction
    //! or the inverse if the object is moving against the road direction
    virtual double GetAbsVelocityDouble() const = 0;

    //!Returns the absolute value of the acceleration if the object is moving in road direction
    //! or the inverse if the object is moving against the road direction
    virtual double GetAbsAccelerationDouble() const = 0;

    //!Returns the road coordinates of the object
    virtual const ObjectPosition& GetLocatedPosition() const = 0;

    //!Assigns a lane to this object
    virtual void AddLaneAssignment(const Interfaces::Lane& lane) = 0;

    //!Returns a vector of all lanes that this object is assigned to
    virtual const Interfaces::Lanes& GetLaneAssignments() const = 0;

    //!Clears the list of lanes that this object is assigned to
    virtual void ClearLaneAssignments() = 0;

    //!Sets the position of the reference point of the object in absolute coordinates (i. e. world coordinates)
    virtual void SetReferencePointPosition(const Primitive::AbsPosition& newPosition) = 0;

    //!Sets the dimension of the object
    virtual void SetDimension(const Primitive::Dimension& newDimension) = 0;

    //!Sets the orientation of the object in absolute coordinates (i. e. world coordinates)
    virtual void SetAbsOrientation(const Primitive::AbsOrientation& newOrientation) = 0;

    //!Sets the road coordinates of the object
    virtual void SetLocatedPosition(const ObjectPosition& position) = 0;

    virtual double GetDistance(MeasurementPoint measurementPoint, const std::string& roadId) const = 0;

    //!Returns the WorldObjectInterface that this WorldObject is linked to and casts it to T*
    //! T can be
    //! -WorldObjectsAdapter or
    //! -AgentAdapter, if this is a MovingObject or
    //! -TrafficAdapter, if this is a StationaryObject
    template <typename T>
    T* GetLink()
    {
        auto link = dynamic_cast<T*>(static_cast<WorldObjectInterface*>(linkedObject));
        assert(link != nullptr);
        return link;
    }

    //!Returns the WorldObjectInterface that this WorldObject is linked to and casts it to T*
    //! T can be
    //! -WorldObjectsAdapter or
    //! -AgentAdapter, if this is a MovingObject or
    //! -TrafficAdapter, if this is a StationaryObject
    template <typename T>
    T* GetLink() const
    {
        auto link = dynamic_cast<T*>(static_cast<WorldObjectInterface*>(linkedObject));
        assert(link != nullptr);
        return link;
    }

    //!Returns true is this object is of type T otherwise false
    template <typename T>
    bool Is() const
    {
        return !(dynamic_cast<const T*>(this) == nullptr);
    }

    //!Returns this object casted to T*
    template <typename T>
    T* As() const
    {
        return static_cast<T*>(this);
    }

    //!Returns this object casted to T*
    template <typename T>
    T* As()
    {
        return static_cast<T*>(this);
    }

protected:
    void* linkedObject{nullptr};
};

class StationaryObject : public Interfaces::WorldObject
{
public:
    virtual ~StationaryObject() = default;

    virtual const ObjectPosition& GetLocatedPosition() const = 0;

    virtual void SetReferencePointPosition(const Primitive::AbsPosition& newPosition) = 0;
    virtual void SetDimension(const Primitive::Dimension& newDimension) = 0;
    virtual void SetAbsOrientation(const Primitive::AbsOrientation& newOrientation) = 0;
    virtual void SetLocatedPosition(const ObjectPosition& position) = 0;
};


class MovingObject : public Interfaces::WorldObject
{
public:
    virtual ~MovingObject() = default;

    virtual const ObjectPosition& GetLocatedPosition() const = 0;
    virtual Primitive::LaneOrientation GetLaneOrientation() const = 0;

    virtual Primitive::AbsVelocity GetAbsVelocity() const = 0;

    virtual Primitive::AbsAcceleration GetAbsAcceleration() const = 0;

    virtual Primitive::AbsOrientationRate GetAbsOrientationRate() const = 0;

    virtual void SetDimension(const Primitive::Dimension& newDimension) = 0;
    virtual void SetLength(const double newLength) = 0;
    virtual void SetWidth(const double newWidth) = 0;
    virtual void SetHeight(const double newHeight) = 0;
    virtual void SetBoundingBoxCenterToRear(const double distance) = 0;
    virtual double GetDistanceReferencePointToLeadingEdge() const = 0;

    virtual void SetReferencePointPosition(const Primitive::AbsPosition& newPosition) = 0;
    virtual void SetX(const double newX) = 0;
    virtual void SetY(const double newY) = 0;
    virtual void SetZ(const double newZ) = 0;

    virtual void SetLocatedPosition(const ObjectPosition& position) = 0;

    virtual void SetAbsOrientation(const Primitive::AbsOrientation& newOrientation) = 0;
    virtual void SetYaw(const double newYaw) = 0;
    virtual void SetPitch(const double newPitch) = 0;
    virtual void SetRoll(const double newRoll) = 0;

    virtual void SetAbsVelocity(const Primitive::AbsVelocity& newVelocity) = 0;
    virtual void SetAbsVelocity(const double newVelocity) = 0;

    virtual void SetAbsAcceleration(const Primitive::AbsAcceleration& newAcceleration) = 0;
    virtual void SetAbsAcceleration(const double newAcceleration) = 0;

    virtual void SetAbsOrientationRate(const Primitive::AbsOrientationRate& newOrientationRate) = 0;

    virtual void AddLaneAssignment(const Interfaces::Lane& lane) = 0;
    virtual const Interfaces::Lanes& GetLaneAssignments() const = 0;
    virtual void ClearLaneAssignments() = 0;

    virtual void SetIndicatorState(IndicatorState indicatorState) = 0;
    virtual IndicatorState GetIndicatorState() const = 0;
    virtual void SetBrakeLightState(bool brakeLightState) = 0;
    virtual bool GetBrakeLightState() const = 0;
    virtual void SetHeadLight(bool headLight) = 0;
    virtual bool GetHeadLight() const = 0;
    virtual void SetHighBeamLight(bool highbeamLight) = 0;
    virtual bool GetHighBeamLight() const = 0;

    virtual void SetType(AgentVehicleType type) = 0;
};

//! This class represents a static traffic sign
class TrafficSign
{
public:
    virtual ~TrafficSign() = default;

    //! Returns the OpenDrive ID
    virtual std::string GetId() const = 0;

    //! Returns the s coordinate
    virtual double GetS() const = 0;

    //! Returns the value of the sign converted in SI Units
    virtual std::pair<double, CommonTrafficSign::Unit> GetValueAndUnitInSI(const double osiValue, const osi3::TrafficSignValue_Unit osiUnit) const = 0;

    //! Returns the specification of the sign with the set relative distance
    virtual CommonTrafficSign::Entity GetSpecification(const double relativeDistance) const = 0;

    //! Returns wether the sign is valid for the specified lane
    virtual bool IsValidForLane(OWL::Id laneId) const = 0;

    //!Returns the position of the reference point of the object in absolute coordinates (i. e. world coordinates)
    virtual Primitive::AbsPosition GetReferencePointPosition() const = 0;

    //!Returns the dimension of the sign
    virtual Primitive::Dimension GetDimension() const = 0;

    //! Sets the s coordinate
    virtual void SetS(double sPos) = 0;

    //! Adds the specified lane to the list of valid lanes
    virtual void SetValidForLane(OWL::Id laneId) = 0;

    //! Converts the specification imported from OpenDrive to OSI.
    //!
    //! \param signal       OpenDrive specification
    //! \param position     position in the world
    //! \return     true if succesful, false if the sign can not be converted
    virtual bool SetSpecification(RoadSignalInterface* signal, Position position) = 0;

    //! Adds a supplementary sign to this sign and converts its specifation from OpenDrive to OSI
    //!
    //! \param odSignal     OpenDrive specification of supplementary sign
    //! \param position     position of the supplementary sign
    //! \return     true if succesful, false if the sign can not be converted
    virtual bool AddSupplementarySign(RoadSignalInterface* odSignal, Position position) = 0;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;
};

//! This class represents a traffic light
class TrafficLight
{
public:
    virtual ~TrafficLight() = default;

    //! Returns the OpenDrive ID
    virtual std::string GetId() const = 0;

    //! Returns the s coordinate
    virtual double GetS() const = 0;

    //! Returns wether the sign is valid for the specified lane
    virtual bool IsValidForLane(OWL::Id laneId) const = 0;

    //!Returns the position of the reference point of the object in absolute coordinates (i. e. world coordinates)
    virtual Primitive::AbsPosition GetReferencePointPosition() const = 0;

    //! Sets the s coordinate
    virtual void SetS(double sPos) = 0;

    //! Converts the specification imported from OpenDrive to OSI.
    //!
    //! \param signal       OpenDrive specification
    //! \param position     position in the world
    //! \return     true if succesfull, false if the sign can not be converted
    virtual bool SetSpecification(RoadSignalInterface* signal, const Position& position) = 0;

    //! Adds the specified lane to the list of valid lanes
    virtual void SetValidForLane(OWL::Id laneId) = 0;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;

    //! Sets the state
    virtual void SetState(CommonTrafficLight::State newState) = 0;

    //! Returns the specification of the light with the set relative distance
    virtual CommonTrafficLight::Entity GetSpecification(const double relativeDistance) const = 0;

    //!Returns the current state
    virtual CommonTrafficLight::State GetState() const = 0;
};

//! This class represents a traffic sign that is painted on the road surface
class RoadMarking
{
public:
    virtual ~RoadMarking() = default;

    //! Returns the OpenDrive ID
    virtual std::string GetId() const = 0;

    //! Returns the s coordinate
    virtual double GetS() const = 0;

    //! Returns the specification of the sign with the set relative distance
    virtual CommonTrafficSign::Entity GetSpecification(const double relativeDistance) const = 0;

    //! Returns wether the sign is valid for the specified lane
    virtual bool IsValidForLane(OWL::Id laneId) const = 0;

    //!Returns the position of the reference point of the object in absolute coordinates (i. e. world coordinates)
    virtual Primitive::AbsPosition GetReferencePointPosition() const = 0;

    //!Returns the dimension of the marking
    virtual Primitive::Dimension GetDimension() const = 0;

    //! Sets the s coordinate
    virtual void SetS(double sPos) = 0;

    //! Adds the specified lane to the list of valid lanes
    virtual void SetValidForLane(OWL::Id laneId) = 0;

    //! Converts the specification imported from OpenDrive to OSI.
    //!
    //! \param signal       OpenDrive specification
    //! \param position     position in the world
    //! \return     true if succesfull, false if the road marking can not be converted
    virtual bool SetSpecification(RoadSignalInterface* signal, Position position) = 0;

    //! Converts the specification imported from OpenDrive to OSI.
    //!
    //! \param signal       OpenDrive specification
    //! \param position     position in the world
    //! \return     true if succesfull, false if the road marking can not be converted
    virtual bool SetSpecification(RoadObjectInterface* signal, Position position) = 0;

    /*!
     * \brief Copies the underlying OSI object to the given GroundTruth
     *
     * \param[in]   target   The target OSI GroundTruth
     */
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const = 0;
};



} // namespace Interfaces

namespace Implementation {


class Lane : public Interfaces::Lane
{
public:
    //! @param[in] osiLane  representation of the road in OpenDrive
    //! @param[in] section  section that this lane is part of
    //! @param[in] isInStreamDirection  flag whether this lane is in the same direction as the LaneStream it belongs to
    Lane(osi3::Lane* osiLane, const Interfaces::Section* section);
    ~Lane() override;

    void CopyToGroundTruth(osi3::GroundTruth& target) const override;

    Id GetId() const override;
    bool Exists() const override;
    const Interfaces::Section& GetSection() const override;
    const Interfaces::Road& GetRoad() const override;

    double GetLength() const override;
    int GetRightLaneCount() const override;

    double GetCurvature(double distance) const override;
    double GetWidth(double distance) const override;
    double GetDirection(double distance) const override;
    const std::vector<Id>& GetNext() const override
    {
        return next;
    }
    const std::vector<Id>& GetPrevious() const override
    {
        return previous;
    }
    const Interfaces::Lane& GetLeftLane() const override;
    const Interfaces::Lane& GetRightLane() const override;
    const std::vector<Id> GetLeftLaneBoundaries() const override;
    const std::vector<Id> GetRightLaneBoundaries() const override;

    double GetDistance(MeasurementPoint measurementPoint) const override;
    LaneType GetLaneType() const override;
    bool Covers(double distance) const override;

    void AddNext(const Interfaces::Lane* lane) override;
    void AddPrevious(const Interfaces::Lane* lane) override;
    const Interfaces::LaneGeometryElements& GetLaneGeometryElements() const override;
    void AddLaneGeometryJoint(const Common::Vector2d& pointLeft,
                              const Common::Vector2d& pointCenter,
                              const Common::Vector2d& pointRight,
                              double sOffset,
                              double curvature,
                              double heading) override;

    void SetLeftLane(const Interfaces::Lane& lane, bool transferLaneBoundary) override;
    void SetRightLane(const Interfaces::Lane& lane, bool transferLaneBoundary) override;
    void SetLeftLaneBoundaries(const std::vector<OWL::Id> laneBoundaries) override;
    void SetRightLaneBoundaries(const std::vector<OWL::Id> laneBoundaries) override;
    void SetLaneType(LaneType specifiedType) override;

    const Interfaces::LaneAssignments& GetWorldObjects(bool direction) const override;
    const Interfaces::TrafficSigns& GetTrafficSigns() const override;
    const Interfaces::RoadMarkings& GetRoadMarkings() const override;
    const Interfaces::TrafficLights& GetTrafficLights() const override;

    void AddMovingObject(OWL::Interfaces::MovingObject& movingObject, const LaneOverlap& laneOverlap) override;
    void AddStationaryObject(OWL::Interfaces::StationaryObject& stationaryObject, const LaneOverlap& laneOverlap) override;
    void AddWorldObject(Interfaces::WorldObject& worldObject, const LaneOverlap& laneOverlap) override;
    void AddTrafficSign (Interfaces::TrafficSign &trafficSign) override;
    void AddRoadMarking (Interfaces::RoadMarking &roadMarking) override;
    void AddTrafficLight (Interfaces::TrafficLight &trafficLight) override;
    void ClearMovingObjects() override;

    std::tuple<const Primitive::LaneGeometryJoint*, const Primitive::LaneGeometryJoint*> GetNeighbouringJoints(
            double distance) const;

    const Primitive::LaneGeometryJoint::Points GetInterpolatedPointsAtDistance(double distance) const override;

    //! @brief Collects objects assigned to lanes and manages their order w.r.t the querying direction
    //!
    //! Depending on the querying direction, objects on a lane need to be sorted 
    //! either by their starting coordinate (s_min) or their ending coordinate (s_max).
    //! When looking downstream, objects are sorted ascending by their s_min coordinate.
    //! When looking upstream, objects are sorted descending by their s_max coordinate.
    class LaneAssignmentCollector
    {
        public:
            LaneAssignmentCollector() noexcept;

            //! @brief Insert an object, given it's lane overlap w.r.t the current lane
            //! @param LaneOverlap minimum and maximum s coordinate of the object
            //! @param object      the object
            void Insert(const LaneOverlap& laneOverlap, const Interfaces::WorldObject* object);

            //! @brief Get list of LaneAssignements
            //! @param downstream  true for looking "in stream direction", else false
            const Interfaces::LaneAssignments& Get(bool downstream) const;

            //! @brief Clears the managed collections
            void Clear();
        private:
            //! @brief Sorts the managed collections using a custom sorter for LaneOverlap
            //!
            //! The two internal collecions are sorted by the following comparisons:
            //! Ascending: Unless equal, the smaller s_min wins, else the smaller s_max
            //! Descending: Unless equal, the larger s_max wins, else the larger s_min
            void Sort() const;

            //! @brief  Flag indicating, that collections need sorting
            //!
            //! For performance reasons, this class uses lazy sorting, meaning that insertion
            //! happens unordered and sorting is applied at the very first access of the collections.
            bool mutable dirty{false};
            
            Interfaces::LaneAssignments mutable downstreamOrderAssignments;
            Interfaces::LaneAssignments mutable upstreamOrderAssignments;

            static constexpr size_t INITIAL_COLLECTION_SIZE = 32; //!!< Minimum reserved collection size to prevent to many reallocations
    };

protected:
    osi3::Lane* osiLane{nullptr};

private:
    LaneType laneType{LaneType::Undefined};
    LaneAssignmentCollector worldObjects;
    Interfaces::LaneAssignments stationaryObjects;
    Interfaces::TrafficSigns trafficSigns;
    Interfaces::RoadMarkings roadMarkings;
    Interfaces::TrafficLights trafficLights;
    const Interfaces::Section* section;
    Interfaces::LaneGeometryJoints laneGeometryJoints;
    Interfaces::LaneGeometryElements laneGeometryElements;
    std::vector<Id> next;
    std::vector<Id> previous;
    const Interfaces::Lane* leftLane;
    const Interfaces::Lane* rightLane;
    double length{0.0};
    bool leftLaneIsDummy{section == nullptr};
    bool rightLaneIsDummy{section == nullptr};
};

class InvalidLane : public Lane
{
public:
    InvalidLane() : Lane(new osi3::Lane(), nullptr)
    {
        // set 'invalid' id
        osiLane->mutable_id()->set_value(InvalidId);
    }

    virtual ~InvalidLane();
    InvalidLane(const InvalidLane&) = delete;
    InvalidLane& operator=(const InvalidLane&) = delete;
    InvalidLane(InvalidLane&&) = delete;
    InvalidLane& operator=(InvalidLane&&) = delete;
    const Interfaces::Lane& GetLeftLane() const override
    {
        return *this;
    }
    const Interfaces::Lane& GetRightLane() const override
    {
        return *this;
    }
};

class LaneBoundary : public Interfaces::LaneBoundary
{
public:
    LaneBoundary(osi3::LaneBoundary* osiLaneBoundary, double width, double sStart, double sEnd, LaneMarkingSide side);
    virtual Id GetId() const override;
    virtual double GetWidth() const override;
    virtual double GetSStart() const override;
    virtual double GetSEnd() const override;
    virtual LaneMarking::Type GetType() const override;
    virtual LaneMarking::Color GetColor() const override;
    virtual LaneMarkingSide GetSide() const override;
    virtual void AddBoundaryPoint(const Common::Vector2d& point, double heading) override;
    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const override;

private:
    osi3::LaneBoundary* osiLaneBoundary;    //! underlying OSI object
    double sStart;                          //! s coordinate where this boundary starts
    double sEnd;                            //! s coordinate where this boundary ends
    double width;                           //! width of the line
    LaneMarkingSide side;                   //! Side (Left/Right) if this boundary is part of a double line or Single otherwise
};

class Section : public Interfaces::Section
{
public:
    Section(double sOffset);

    void AddNext(const Interfaces::Section& section) override;
    void AddPrevious(const Interfaces::Section& section) override;

    void AddLane(const Interfaces::Lane& lane) override;
    const Interfaces::Lanes& GetLanes() const override;

    void SetRoad(Interfaces::Road* road) override;
    const Interfaces::Road& GetRoad() const override;

    virtual double GetDistance(MeasurementPoint measurementPoint) const override;
    virtual bool Covers(double distance) const override;
    virtual bool CoversInterval(double startDistance, double endDistance) const override;

    double GetSOffset() const override;
    double GetLength() const override;

    virtual void SetCenterLaneBoundary(std::vector<Id> laneBoundaryIds) override;
    virtual std::vector<Id> GetCenterLaneBoundary() const override;

private:
    Interfaces::Lanes lanes;
    const Interfaces::Road* road;
    Interfaces::Sections nextSections;
    Interfaces::Sections previousSections;
    std::vector<Id> centerLaneBoundary;
    double sOffset;

public:
    Section(const Section&) = delete;
    Section& operator=(const Section&) = delete;
    Section(Section&&) = delete;
    Section& operator=(Section&&) = delete;

    virtual ~Section() override = default;
};

class InvalidSection : public Section
{
public:
    InvalidSection() : Section(0)
    {}

    InvalidSection(const InvalidSection&) = delete;
    InvalidSection& operator=(const InvalidSection&) = delete;
    InvalidSection(InvalidSection&&) = delete;
    InvalidSection& operator=(InvalidSection&&) = delete;
};

class Road : public Interfaces::Road
{
public:
    Road(bool isInStreamDirection, const std::string& id);

    Road(const Road&) = delete;
    Road& operator=(const Road&) = delete;
    Road(Road&&) = delete;
    Road& operator=(Road&&) = delete;

    virtual ~Road() override = default;

    const std::string& GetId() const override;
    const Interfaces::Sections& GetSections() const override;
    void AddSection(Interfaces::Section& section) override;
    double GetLength() const override;
    const std::string& GetSuccessor() const override;
    const std::string& GetPredecessor() const override;
    void SetSuccessor(const std::string& successor) override;
    void SetPredecessor(const std::string& predecessor) override;
    bool IsInStreamDirection() const override;
    double GetDistance(MeasurementPoint mp) const override;

private:
    Interfaces::Sections sections;
    std::string successor{};
    std::string predecessor{};
    bool isInStreamDirection;
    std::string id;
};

class InvalidRoad : public Road
{
public:
    InvalidRoad() : Road(true, "")
    {}

    InvalidRoad(const InvalidRoad&) = delete;
    InvalidRoad& operator=(const InvalidRoad&) = delete;
    InvalidRoad(InvalidRoad&&) = delete;
    InvalidRoad& operator=(InvalidRoad&&) = delete;
};

class Junction : public Interfaces::Junction
{
public:
    Junction(const std::string& id) :
        id(id)
    {}

    virtual ~Junction() override = default;

    virtual const std::string& GetId() const override
    {
        return id;
    }

    virtual void AddConnectingRoad(const Interfaces::Road* connectingRoad) override
    {
        connectingRoads.push_back(connectingRoad);
    }

    virtual const std::list<const Interfaces::Road*>& GetConnectingRoads() const override
    {
        return connectingRoads;
    }

    virtual void AddPriority(const std::string& high, const std::string& low) override
    {
        priorities.emplace_back(std::pair{high, low});
    }

    virtual const Priorities& GetPriorities() const override
    {
        return priorities;
    }

    virtual void AddIntersectionInfo(const std::string& roadId, const IntersectionInfo& intersectionInfo) override
    {
        auto& intersectionInfos = intersections[roadId];

        // if roadId already has intersection info registered to it, check if any intersection info is registered for the intersectionInfo.intersectingRoad
        auto existingIntersectionInfoForRoadPairIter = std::find_if(intersectionInfos.begin(),
                                                                          intersectionInfos.end(),
                                                                          [&intersectionInfo](const auto& existingIntersectionInfo) -> bool
        {
            return intersectionInfo.intersectingRoad == existingIntersectionInfo.intersectingRoad;
        });

        // if it does, add the new sOffsets to the already existing intersectionInfo for the pair of roads
        if (existingIntersectionInfoForRoadPairIter != intersectionInfos.end())
        {
            existingIntersectionInfoForRoadPairIter->sOffsets.insert(intersectionInfo.sOffsets.begin(), intersectionInfo.sOffsets.end());
        }
        else
        {
            intersectionInfos.emplace_back(intersectionInfo);
        }
    }

    virtual const std::map<std::string, std::vector<IntersectionInfo>>& GetIntersections() const override
    {
        return intersections;
    }

    Junction(const Junction&) = delete;
    Junction& operator=(const Junction&) = delete;
    Junction(Junction&&) = delete;
    Junction& operator=(Junction&&) = delete;

private:
    std::string id;
    std::list<const Interfaces::Road*> connectingRoads;
    Priorities priorities;
    std::map<std::string, std::vector<IntersectionInfo>> intersections;
};

class StationaryObject : public Interfaces::StationaryObject
{
public:
    StationaryObject(osi3::StationaryObject* osiStationaryObject, void* linkedObject);
    virtual ~StationaryObject() override = default;

    virtual Primitive::Dimension GetDimension() const override;

    virtual Primitive::AbsOrientation GetAbsOrientation() const override;
    virtual Primitive::AbsPosition GetReferencePointPosition() const override;
    virtual double GetAbsVelocityDouble() const override;
    virtual double GetAbsAccelerationDouble() const override;
    virtual const ObjectPosition& GetLocatedPosition() const override;

    void SetReferencePointPosition(const Primitive::AbsPosition& newPosition) override;
    void SetDimension(const Primitive::Dimension& newDimension) override;
    void SetAbsOrientation(const Primitive::AbsOrientation& newOrientation) override;

    Id GetId() const override;
    void AddLaneAssignment(const Interfaces::Lane& lane) override;
    const Interfaces::Lanes& GetLaneAssignments() const override;
    void ClearLaneAssignments() override;
    double GetDistance(MeasurementPoint measurementPoint, const std::string& roadId) const override;
    void SetLocatedPosition(const ObjectPosition& position) override;

    void CopyToGroundTruth(osi3::GroundTruth& target) const override;
private:
    osi3::StationaryObject* osiObject;
    Interfaces::Lanes assignedLanes;
    ObjectPosition position;
};

class MovingObject : public Interfaces::MovingObject
{
public:
    MovingObject(osi3::MovingObject* osiMovingObject, void* linkedObject);
    virtual ~MovingObject() override = default;

    virtual Id GetId() const override;

    virtual Primitive::Dimension GetDimension() const override;
    virtual Primitive::AbsPosition GetReferencePointPosition() const override;
    virtual const ObjectPosition& GetLocatedPosition() const override;
    virtual double GetDistanceReferencePointToLeadingEdge() const override;

    virtual Primitive::AbsOrientation GetAbsOrientation() const override;
    virtual Primitive::LaneOrientation GetLaneOrientation() const override;

    virtual Primitive::AbsVelocity GetAbsVelocity() const override;
    virtual double GetAbsVelocityDouble() const override;

    virtual Primitive::AbsAcceleration GetAbsAcceleration() const override;
    virtual double GetAbsAccelerationDouble() const override;

    virtual Primitive::AbsOrientationRate GetAbsOrientationRate() const override;

    virtual double GetDistance(MeasurementPoint measurementPoint, const std::string& roadId) const override;

    virtual void SetDimension(const Primitive::Dimension& newDimension) override;
    virtual void SetLength(const double newLength) override;
    virtual void SetWidth(const double newWidth) override;
    virtual void SetHeight(const double newHeight) override;
    virtual void SetBoundingBoxCenterToRear(const double distance) override;

    virtual void SetReferencePointPosition(const Primitive::AbsPosition& newPosition) override;
    virtual void SetX(const double newX) override;
    virtual void SetY(const double newY) override;
    virtual void SetZ(const double newZ) override;

    virtual void SetLocatedPosition(const ObjectPosition& position) override;

    virtual void SetAbsOrientation(const Primitive::AbsOrientation& newOrientation) override;
    virtual void SetYaw(const double newYaw) override;
    virtual void SetPitch(const double newPitch) override;
    virtual void SetRoll(const double newRoll) override;

    virtual void SetAbsVelocity(const Primitive::AbsVelocity& newVelocity) override;
    virtual void SetAbsVelocity(const double newVelocity) override;

    virtual void SetAbsAcceleration(const Primitive::AbsAcceleration& newAcceleration) override;
    virtual void SetAbsAcceleration(const double newAcceleration) override;

    virtual void SetAbsOrientationRate(const Primitive::AbsOrientationRate& newOrientationRate) override;

    void AddLaneAssignment(const Interfaces::Lane& lane) override;
    const Interfaces::Lanes& GetLaneAssignments() const override;
    void ClearLaneAssignments() override;

    virtual void SetIndicatorState(IndicatorState indicatorState) override;
    virtual IndicatorState GetIndicatorState() const override;
    virtual void SetBrakeLightState(bool brakeLightState) override;
    virtual bool GetBrakeLightState() const override;
    virtual void SetHeadLight(bool headLight) override;
    virtual bool GetHeadLight() const override;
    virtual void SetHighBeamLight(bool highbeamLight) override;
    virtual bool GetHighBeamLight() const override;

    virtual void SetType(AgentVehicleType) override;

    void CopyToGroundTruth(osi3::GroundTruth& target) const override;
private:
    osi3::MovingObject* osiObject;
    ObjectPosition position;
    Interfaces::Lanes assignedLanes;

    mutable Lazy<double> frontDistance;
    mutable Lazy<double> rearDistance;

    const Implementation::InvalidLane invalidLane;
    const Implementation::InvalidSection invalidSection;
    const Implementation::InvalidRoad invalidRoad;
};

class Vehicle : public MovingObject
{
public:
    Angle GetSteeringWheelAngle();
    void SetSteeringWheelAngle(const Angle newValue);
};

class TrafficSign : public Interfaces::TrafficSign
{
public:

    TrafficSign(osi3::TrafficSign* osiObject);

    virtual ~TrafficSign() override = default;

    virtual std::string GetId() const override
    {
        return id;
    }

    virtual double GetS() const override
    {
        return s;
    }

    virtual std::pair<double, CommonTrafficSign::Unit> GetValueAndUnitInSI(const double osiValue, const osi3::TrafficSignValue_Unit osiUnit) const override;

    virtual CommonTrafficSign::Entity GetSpecification(const double relativeDistance) const override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;
    virtual Primitive::Dimension GetDimension() const override;

    virtual void SetS(double sPos) override
    {
        s = sPos;
    }

    virtual void SetValidForLane(OWL::Id laneId) override
    {
        osiSign->mutable_main_sign()->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual bool SetSpecification(RoadSignalInterface* signal, Position position) override;

    virtual bool AddSupplementarySign(RoadSignalInterface* odSignal, Position position) override;

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const override;

private:
    std::string id {""};
    double s {0.0};

    osi3::TrafficSign* osiSign {nullptr};
};

class TrafficLight : public Interfaces::TrafficLight
{
public:
    TrafficLight(osi3::TrafficLight* osiLightRed,
                 osi3::TrafficLight* osiLightYellow,
                 osi3::TrafficLight* osiLightGreen);

    virtual ~TrafficLight() = default;

    virtual std::string GetId() const override
    {
        return id;
    }

    virtual double GetS() const override
    {
        return s;
    }

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual bool SetSpecification(RoadSignalInterface* signal, const Position& position) override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;

    virtual void SetS(double sPos) override
    {
        s = sPos;
    }

    virtual void SetValidForLane(OWL::Id laneId) override
    {
        osiLightRed->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
        osiLightYellow->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
        osiLightGreen->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const;

    virtual void SetState(CommonTrafficLight::State newState);

    virtual CommonTrafficLight::Entity GetSpecification(const double relativeDistance) const override;

    virtual CommonTrafficLight::State GetState() const;

private:
    std::string id {""};
    double s {0.0};

    osi3::TrafficLight* osiLightRed {nullptr};
    osi3::TrafficLight* osiLightYellow {nullptr};
    osi3::TrafficLight* osiLightGreen {nullptr};
};
class RoadMarking : public Interfaces::RoadMarking
{
public:

    RoadMarking(osi3::RoadMarking* osiObject);

    virtual ~RoadMarking() override = default;

    virtual std::string GetId() const override
    {
        return id;
    }

    virtual double GetS() const override
    {
        return s;
    }

    virtual CommonTrafficSign::Entity GetSpecification(const double relativeDistance) const override;

    virtual Primitive::AbsPosition GetReferencePointPosition() const override;
    virtual Primitive::Dimension GetDimension() const override;

    virtual void SetS(double sPos) override
    {
        s = sPos;
    }

    virtual void SetValidForLane(OWL::Id laneId) override
    {
        osiSign->mutable_classification()->add_assigned_lane_id()->set_value(laneId);
    }

    virtual bool SetSpecification(RoadSignalInterface* signal, Position position) override;

    virtual bool SetSpecification(RoadObjectInterface* object, Position position) override;

    virtual bool IsValidForLane(OWL::Id laneId) const override;

    virtual void CopyToGroundTruth(osi3::GroundTruth& target) const override;

private:
    std::string id {""};
    double s {0.0};

    osi3::RoadMarking* osiSign {nullptr};
};

} // namespace Implementation


/*
using LaneGeometryElement  = Primitive::LaneGeometryElement;
using Lane                 = Implementation::Lane;
using Section              = Implementation::Section;
using Road                 = Implementation::Road;
using StationaryObject     = Implementation::StationaryObject;
using MovingObject         = Implementation::MovingObject;
using Vehicle              = Implementation::Vehicle;

using CLaneGeometryElement  = const Primitive::LaneGeometryElement;
using CLane                 = const Implementation::Lane;
using CSection              = const Implementation::Section;
using CRoad                 = const Implementation::Road;
using CStationaryObject     = const Implementation::StationaryObject;
using CMovingObject         = const Implementation::MovingObject;
using CVehicle              = const Implementation::Vehicle;
*/

//using Sections             = std::list<Implementation::Section<>>;

//using StationaryObject     = Interfaces::StationaryObject;
//using MovingObject         = Interfaces::MovingObject;
//using Vehicle              = Interfaces::Vehicle;

//using LaneGeometryElements = std::list<Primitive::LaneGeometryElement>;
//using Lanes                = std::list<Interfaces::Lane>;
//using Roads                = std::list<Interfaces::Road>;
//using StationaryObjects    = std::list<Interfaces::StationaryObject>;
//using MovingObjects        = std::list<Interfaces::MovingObject>;
//using Vehicles             = std::list<Interfaces::Vehicle>;

} // namespace OWL
