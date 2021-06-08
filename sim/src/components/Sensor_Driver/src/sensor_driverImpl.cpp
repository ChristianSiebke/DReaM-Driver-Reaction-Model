/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*               2018, 2019 AMFD GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
/** \file  SensorDriverImplementation.cpp */
//-----------------------------------------------------------------------------

#include <memory>
#include <qglobal.h>

#include "include/worldInterface.h"
#include "common/RoutePlanning/RouteCalculation.h"
#include "sensor_driverImpl.h"
#include "Signals/sensorDriverSignal.h"

SensorDriverImplementation::SensorDriverImplementation(
        std::string componentName,
        bool isInit,
        int priority,
        int offsetTime,
        int responseTime,
        int cycleTime,
        StochasticsInterface *stochastics,
        WorldInterface *world,
        const ParameterInterface *parameters,
        PublisherInterface * const publisher,
        const CallbackInterface *callbacks,
        AgentInterface *agent) :
    SensorInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent),
    egoAgent(agent->GetEgoAgent()),
    sensorDriverCalculations(egoAgent)
{
    UpdateGraphPosition();
}

void SensorDriverImplementation::UpdateInput(int localLinkId,
                                             const std::shared_ptr<const SignalInterface> &data,
                                             int time)
{
    Q_UNUSED(time);
    Q_UNUSED(localLinkId);
    Q_UNUSED(data)
}

void SensorDriverImplementation::UpdateOutput(int localLinkId,
                                              std::shared_ptr<const SignalInterface> &data,
                                              int time)
{
    Q_UNUSED(time);

    try
    {
        switch(localLinkId)
        {
        case 0:
            // driver sensor data
            data = std::make_shared<SensorDriverSignal const>(ownVehicleInformation, trafficRuleInformation, geometryInformation, surroundingObjects);
            break;
        default:
            const std::string msg = COMPONENTNAME + " invalid link";
            LOG(CbkLogLevel::Debug, msg);
            throw std::runtime_error(msg);
        }
    }
    catch(const std::bad_alloc&)
    {
        const std::string msg = COMPONENTNAME + " could not instantiate signal";
        LOG(CbkLogLevel::Debug, msg);
        throw std::runtime_error(msg);
    }
}

void SensorDriverImplementation::Trigger(int time)
{
    Q_UNUSED(time);
    UpdateGraphPosition();
    GetOwnVehicleInformation();
    GetGeometryInformation();
    GetTrafficRuleInformation();
    GetSurroundingObjectsInformation();
}

void SensorDriverImplementation::GetNewRoute()
{
    constexpr int maxDepth = 10;
    const auto& roadIds = GetAgent()->GetRoads(MeasurementPoint::Front);
    if (roadIds.empty())
    {
        return;
    }
    auto [roadGraph, root] = GetWorld()->GetRoadGraph(CommonHelper::GetRoadWithLowestHeading(GetAgent()->GetObjectPosition().mainLocatePoint, *GetWorld()), maxDepth);
    std::map<RoadGraph::edge_descriptor, double> weights = GetWorld()->GetEdgeWeights(roadGraph);
    auto target = RouteCalculation::SampleRoute(roadGraph, root, weights, *GetStochastics());
    egoAgent.SetRoadGraph(std::move(roadGraph), root, target);
}

void SensorDriverImplementation::UpdateGraphPosition()
{
    if(!egoAgent.HasValidRoute())
    {
        GetNewRoute();
    }
}

void SensorDriverImplementation::GetOwnVehicleInformation()
{
    ownVehicleInformation.absoluteVelocity              = GetAgent()->GetVelocity(VelocityScope::Absolute);
    ownVehicleInformation.acceleration                  = GetAgent()->GetAcceleration();
    ownVehicleInformation.lateralPosition               = egoAgent.GetPositionLateral();
    ownVehicleInformation.heading                       = egoAgent.GetRelativeYaw();
    ownVehicleInformation.steeringWheelAngle            = GetAgent()->GetSteeringWheelAngle();
    ownVehicleInformation.distanceToLaneBoundaryLeft    = egoAgent.GetLaneRemainder(Side::Left);
    ownVehicleInformation.distanceToLaneBoundaryRight   = egoAgent.GetLaneRemainder(Side::Right);
    ownVehicleInformation.collision                     = GetAgent()->GetCollisionPartners().size() > 0;
}

void SensorDriverImplementation::GetTrafficRuleInformation()
{
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();
    trafficRuleInformation.laneEgo    = GetTrafficRuleLaneInformationEgo();
    trafficRuleInformation.laneLeft   = GetTrafficRuleLaneInformationLeft();
    trafficRuleInformation.laneRight  = GetTrafficRuleLaneInformationRight();
    trafficRuleInformation.laneMarkingsLeft = egoAgent.GetLaneMarkingsInRange(visibilityDistance, Side::Left);
    trafficRuleInformation.laneMarkingsRight = egoAgent.GetLaneMarkingsInRange(visibilityDistance, Side::Right);

    const auto laneIntervals = egoAgent.GetRelativeLanes(0.0);

    if (laneIntervals.size() > 0)
    {
        const auto relativeLanes = laneIntervals.front().lanes;

        for (auto relativeLane : relativeLanes)
        {
            if(relativeLane.relativeId == 1)
            {
                trafficRuleInformation.laneMarkingsLeftOfLeftLane = egoAgent.GetLaneMarkingsInRange(visibilityDistance, Side::Left, 1);
            }
            if(relativeLane.relativeId == -1)
            {
                trafficRuleInformation.laneMarkingsRightOfRightLane = egoAgent.GetLaneMarkingsInRange(visibilityDistance,Side::Right, -1);
            }
        }
    }
}

LaneInformationTrafficRules SensorDriverImplementation::GetTrafficRuleLaneInformationEgo()
{
    LaneInformationTrafficRules laneInformation;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns            = egoAgent.GetTrafficSignsInRange(visibilityDistance);
    laneInformation.trafficLights           = egoAgent.GetTrafficLightsInRange(visibilityDistance);

    return laneInformation;
}

LaneInformationTrafficRules SensorDriverImplementation::GetTrafficRuleLaneInformationLeft()
{
    LaneInformationTrafficRules laneInformation;
    const int relativeLaneId = 1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns            = egoAgent.GetTrafficSignsInRange(visibilityDistance, relativeLaneId);
    laneInformation.trafficLights           = egoAgent.GetTrafficLightsInRange(visibilityDistance, relativeLaneId);

    return laneInformation;
}

LaneInformationTrafficRules SensorDriverImplementation::GetTrafficRuleLaneInformationRight()
{
    LaneInformationTrafficRules laneInformation;
    const int relativeLaneId = -1;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.trafficSigns            = egoAgent.GetTrafficSignsInRange(visibilityDistance, relativeLaneId);
    laneInformation.trafficLights           = egoAgent.GetTrafficLightsInRange(visibilityDistance, relativeLaneId);

    return laneInformation;
}

void SensorDriverImplementation::GetGeometryInformation()
{
    geometryInformation.visibilityDistance            = GetWorld()->GetVisibilityDistance();
    geometryInformation.laneEgo    = GetGeometryLaneInformationEgo();
    geometryInformation.laneLeft   = GetGeometryLaneInformation(1);
    geometryInformation.laneRight  = GetGeometryLaneInformation(-1);
}

LaneInformationGeometry SensorDriverImplementation::GetGeometryLaneInformationEgo()
{
    LaneInformationGeometry laneInformation;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.exists                  = true;     // Ego lane must exist by definition, or else vehicle would have despawned by now. Information not necessary atm!
    laneInformation.curvature               = egoAgent.GetLaneCurvature();
    laneInformation.width                   = egoAgent.GetLaneWidth();
    laneInformation.distanceToEndOfLane     = egoAgent.GetDistanceToEndOfLane(visibilityDistance);

    return laneInformation;
}

LaneInformationGeometry SensorDriverImplementation::GetGeometryLaneInformation(int relativeLaneId)
{
    LaneInformationGeometry laneInformation;
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    laneInformation.exists = false;

    const auto laneIntervals = egoAgent.GetRelativeLanes(0.0);

    if (!laneIntervals.empty())
    {
        const auto& relativeLanes = laneIntervals.front().lanes;

        if (std::find_if(relativeLanes.cbegin(),
                         relativeLanes.cend(),
                         [relativeLaneId](const auto& relativeLane) {
                             return relativeLane.relativeId == relativeLaneId;
                         }) != relativeLanes.cend())
        {
            laneInformation.exists = true;
            laneInformation.curvature               = egoAgent.GetLaneCurvature(relativeLaneId);
            laneInformation.width                   = egoAgent.GetLaneWidth(relativeLaneId);
            laneInformation.distanceToEndOfLane     = egoAgent.GetDistanceToEndOfLane(visibilityDistance, relativeLaneId);
        }
    }

    return laneInformation;
}

void SensorDriverImplementation::GetSurroundingObjectsInformation()
{
    const double visibilityDistance = GetWorld()->GetVisibilityDistance();

    surroundingObjects.objectFront = GetOtherObjectInformation(GetObject(visibilityDistance, 0, true));
    surroundingObjects.objectRear = GetOtherObjectInformation(GetObject(visibilityDistance, 0, false));
    surroundingObjects.objectFrontLeft = GetOtherObjectInformation(GetObject(visibilityDistance, 1, true));
    surroundingObjects.objectRearLeft = GetOtherObjectInformation(GetObject(visibilityDistance, 1, false));
    surroundingObjects.objectFrontRight = GetOtherObjectInformation(GetObject(visibilityDistance, -1, true));
    surroundingObjects.objectRearRight = GetOtherObjectInformation(GetObject(visibilityDistance, -1, false));

    GetPublisher()->Publish("AgentInFront", surroundingObjects.objectFront.exist ? surroundingObjects.objectFront.id : -1);
}

const WorldObjectInterface* SensorDriverImplementation::GetObject(double visibilityDistance, int relativeLane, bool forwardSearch)
{
    const auto objectsInRange = egoAgent.GetObjectsInRange(forwardSearch ? 0 : visibilityDistance,
                                                           forwardSearch ? visibilityDistance : 0,
                                                           relativeLane);
    if (objectsInRange.empty())
    {
        return nullptr;
    }
    return forwardSearch ? objectsInRange.front() : objectsInRange.back();
}

ObjectInformation SensorDriverImplementation::GetOtherObjectInformation(const WorldObjectInterface* surroundingObject)
{
    ObjectInformation objectInformation;

    auto surroundingVehicle = dynamic_cast<const AgentInterface*>(surroundingObject);

    if (!surroundingObject)
    {
        objectInformation.exist = false;
    }
    else
    {
        objectInformation.exist = true;
        objectInformation.id = surroundingObject->GetId();
        objectInformation.relativeLongitudinalDistance = egoAgent.GetDistanceToObject(surroundingObject).netDistance.value();
        objectInformation.heading = surroundingObject->GetYaw();
        objectInformation.length = surroundingObject->GetLength();
        objectInformation.width = surroundingObject->GetWidth();
        objectInformation.height = surroundingObject->GetHeight();
        if (!surroundingVehicle)
        {
            objectInformation.isStatic = true;
        }
        else
        {
            objectInformation.isStatic = false;
//            objectInformation.heading = surroundingVehicle->GetRelativeYaw(get(RouteElement(), route, currentPositionInGraph).roadId);
            objectInformation.absoluteVelocity = surroundingVehicle->GetVelocity();
            objectInformation.acceleration = surroundingVehicle->GetAcceleration();
//            objectInformation.relativeLateralDistance = sensorDriverCalculations.GetLateralDistanceToObject(get(RouteElement(), route, currentPositionInGraph).roadId, surroundingVehicle);
        }
    }

    return objectInformation;
}
