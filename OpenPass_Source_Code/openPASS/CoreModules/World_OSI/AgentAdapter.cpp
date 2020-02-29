/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
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
    WorldObjectAdapter{worldData->AddMovingObject(static_cast<void*>(static_cast<WorldObjectInterface*>(this)))},
    world{world},
    callbacks{callbacks},
    worldData{worldData},
    localizer{localizer}
{
}

AgentAdapter::~AgentAdapter()
{
}

bool AgentAdapter::InitAgentParameter(int id,
                                      int spawnTime,
                                      AgentBlueprintInterface* agentBlueprint)
{
    VehicleModelParameters vehicleModelParameters = agentBlueprint->GetVehicleModelParameters();
    this->vehicleModelParameters = vehicleModelParameters;

    this->vehicleModelType = agentBlueprint->GetVehicleModelName();
    this->driverProfileName = agentBlueprint->GetDriverProfileName();
    this->vehicleType = vehicleModelParameters.vehicleType;
    this->id = id;
    this->agentCategory = agentBlueprint->GetAgentCategory();
    this->agentTypeName = agentBlueprint->GetAgentProfileName();
    this->objectName = agentBlueprint->GetObjectName();
    this->speedGoalMin = agentBlueprint->GetSpeedGoalMin();
    this->spawnTime = spawnTime;

    if (vehicleType != AgentVehicleType::Car &&
            vehicleType != AgentVehicleType::Truck &&
            vehicleType != AgentVehicleType::Motorbike &&
            vehicleType != AgentVehicleType::Bicycle &&
            vehicleType != AgentVehicleType::Pedestrian)
    {
        LOG(CbkLogLevel::Error, "undefined traffic object type");
        return false;
    }

    // set default values
    GetBaseTrafficObject().SetHeight(1.0);
    GetBaseTrafficObject().SetZ(0.0);
    GetBaseTrafficObject().SetPitch(0.0);
    GetBaseTrafficObject().SetRoll(0.0);


    UpdateWidth(vehicleModelParameters.width);
    UpdateLength(vehicleModelParameters.length);
    UpdateDistanceReferencePointToFrontAxle(vehicleModelParameters.distanceReferencePointToFrontAxle);
    UpdateDistanceReferencePointToLeadingEdge(vehicleModelParameters.distanceReferencePointToLeadingEdge);
    UpdateYaw(agentBlueprint->GetSpawnParameter().yawAngle); //Yaw is needed to calculate velocity
    UpdatePositionX(agentBlueprint->GetSpawnParameter().positionX);
    UpdatePositionY(agentBlueprint->GetSpawnParameter().positionY);
    UpdateAccelPedal(0.);
    UpdateBrakePedal(0.);
    UpdateVelocity(agentBlueprint->GetSpawnParameter().velocity);
    UpdateAcceleration(agentBlueprint->GetSpawnParameter().acceleration);
    UpdateGear(agentBlueprint->GetSpawnParameter().gear);
    UpdateWeight(vehicleModelParameters.weight);
    UpdateHeightCOG(vehicleModelParameters.heightCOG);
    UpdateWheelbase(vehicleModelParameters.wheelbase);
    UpdateFrictionCoeff(vehicleModelParameters.frictionCoeff);
    UpdateTrackWidth(vehicleModelParameters.trackwidth);
    UpdateMomentInertiaRoll(vehicleModelParameters.momentInertiaRoll);
    UpdateMomentInertiaPitch(vehicleModelParameters.momentInertiaPitch);
    UpdateMomentInertiaYaw(vehicleModelParameters.momentInertiaYaw);
    UpdateMaxVelocity(vehicleModelParameters.maxVelocity);
    UpdateMaxCurvature(vehicleModelParameters.maxCurvature);

    SetSensorParameters(agentBlueprint->GetSensorParameters());

    // spawn tasks are executed before any other task types within current scheduling time
    // other task types will have a consistent view of the world
    // calculate initial position
    Locate();

    return true;
}

bool AgentAdapter::Update()
{
    boundingBoxNeedsUpdate = (std::abs(GetVelocity()) >= zeroBaseline);
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

int AgentAdapter::GetNumberOfLanes()
{
    return world->GetNumberOfLanes(GetRoadId(), GetDistanceToStartOfRoad());
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
    return world->GetLaneWidth(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetRoadPosition().s, distance);
}

double AgentAdapter::GetLaneWidthRightDrivingAndStopLane() const
{
    if (!IsLaneDrivingLane(GetLaneIdRight()) && !IsLaneStopLane(GetLaneIdRight()))
    {
        return 0.0;
    }
    return world->GetLaneWidth(route, GetRoadId(), GetLaneIdRight(), GetDistanceToStartOfRoad(), 0);
}

double AgentAdapter::GetLaneCurvature(int relativeLane, double distance) const
{
    return world->GetLaneCurvature(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetRoadPosition().s, distance);
}

bool AgentAdapter::IsEgoAgent() const
{
    return agentCategory == AgentCategory::Ego;
}

WorldObjectInterface* AgentAdapter::GetObjectInFront(double previewDistance, int relativeLaneId) const
{
    return world->GetNextObjectInLane(route, GetRoadId(), GetLaneIdFromRelative(relativeLaneId), GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId),true , previewDistance);
}

WorldObjectInterface *AgentAdapter::GetObjectBehind(double previewDistance, int relativeLaneId) const
{
    return world->GetNextObjectInLane(route, GetRoadId(), GetLaneIdFromRelative(relativeLaneId), GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId),false , previewDistance);
}

TrafficObjectInterface* AgentAdapter::GetTrafficObjectInFront(int laneId) const
{
    return world->GetNextTrafficObjectInLane(route, GetRoadId(), laneId,
            GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId), true);
}

AgentInterface* AgentAdapter::GetAgentInFront(int laneId) const
{
    return world->GetNextAgentInLane(route, GetRoadId(), laneId, GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId), true);
}

std::vector<const TrafficObjectInterface*> AgentAdapter::GetAllTrafficObjectsInFront() const
{
    std::vector<const TrafficObjectInterface*> frontTrafficObjects;
    for (auto laneId : locateResult.frontLaneIds)
    {
        const auto trafficObjectInFront = GetTrafficObjectInFront(laneId);
        if (trafficObjectInFront)
        {
            frontTrafficObjects.push_back(trafficObjectInFront);
        }
    }
    return frontTrafficObjects;
}

std::vector<const WorldObjectInterface*> AgentAdapter::GetAllWorldObjectsInFront() const
{
    std::vector<const WorldObjectInterface*> frontObjects;
    for (auto laneId : locateResult.frontLaneIds)
    {
        const auto objectInFront = GetObjectInFront(laneId);
        if (objectInFront)
        {
            frontObjects.push_back(objectInFront);
        }
    }
    return frontObjects;
}

std::vector<AgentInterface*> AgentAdapter::GetAllAgentsInFront() const
{
    std::vector<AgentInterface*> frontAgents;
    for (auto laneId : locateResult.frontLaneIds)
    {
        auto agentInFront = GetAgentInFront(laneId);
        if (agentInFront)
        {
            frontAgents.push_back(agentInFront);
        }
    }
    return frontAgents;
}

const AgentInterface* AgentAdapter::GetAgentBehind(int laneId) const
{
    return world->GetLastAgentInLane(route, GetRoadId(), laneId,
                                     GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadStart, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId), false);
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
    //  (int)INFINITY dows not really make sense and equals to 0 on windows, could be < MAXINT instead assert(relativeLane != (int)INFINITY);
    return world->GetTrafficSignsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetDistanceToStartOfRoad(), searchDistance);
}

std::vector<LaneMarking::Entity> AgentAdapter::GetLaneMarkingsInRange(double searchDistance, int relativeLane, Side side) const
{
    return world->GetLaneMarkings(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetDistanceToStartOfRoad(), searchDistance, side);
}

RelativeWorldView::Junctions AgentAdapter::GetRelativeJunctions(double range) const
{
    return world->GetRelativeJunctions(route, GetRoadId(), GetDistanceToStartOfRoad(), range);
}

RelativeWorldView::Lanes AgentAdapter::GetRelativeLanes(double range) const
{
    return world->GetRelativeLanes(route, GetRoadId(), GetMainLaneId(), GetDistanceToStartOfRoad(), range);
}

bool AgentAdapter::GetHeadLight() const
{
    return GetBaseTrafficObject().GetHeadLight();
}

bool AgentAdapter::GetHighBeamLight() const
{
    return GetBaseTrafficObject().GetHighBeamLight();
}

double AgentAdapter::GetDistanceToFrontTrafficObject() const
{
    std::vector<const TrafficObjectInterface*> trafficObjectsInFront = GetAllTrafficObjectsInFront();
    double minDist = INFINITY;

    for (auto trafficObjectInFront : trafficObjectsInFront)
    {
        double currDist = trafficObjectInFront->GetDistanceToStartOfRoad() - GetDistanceToStartOfRoad();
        minDist = std::min(minDist, currDist);
    }

    return minDist;
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

bool AgentAdapter::ExistsLaneLeft() const
{
    return world->ExistsLaneLeft(GetRoadId(), GetMainLaneId(),
                                 GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId));
}

bool AgentAdapter::ExistsLaneRight() const
{
    return world->ExistsLaneRight(GetRoadId(), GetMainLaneId(),
                                  GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId));
}

bool AgentAdapter::IsLaneStopLane(int laneId, double distance) const
{
    const auto& availableStopLanes = world->GetStopLanesAtDistance(GetRoadId(), GetDistanceToStartOfRoad() + distance);
    for (int lane : availableStopLanes)
    {
        if (lane == laneId)
        {
            return true;
        }
    }
    return false;
}

bool AgentAdapter::IsLaneDrivingLane(int laneId, double distance) const
{
    const auto& availableDrivingLanes = world->GetDrivingLanesAtDistance(GetRoadId(), GetDistanceToStartOfRoad() + distance);
    for (int lane : availableDrivingLanes)
    {
        if (lane == laneId)
        {
            return true;
        }
    }
    return false;
}

bool AgentAdapter::IsLaneExitLane(int laneId, double distance) const
{
    const auto& availableExitLanes = world->GetExitLanesAtDistance(GetRoadId(), GetDistanceToStartOfRoad() + distance);
    for (int lane : availableExitLanes)
    {
        if (lane == laneId)
        {
            return true;
        }
    }
    return false;
}

bool AgentAdapter::IsLaneRamp(int laneId, double distance) const
{
    const auto& availableRamps = world->GetRampsAtDistance(GetRoadId(), GetDistanceToStartOfRoad() + distance);
    for (int lane : availableRamps)
    {
        if (lane == laneId)
        {
            return true;
        }
    }
    return false;
}

double AgentAdapter::GetDistanceToEndOfLane(double sightDistance, int relativeLane) const
{
    return world->GetDistanceToEndOfLane(route, GetRoadId(), GetLaneIdFromRelative(relativeLane),
                                         GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId),
                                         sightDistance);
}

std::vector<const WorldObjectInterface*> AgentAdapter::GetObjectsInRange(int relativeLane, double backwardRange,
        double forwardRange, MeasurementPoint mp) const
{
    return world->GetObjectsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetDistanceToStartOfRoad(mp), backwardRange, forwardRange);
}

std::vector<const AgentInterface*> AgentAdapter::GetAgentsInRange(int relativeLane, double backwardRange,
        double forwardRange, MeasurementPoint mp) const
{
    return world->GetAgentsInRange(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetDistanceToStartOfRoad(mp), backwardRange, forwardRange);
}

std::vector<const AgentInterface*> AgentAdapter::GetAgentsInRangeAbsolute(int laneId, double minDistance,
        double maxDistance) const
{
    return world->GetAgentsInRange(route, GetRoadId(), laneId, minDistance, maxDistance - minDistance, 0.0);
}

double AgentAdapter::GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorEntrance(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorDeparture(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetDistanceToEndOfRamp(int laneId, double sightDistance) const
{

    return world->GetDistanceToEndOfRamp(route, GetRoadId(), laneId,
                                         GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId),
                                         sightDistance);
}

double AgentAdapter::GetPositionLateral() const
{
    return GetRoadPosition().t;
}

double AgentAdapter::GetLaneDirection(int relativeLane, double distance) const
{
    return world->GetLaneDirection(route, GetRoadId(), GetLaneIdFromRelative(relativeLane), GetRoadPosition().s, distance);
}

void AgentAdapter::Unregister() const
{
    worldData->RemoveMovingObjectById(GetBaseTrafficObject().GetId());
}

double AgentAdapter::GetLaneRemainder(Side side) const
{
    return side == Side::Left ? locateResult.remainder.left : locateResult.remainder.right;
}

double AgentAdapter::GetDistanceToEndOfExit(int laneID, double sightDistance) const
{
    return world->GetDistanceToEndOfExit(route, GetRoadId(), laneID,
                                         GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, GetBaseTrafficObject().GetLocatedPosition().referencePoint.roadId),
                                         sightDistance);
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

std::set<int> AgentAdapter::GetSecondaryCoveredLanes()
{
    return locateResult.touchedLaneIds;
}

double AgentAdapter::GetDistanceToNextJunction() const
{
    const auto nextJunctionId = world->GetNextJunctionIdOnRoute(route, GetBaseTrafficObject().GetLocatedPosition());
    return world->GetDistanceToJunction(route, GetBaseTrafficObject().GetLocatedPosition(), nextJunctionId);
}
