/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*               2018 AMFD GmbH
*               2016 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <cassert>
#include <algorithm>
#include "AgentAdapter.h"
#include "Common/globalDefinitions.h"
#include "RoutePlanning/StochasticNavigation.h"

namespace loc = World::Localization;

AgentAdapter::AgentAdapter(WorldInterface* world,
                           const CallbackInterface* callbacks,
                           OWL::Interfaces::WorldData* worldData,
                           const World::Localization::Localizer& localizer) :
    WorldObjectAdapter{worldData->AddMovingObject(static_cast<void*>(this))},
    world{world},
    callbacks{callbacks},
    worldData{worldData},
    localizer{localizer}
{
}

bool AgentAdapter::InitAgentParameter(int id,
                                      AgentBlueprintInterface* agentBlueprint)
{
    UpdateVehicleModelParameter(agentBlueprint->GetVehicleModelParameters());

    const auto & vehicleType = this->vehicleModelParameters.vehicleType;
    if (vehicleType != AgentVehicleType::Car &&
            vehicleType != AgentVehicleType::Truck &&
            vehicleType != AgentVehicleType::Motorbike &&
            vehicleType != AgentVehicleType::Bicycle &&
            vehicleType != AgentVehicleType::Pedestrian)
    {
        LOG(CbkLogLevel::Error, "undefined traffic object type");
        return false;
    }

    this->vehicleModelType = agentBlueprint->GetVehicleModelName();
    this->driverProfileName = agentBlueprint->GetDriverProfileName();
    this->id = id;
    this->agentCategory = agentBlueprint->GetAgentCategory();
    this->agentTypeName = agentBlueprint->GetAgentProfileName();
    this->objectName = agentBlueprint->GetObjectName();
    this->speedGoalMin = agentBlueprint->GetSpeedGoalMin();

    // set default values
    GetBaseTrafficObject().SetZ(0.0);
    GetBaseTrafficObject().SetPitch(0.0);
    GetBaseTrafficObject().SetRoll(0.0);

    const auto &spawnParameter = agentBlueprint->GetSpawnParameter();
    UpdateYaw(spawnParameter.yawAngle);
    GetBaseTrafficObject().SetX(spawnParameter.positionX);
    GetBaseTrafficObject().SetY(spawnParameter.positionY);
    GetBaseTrafficObject().SetAbsVelocity(spawnParameter.velocity);
    GetBaseTrafficObject().SetAbsAcceleration(spawnParameter.acceleration);
    this->currentGear = static_cast<int>(spawnParameter.gear);

    route = spawnParameter.route.value_or(Route());

    SetSensorParameters(agentBlueprint->GetSensorParameters());

    // spawn tasks are executed before any other task types within current scheduling time
    // other task types will have a consistent view of the world
    // calculate initial position
    Locate();

    return true;
}

bool AgentAdapter::Update()
{
    // currently set to constant true to correctly update BB of rotating objects (with velocity = 0)
    // and objects with an incomplete set of dynamic parameters (i. e. changing x/y with velocity = 0)
    //boundingBoxNeedsUpdate = std::abs(GetVelocity()) >= zeroBaseline;
    boundingBoxNeedsUpdate = true;
    return Locate();
}

void AgentAdapter::SetBrakeLight(bool brakeLightStatus)
{
    GetBaseTrafficObject().SetBrakeLightState(brakeLightStatus);
}

bool AgentAdapter::GetBrakeLight() const
{
    return GetBaseTrafficObject().GetBrakeLightState();
}

bool AgentAdapter::Locate()
{
    // reset on-demand values
    boundaryPoints.clear();

    locateResult = localizer.Locate(GetBoundingBox2D(), GetBaseTrafficObject(), route);

    // locator reports we've fallen off route - try to calculate new route
    if (!locateResult.isOnRoute && locateResult.position.mainLocatePoint.roadId != "")
    {
        localizer.Unlocate(GetBaseTrafficObject());

        route = world->GetRoute(locateResult.position.mainLocatePoint);
        locateResult = localizer.Locate(GetBoundingBox2D(), GetBaseTrafficObject(), route);
    }

    GetBaseTrafficObject().SetLocatedPosition(locateResult.position);

    return locateResult.isOnRoute;
}

void AgentAdapter::Unlocate()
{
    localizer.Unlocate(GetBaseTrafficObject());
}

AgentCategory AgentAdapter::GetAgentCategory() const
{
    return agentCategory;
}

std::string AgentAdapter::GetAgentTypeName() const
{
    return agentTypeName;
}

void AgentAdapter::SetIndicatorState(IndicatorState indicatorState)
{
    GetBaseTrafficObject().SetIndicatorState(indicatorState);
}

IndicatorState AgentAdapter::GetIndicatorState() const
{
    return GetBaseTrafficObject().GetIndicatorState();
}

std::string AgentAdapter::GetRoadId(MeasurementPoint mp) const
{
    if (mp == MeasurementPoint::Front)
    {
        return locateResult.position.mainLocatePoint.roadId;
    }
    if (mp == MeasurementPoint::Reference)
    {
        return locateResult.position.referencePoint.roadId;
    }
    if (mp == MeasurementPoint::Rear)
    {
        throw std::invalid_argument("measurement point not valid");
    }

    return {};
}

int AgentAdapter::GetMainLaneId(MeasurementPoint mp) const
{
    if (mp == MeasurementPoint::Front)
    {
        return locateResult.position.mainLocatePoint.laneId;
    }
    if (mp == MeasurementPoint::Reference)
    {
        return locateResult.position.referencePoint.laneId;
    }
    if (mp == MeasurementPoint::Rear)
    {
        throw std::invalid_argument("measurement point not valid");
    }

    return -999;
}

int AgentAdapter::GetLaneIdLeft() const
{
    return locateResult.position.referencePoint.laneId + 1;
}

int AgentAdapter::GetLaneIdRight() const
{
    return locateResult.position.referencePoint.laneId - 1;
}

int AgentAdapter::GetLaneIdFromRelative(int relativeLaneId) const
{
    if (GetMainLaneId() < 0)
    {
        return GetMainLaneId() + relativeLaneId;
    }
    else
    {
        return  GetMainLaneId() - relativeLaneId;
    }
}

bool AgentAdapter::IsAgentInWorld() const
{
    return locateResult.isOnRoute;
}

void AgentAdapter::SetPosition(Position pos)
{
    SetPositionX(pos.xPos);
    SetPositionY(pos.yPos);
    SetYaw(pos.yawAngle);
}

double AgentAdapter::GetDistanceToStartOfRoad(MeasurementPoint mp) const
{
    if (mp == MeasurementPoint::Front)
    {
        return GetDistanceToStartOfRoad(mp, GetBaseTrafficObject().GetLocatedPosition().mainLocatePoint.roadId);
    }
    return GetDistanceToStartOfRoad(mp, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId);
}

double AgentAdapter::GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const
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

double AgentAdapter::GetVelocity(VelocityScope velocityScope) const
{
    if (velocityScope == VelocityScope::Absolute)
    {
        return GetBaseTrafficObject().GetAbsVelocityDouble();
    }
    if (velocityScope == VelocityScope::Lateral)
    {
        return GetBaseTrafficObject().GetAbsVelocityDouble() * std::sin(GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadPosition.hdg);
    }
    if (velocityScope == VelocityScope::Longitudinal)
    {
        return GetBaseTrafficObject().GetAbsVelocityDouble() * std::cos(GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadPosition.hdg);
    }
    if (velocityScope == VelocityScope::DirectionX)
    {
        return GetBaseTrafficObject().GetAbsVelocity().vx;
    }
    if (velocityScope == VelocityScope::DirectionY)
    {
        return GetBaseTrafficObject().GetAbsVelocity().vy;
    }

    throw std::invalid_argument("velocity scope not within valid bounds");
}

double AgentAdapter::GetLaneWidth(int relativeLane, double distance) const
{
    return world->GetLaneWidth(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), distance);
}

double AgentAdapter::GetLaneCurvature(int relativeLane, double distance) const
{
    return world->GetLaneCurvature(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), distance);
}

bool AgentAdapter::IsEgoAgent() const
{
    return agentCategory == AgentCategory::Ego;
}

double AgentAdapter::GetDistanceToObject(const WorldObjectInterface* otherObject) const
{
    if (otherObject == nullptr)
    {
        return std::numeric_limits<double>::max();
    }

    const auto castedOtherObject = dynamic_cast<const WorldObjectAdapter*>(otherObject);

    const auto objectPos = GetBaseTrafficObject().GetLocatedPosition();
    const auto otherObjectPos = castedOtherObject->GetBaseTrafficObject().GetLocatedPosition();
    const auto distance = world->GetDistanceBetweenObjects(route, objectPos, otherObjectPos);

    return distance;
}

void AgentAdapter::UpdateCollision(std::pair<ObjectTypeOSI, int> collisionPartner)
{
    auto findIter = std::find_if(collisionPartners.begin(), collisionPartners.end(),
                                 [collisionPartner](const std::pair<ObjectTypeOSI, int>& storedCollisionPartner)
    {
        return collisionPartner == storedCollisionPartner;
    });
    if (findIter == collisionPartners.end())
    {
        collisionPartners.push_back(collisionPartner);
    }
}

const WorldObjectInterface* AgentAdapter::GetNearestObjectInVector(std::vector<const WorldObjectInterface*> objects)
{
    const WorldObjectInterface* nearestObject = nullptr;

    double minDistance = INFINITY;
    double currentDistance;

    for (const WorldObjectInterface* object : objects)
    {
        currentDistance = GetDistanceToObject(object);
        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            nearestObject = object;
        }
    }
    return nearestObject;
}

bool AgentAdapter::IsLeavingWorld() const
{
    return false;
}

bool AgentAdapter::IsCrossingLanes() const
{
    return locateResult.isCrossingLanes;
}

std::vector<CommonTrafficSign::Entity> AgentAdapter::GetTrafficSignsInRange(double searchDistance, int relativeLane) const
{
    assert(relativeLane != (int)INFINITY);
    return world->GetTrafficSignsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), searchDistance);
}

std::vector<LaneMarking::Entity> AgentAdapter::GetLaneMarkingsInRange(double searchDistance, int relativeLane, Side side) const
{
    return world->GetLaneMarkings(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), searchDistance, side);
}

RelativeWorldView::Junctions AgentAdapter::GetRelativeJunctions(double range) const
{
    return world->GetRelativeJunctions(route, GetRoadId(), GetMainLocateS(), range);
}

RelativeWorldView::Lanes AgentAdapter::GetRelativeLanes(double range) const
{
    return world->GetRelativeLanes(route, GetRoadId(), GetMainLaneId(), GetMainLocateS(), range);
}

bool AgentAdapter::GetHeadLight() const
{
    return GetBaseTrafficObject().GetHeadLight();
}

bool AgentAdapter::GetHighBeamLight() const
{
    return GetBaseTrafficObject().GetHighBeamLight();
}

const Obstruction AgentAdapter::GetObstruction(const WorldObjectInterface& worldObject) const
{
    const auto opponent = dynamic_cast<const WorldObjectAdapter*>(&worldObject);

    const auto opponentPosition = opponent->GetBaseTrafficObject().GetLocatedPosition();
    const auto boundingBox = opponent->GetBoundingBox2D();
    std::vector<Common::Vector2d> objectCorners;
    for (const auto& point : boundingBox.outer())
    {
        objectCorners.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    objectCorners.pop_back(); //Boost polygon contains first point also as last point

    return world->GetObstruction(route, GetBaseTrafficObject().GetLocatedPosition().mainLocatePoint, opponentPosition, objectCorners);
}

double AgentAdapter::GetDistanceToEndOfLane(double sightDistance, int relativeLane) const
{
    return world->GetDistanceToEndOfLane(route, GetRoadId(), GetLaneIdFromRelative(relativeLane),
                                         GetMainLocateS(),
                                         sightDistance);
}

double AgentAdapter::GetDistanceToEndOfLane(double sightDistance, int relativeLane, const LaneTypes& laneTypes) const
{
    return world->GetDistanceToEndOfLane(route, GetRoadId(), GetLaneIdFromRelative(relativeLane),
                                         GetMainLocateS(),
                                         sightDistance, laneTypes);
}

std::vector<const WorldObjectInterface*> AgentAdapter::GetObjectsInRange(int relativeLane, double backwardRange,
        double forwardRange, MeasurementPoint mp) const
{
    auto objectsInRange = world->GetObjectsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), backwardRange, forwardRange);

    auto self = std::find(objectsInRange.cbegin(), objectsInRange.cend(), this);
    if (self != objectsInRange.cend())
    {
        objectsInRange.erase(self);
    }

    return objectsInRange;
}

std::vector<const AgentInterface*> AgentAdapter::GetAgentsInRange(int relativeLane, double backwardRange,
        double forwardRange, MeasurementPoint mp) const
{
    auto agentsInRange = world->GetAgentsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), backwardRange, forwardRange);

    auto self = std::find(agentsInRange.cbegin(), agentsInRange.cend(), this);
    if (self != agentsInRange.cend())
    {
        agentsInRange.erase(self);
    }

    return agentsInRange;
}

double AgentAdapter::GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorEntrance(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorDeparture(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetPositionLateral() const
{
    return GetRoadPosition().t;
}

double AgentAdapter::GetLaneDirection(int relativeLane, double distance) const
{
    return world->GetLaneDirection(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetMainLocateS(), distance);
}

void AgentAdapter::Unregister() const
{
    worldData->RemoveMovingObjectById(GetBaseTrafficObject().GetId());
}

double AgentAdapter::GetLaneRemainder(Side side) const
{
    return side == Side::Left ? locateResult.remainder.left : locateResult.remainder.right;
}

LightState AgentAdapter::GetLightState() const
{

    if (GetFlasher())
    {
        return LightState::Flash;
    }
    if (GetHighBeamLight())
    {
        return LightState::HighBeam;
    }
    if (GetHeadLight())
    {
        return LightState::LowBeam;
    }

    return LightState::Off;
}

double AgentAdapter::GetMainLocateS() const
{
    return GetBaseTrafficObject().GetLocatedPosition().mainLocatePoint.roadPosition.s;
}

std::set<int> AgentAdapter::GetSecondaryCoveredLanes()
{
    return locateResult.touchedLaneIds;
}

double AgentAdapter::GetDistanceToNextJunction() const
{
    const auto nextJunctionId = world->GetNextJunctionIdOnRoute(route, GetBaseTrafficObject().GetLocatedPosition());
    return world->GetDistanceToJunction(route, GetBaseTrafficObject().GetLocatedPosition(), nextJunctionId);
}
