/********************************************************************************
 * Copyright (c) 2018 AMFD GmbH
 *               2020 HLRS, University of Stuttgart
 *               2016 ITK Engineering GmbH
 *               2017-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include <cassert>
#include <algorithm>
#include "AgentAdapter.h"
#include "common/globalDefinitions.h"

namespace loc = World::Localization;

AgentAdapter::AgentAdapter(const openpass::type::EntityId id,
                           WorldInterface* world,
                           const CallbackInterface* callbacks,
                           OWL::Interfaces::WorldData* worldData,
                           const World::Localization::Localizer& localizer) :
    id{static_cast<int>(id.value)},
    WorldObjectAdapter{worldData->AddMovingObject(id.value, static_cast<void*>(static_cast<WorldObjectInterface*>(this)))},
    world{world},
    callbacks{callbacks},
    worldData{worldData},
    localizer{localizer},
    egoAgent{this, world}
{
}

AgentAdapter::~AgentAdapter()
{
}

void AgentAdapter::InitParameter(const AgentBlueprintInterface& agentBlueprint)
{
    UpdateVehicleModelParameter(agentBlueprint.GetVehicleModelParameters());

    const auto & vehicleType = this->vehicleModelParameters.vehicleType;
    if (vehicleType != AgentVehicleType::Car &&
            vehicleType != AgentVehicleType::Truck &&
            vehicleType != AgentVehicleType::Motorbike &&
            vehicleType != AgentVehicleType::Bicycle &&
            vehicleType != AgentVehicleType::Pedestrian)
    {
        LOG(CbkLogLevel::Error, "undefined traffic object type");
        throw std::runtime_error("undefined traffic object type");
    }

    this->vehicleModelType = agentBlueprint.GetVehicleModelName();
    this->driverProfileName = agentBlueprint.GetDriverProfileName();
    this->agentCategory = agentBlueprint.GetAgentCategory();
    this->agentTypeName = agentBlueprint.GetAgentProfileName();
    this->objectName = agentBlueprint.GetObjectName();
    this->speedGoalMin = agentBlueprint.GetSpeedGoalMin();

    // set default values
    GetBaseTrafficObject().SetZ(0.0);
    GetBaseTrafficObject().SetPitch(0.0);
    GetBaseTrafficObject().SetRoll(0.0);

    const auto &spawnParameter = agentBlueprint.GetSpawnParameter();
    UpdateYaw(spawnParameter.yawAngle);
    GetBaseTrafficObject().SetX(spawnParameter.positionX);
    GetBaseTrafficObject().SetY(spawnParameter.positionY);
    GetBaseTrafficObject().SetAbsVelocity(spawnParameter.velocity);
    GetBaseTrafficObject().SetAbsAcceleration(spawnParameter.acceleration);
    this->currentGear = static_cast<int>(spawnParameter.gear);

    SetSensorParameters(agentBlueprint.GetSensorParameters());

    // spawn tasks are executed before any other task types within current scheduling time
    // other task types will have a consistent view of the world
    // calculate initial position
    Locate();

    auto& route = spawnParameter.route;
    egoAgent.SetRoadGraph(std::move(route.roadGraph), route.root, route.target);
}

bool AgentAdapter::Update()
{
    // currently set to constant true to correctly update BB of rotating objects (with velocity = 0)
    // and objects with an incomplete set of dynamic parameters (i. e. changing x/y with velocity = 0)
    //boundingBoxNeedsUpdate = std::abs(GetVelocity()) >= zeroBaseline;
    boundingBoxNeedsUpdate = true;
    if(!Locate())
    {
        return false;
    }
    return true;
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

    locateResult = localizer.Locate(GetBoundingBox2D(), GetBaseTrafficObject());

    GetBaseTrafficObject().SetLocatedPosition(locateResult.position);

    egoAgent.Update();

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

double AgentAdapter::GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const
{
    switch (mp)
    {
        case MeasurementPoint::Front:
            return GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadEnd, roadId);
        case MeasurementPoint::Rear:
            return GetBaseTrafficObject().GetDistance(OWL::MeasurementPoint::RoadStart, roadId);
        case MeasurementPoint::Reference:
            return GetBaseTrafficObject().GetLocatedPosition().referencePoint.at(roadId).roadPosition.s;
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
        return GetBaseTrafficObject().GetAbsVelocityDouble() * std::sin(GetBaseTrafficObject().GetLocatedPosition().referencePoint.cbegin()->second.roadPosition.hdg);
    }
    if (velocityScope == VelocityScope::Longitudinal)
    {
        return GetBaseTrafficObject().GetAbsVelocityDouble() * std::cos(GetBaseTrafficObject().GetLocatedPosition().referencePoint.cbegin()->second.roadPosition.hdg);
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

bool AgentAdapter::IsEgoAgent() const
{
    return agentCategory == AgentCategory::Ego;
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

bool AgentAdapter::IsLeavingWorld() const
{
    return false;
}

bool AgentAdapter::GetHeadLight() const
{
    return GetBaseTrafficObject().GetHeadLight();
}

bool AgentAdapter::GetHighBeamLight() const
{
    return GetBaseTrafficObject().GetHighBeamLight();
}

std::vector<std::string> AgentAdapter::GetRoads(MeasurementPoint mp) const
{
    std::vector<std::string> roadIds;
    if (mp == MeasurementPoint::Front)
    {
        std::transform(locateResult.position.mainLocatePoint.cbegin(), locateResult.position.mainLocatePoint.cend(), std::back_inserter(roadIds), [](const auto& entry){return entry.first;});
        return roadIds;
    }
    if (mp == MeasurementPoint::Reference)
    {
        std::transform(locateResult.position.referencePoint.cbegin(), locateResult.position.referencePoint.cend(), std::back_inserter(roadIds), [](const auto& entry){return entry.first;});
        return roadIds;
    }
    if (mp == MeasurementPoint::Rear)
    {
        throw std::invalid_argument("measurement point not valid");
    }

    return {};
}

double AgentAdapter::GetDistanceToConnectorEntrance(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorEntrance(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

double AgentAdapter::GetDistanceToConnectorDeparture(std::string intersectingConnectorId, int intersectingLaneId, std::string ownConnectorId) const
{
    return world->GetDistanceToConnectorDeparture(locateResult.position, intersectingConnectorId, intersectingLaneId, ownConnectorId);
}

void AgentAdapter::Unregister() const
{
    worldData->RemoveMovingObjectById(GetBaseTrafficObject().GetId());
}

double AgentAdapter::GetLaneRemainder(const std::string& roadId, Side side) const
{
    return side == Side::Left ? locateResult.position.touchedRoads.at(roadId).remainder.left : locateResult.position.touchedRoads.at(roadId).remainder.right;
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
