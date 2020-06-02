/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*               2020 HLRS, University of Stuttgart.
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <Common/opMath.h>
#include <exception>
#include <qglobal.h>

#include "TrafficObjectAdapter.h"

//TODO: replace GlobalObject with injected shared_pointer to locator

TrafficObjectAdapter::TrafficObjectAdapter(OWL::Interfaces::WorldData& worldData, const World::Localization::Localizer& localizer,
        OWL::Primitive::AbsPosition position,
        OWL::Primitive::Dimension dimension, OWL::Primitive::AbsOrientation orientation) :
    WorldObjectAdapter{worldData.AddStationaryObject(static_cast<void*>(static_cast<WorldObjectInterface*>(this)))}, // objects passed as void * need to be casted to WorldObjectInterface*, because they are retrieved by casting to that class
    localizer{localizer}
{
    baseTrafficObject.SetReferencePointPosition(position);
    baseTrafficObject.SetDimension(dimension);
    baseTrafficObject.SetAbsOrientation(orientation);
    InitLaneDirection(orientation.yaw);
    Locate();
}

ObjectTypeOSI TrafficObjectAdapter::GetType() const
{
    return ObjectTypeOSI::Object;
}

double TrafficObjectAdapter::GetDistanceToStartOfRoad() const
{
    return WorldObjectInterface::GetDistanceToStartOfRoad();
}

void TrafficObjectAdapter::InitLaneDirection(double hdg)
{
    laneDirection = GetYaw() - hdg;
}

double TrafficObjectAdapter::GetDistanceToStartOfRoad(MeasurementPoint mp) const
{
    return GetDistanceToStartOfRoad(mp, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId);
}

double TrafficObjectAdapter::GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const
{
    switch (mp)
    {
        case MeasurementPoint::Front:
            return GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, roadId);
        case MeasurementPoint::Rear:
            return GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadStart, roadId);
        case MeasurementPoint::Reference:
            return GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadPosition.s;
        default:
            throw std::invalid_argument("Invalid measurement point");
    }
}

double TrafficObjectAdapter::GetVelocity(VelocityScope velocityScope) const
{
    Q_UNUSED(velocityScope);

    //TrafficObjects don't move
    return 0.0;
}

double TrafficObjectAdapter::GetLaneDirection() const
{
    return laneDirection;
}

double TrafficObjectAdapter::GetLaneRemainder(Side side) const
{
    return side == Side::Left ? locateResult.remainder.left : locateResult.remainder.right;
}

bool TrafficObjectAdapter::Locate()
{
    // reset on-demand values
    boundaryPoints.clear();

    locateResult = localizer.Locate(GetBoundingBox2D(), baseTrafficObject, Route{});

    baseTrafficObject.SetLocatedPosition(locateResult.position);
    return true;
}

void TrafficObjectAdapter::Unlocate()
{
    localizer.Unlocate(baseTrafficObject);
}

TrafficObjectAdapter::~TrafficObjectAdapter()
{
    Unlocate();
}
