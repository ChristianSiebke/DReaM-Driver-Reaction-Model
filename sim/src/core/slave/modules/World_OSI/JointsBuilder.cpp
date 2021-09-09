/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "JointsBuilder.h"
#include "common/commonTools.h"

constexpr double EPSILON = 0.001;

double BorderPoints::GetSquaredError(const BorderPoints& begin, const BorderPoints& end) const
{
    assert(points.size() == begin.points.size());
    assert(points.size() == end.points.size());

    double maxSquaredError{0.0};

    for (size_t i = 0; i < points.size(); i++)
    {
        const auto& currentPoint = points[i].point;
        const auto& startPoint = begin.points[i].point;
        const auto vector = end.points[i].point - startPoint;
        const auto projection = startPoint + vector * vector.Dot(currentPoint - startPoint) * (1 / vector.Dot(vector));
        const auto difference = currentPoint - projection;
        const auto squaredError = difference.Dot(difference);
        maxSquaredError = std::max(maxSquaredError, squaredError);
    }
    return maxSquaredError;
}

JointsBuilder& JointsBuilder::CalculatePoints()
{
    for (const auto& [s, currentBorderPoints] : sampledGeometry.borderPoints)
    {
        Joint joint;
        joint.s = s;

        for (size_t i = 0; i < currentBorderPoints.size(); i++)
        {
            const auto laneId = currentBorderPoints[i].lane->GetId();

            LaneJoint laneJoint;
            laneJoint.lane = currentBorderPoints[i].lane;
            if (laneId < 0)
            {
                laneJoint.left = currentBorderPoints[i-1].point;
                laneJoint.right = currentBorderPoints[i].point;
            }
            else if (laneId > 0)
            {
                laneJoint.left = currentBorderPoints[i].point;
                laneJoint.right = currentBorderPoints[i+1].point;
            }
            else
            {
                laneJoint.left = currentBorderPoints[i].point;
                laneJoint.right = currentBorderPoints[i].point;
            }
            laneJoint.center = (laneJoint.left + laneJoint.right) * 0.5;
            joint.laneJoints.insert(std::make_pair(laneId, laneJoint));
        }
        joints.push_back(joint);
    }

    return *this;
}

JointsBuilder& JointsBuilder::CalculateHeadings()
{
    for (auto joint = joints.begin(); joint < joints.end(); joint++)
    {
        for (auto& [laneId, laneJoint] : joint->laneJoints)
        {
            if (joint == joints.end() - 1)
            {
                laneJoint.heading = (joints.end() - 2)->laneJoints.at(laneId).heading;
                continue;
            }
            auto& currentCenter = laneJoint.center;
            auto& nextCenter = (joint + 1)->laneJoints.at(laneId).center;
            auto vector = nextCenter - currentCenter;
            if (vector.y > 0)
            {
                laneJoint.heading = M_PI_2 - std::atan(vector.x / vector.y);
            }
            if (vector.y < 0)
            {
                laneJoint.heading = -M_PI_2 - std::atan(vector.x / vector.y);
            }
        }
    }

    return *this;
}

JointsBuilder& JointsBuilder::CalculateCurvatures()
{

    for (auto joint = joints.begin(); joint < joints.end(); joint++)
    {
        for (auto& [laneId, laneJoint] : joint->laneJoints)
        {
            double previousHeading;
            if (joint == joints.begin())
            {
                previousHeading = sampledGeometry.startHeading;
            }
            else
            {
                previousHeading = (joint - 1)->laneJoints.at(laneId).heading;
            }
            double nextHeading;
            if (joint == joints.end() - 1)
            {
                nextHeading = sampledGeometry.endHeading;
            }
            else
            {
                nextHeading = laneJoint.heading;
            }
            auto deltaHeading = CommonHelper::SetAngleToValidRange(nextHeading - previousHeading);
            double deltaS;
            if (joint == joints.begin())
            {
                auto currentCenter = laneJoint.center;
                auto nextElementCenter = ((joint + 1)->laneJoints.at(laneId).center + laneJoint.center) * 0.5;
                deltaS = (nextElementCenter - currentCenter).Length();
            }
            else if (joint == joints.end() - 1)
            {
                auto previousElementCenter = ((joint - 1)->laneJoints.at(laneId).center + laneJoint.center) * 0.5;
                auto currentCenter = laneJoint.center;
                deltaS = (currentCenter - previousElementCenter).Length();
            }
            else
            {
                auto previousElementCenter = ((joint - 1)->laneJoints.at(laneId).center + laneJoint.center) * 0.5;
                auto nextElementCenter = ((joint + 1)->laneJoints.at(laneId).center + laneJoint.center) * 0.5;
                deltaS = (nextElementCenter - previousElementCenter).Length();
            }
            laneJoint.curvature = deltaHeading / deltaS;
        }
    }

    return *this;
}

Joints& JointsBuilder::GetJoints()
{
    return joints;
}

void SampledGeometry::Combine(SampledGeometry& other)
{
    if (other.borderPoints.size() < 2)
    {
        return;
    }
    assert(std::abs(borderPoints.back().s - other.borderPoints.front().s) < EPSILON);
    borderPoints.insert(borderPoints.end(), other.borderPoints.begin() + 1, other.borderPoints.end());
    endHeading = other.endHeading;
}
