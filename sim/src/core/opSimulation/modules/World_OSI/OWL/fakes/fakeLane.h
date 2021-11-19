/********************************************************************************
 * Copyright (c) 2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"

#include "OWL/DataTypes.h"
#include "WorldData.h"

#include <string>

namespace OWL::Fakes {
class Lane : public OWL::Interfaces::Lane
{
public:
    MOCK_CONST_METHOD0(GetId,
                       OWL::Id());
    MOCK_CONST_METHOD0(GetOdId,
                       OdId ());
    MOCK_CONST_METHOD0(GetLaneType,
                       LaneType());
    MOCK_CONST_METHOD0(Exists,
                       bool());
    MOCK_CONST_METHOD0(GetSection,
                       const OWL::Interfaces::Section & ());
    MOCK_CONST_METHOD0(GetRoad,
                       const OWL::Interfaces::Road & ());
    MOCK_CONST_METHOD0(GetLaneGeometryElements,
                       const OWL::Interfaces::LaneGeometryElements & ());
    MOCK_CONST_METHOD0(GetLength,
                       double());
    MOCK_CONST_METHOD0(GetRightLaneCount,
                       int());
    MOCK_CONST_METHOD1(GetCurvature,
                       double(double distance));
    MOCK_CONST_METHOD1(GetWidth,
                       double(double distance));
    MOCK_CONST_METHOD1(GetDirection,
                       double(double distance));
    MOCK_CONST_METHOD1(GetInterpolatedPointsAtDistance,
                       const OWL::Primitive::LaneGeometryJoint::Points(double));
    MOCK_CONST_METHOD0(GetLeftLane,
                       const OWL::Interfaces::Lane & ());
    MOCK_CONST_METHOD0(GetRightLane,
                       const OWL::Interfaces::Lane & ());
    MOCK_CONST_METHOD1(GetDistance,
                       double(OWL::MeasurementPoint mp));
    MOCK_CONST_METHOD1(Covers,
                       bool(double));
    MOCK_METHOD6(AddLaneGeometryJoint,
                 void(const Common::Vector2d& pointLeft,
                      const Common::Vector2d& pointCenter,
                      const Common::Vector2d& pointRight,
                      double sOffset, double curvature, double heading));
    MOCK_METHOD1(SetLaneType,
                 void(LaneType specifiedType));
    MOCK_METHOD2(SetLeftLane,
                 void(const OWL::Interfaces::Lane& lane, bool transferLaneBoundary));
    MOCK_METHOD2(SetRightLane,
                 void(const OWL::Interfaces::Lane& lane, bool transferLaneBoundary));
    MOCK_CONST_METHOD1(GetWorldObjects,
                       const OWL::Interfaces::LaneAssignments& (bool direction));
    MOCK_METHOD2(AddMovingObject,
                 void(OWL::Interfaces::MovingObject& movingObject, const LaneOverlap& laneOverlap));
    MOCK_METHOD2(AddStationaryObject,
                 void(OWL::Interfaces::StationaryObject& stationaryObject, const LaneOverlap& laneOverlap));
    MOCK_METHOD2(AddWorldObject, void (OWL::Interfaces::WorldObject& worldObject, const LaneOverlap& laneOverlap));
    MOCK_METHOD0(ClearMovingObjects, void());

    MOCK_METHOD1(AddNext,
                 void(const OWL::Interfaces::Lane* lane));
    MOCK_METHOD1(AddPrevious,
                 void(const OWL::Interfaces::Lane* lane));
    MOCK_CONST_METHOD1(CopyToGroundTruth, void(osi3::GroundTruth&));
    MOCK_CONST_METHOD0(GetLeftLaneBoundaries,
                       const std::vector<OWL::Id> ());
    MOCK_CONST_METHOD0(GetRightLaneBoundaries,
                       const std::vector<OWL::Id> ());
    MOCK_METHOD1(SetLeftLaneBoundaries,
                 void (const std::vector<OWL::Id> laneBoundaries));
    MOCK_METHOD1(SetRightLaneBoundaries,
                 void (const std::vector<OWL::Id> laneBoundaries));
    MOCK_CONST_METHOD0(GetNext, const std::vector<OWL::Id>& ());
    MOCK_CONST_METHOD0(GetPrevious, const std::vector<OWL::Id>& ());
    MOCK_CONST_METHOD0(GetTrafficSigns, const OWL::Interfaces::TrafficSigns& ());
    MOCK_CONST_METHOD0(GetTrafficLights, const OWL::Interfaces::TrafficLights& ());
    MOCK_CONST_METHOD0(GetRoadMarkings, const OWL::Interfaces::RoadMarkings& ());
    MOCK_METHOD1(AddTrafficSign, void (OWL::Interfaces::TrafficSign &trafficSign));
    MOCK_METHOD1(AddRoadMarking, void (OWL::Interfaces::RoadMarking &roadMarking));
    MOCK_METHOD1(AddTrafficLight, void (OWL::Interfaces::TrafficLight &trafficLight));
};
}
