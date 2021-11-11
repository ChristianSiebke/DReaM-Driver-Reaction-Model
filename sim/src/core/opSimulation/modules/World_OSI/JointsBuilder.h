/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "common/vector2d.h"
#include "include/roadInterface/roadLaneInterface.h"

//! This lane represents the outer boundary points of a lane at one s coordinate
struct BorderPoint
{
    explicit BorderPoint(Common::Vector2d point, RoadLaneInterface* lane) noexcept :
        point{point}, lane{lane} {}

    BorderPoint() = default;

    constexpr BorderPoint(const BorderPoint&) = default;
    constexpr BorderPoint(BorderPoint&&) = default;

    BorderPoint& operator=(const BorderPoint&) = default;
    BorderPoint& operator=(BorderPoint&&) = default;

    Common::Vector2d point;
    RoadLaneInterface *lane; //!This is needed for later adding the point to this lane
};

//! This struct represents the boundary points of all lanes (including lane 0) at one s coordinate (i.e. one joint)
struct BorderPoints
{
    double s;
    std::vector<BorderPoint> points;

    //! Returns the highest squared lateral error of this joint to the line(s) defined by two other joints
    double GetSquaredError(const BorderPoints& begin, const BorderPoints& end) const;
};

//! This struct is the result of the geometry sampling of one road section
struct SampledGeometry
{
    std::vector<BorderPoints> borderPoints;
    double startHeading; //!This is needed for calculating the curvature of the first joint
    double endHeading; //!This is needed for calculating the curvature of the last joint

    //! Combine this SampledGeometry with another, that starts where this ends.
    //!
    //! \param other    another SampledGeometry starting where this ends
    void Combine(SampledGeometry& other);
};

//! This struct contains all information calculated by the JointBuilder for one joint and one lane
struct LaneJoint
{
    RoadLaneInterface* lane;
    Common::Vector2d left{};
    Common::Vector2d center{};
    Common::Vector2d right{};
    double heading{0.0};
    double curvature{0.0};
};

//! This struct contains all information calculated by the JointBuilder for one joint
struct Joint
{
    double s;
    std::map<int, LaneJoint> laneJoints;
};

using Joints = std::vector<Joint>;

//! This class gets the sampled lane boundary points as input and calculates all the other
//! information about the joints, that is needed for adding the joints to the WorldData.
//! The functions have to be called in the order they appear in this header.
class JointsBuilder
{
public:
    JointsBuilder(const SampledGeometry& sampledGeometry) :
        sampledGeometry(sampledGeometry)
    {}

    //!For testing only
    JointsBuilder(const SampledGeometry& sampledGeometry, const Joints& joints) :
        sampledGeometry(sampledGeometry),
        joints(joints)
    {}

    //! Calculates the left, center and right points of each lane
    JointsBuilder& CalculatePoints();

    //! Calculates the headings by interpolating between the center points
    JointsBuilder& CalculateHeadings();

    //! Calculates the curvatures from the heading changes
    JointsBuilder& CalculateCurvatures();

    //! Returns the calculated joints
    Joints& GetJoints();

private:
    SampledGeometry sampledGeometry; //! input of the JointBuilder
    Joints joints;                   //! output of the JointBuilder
};
