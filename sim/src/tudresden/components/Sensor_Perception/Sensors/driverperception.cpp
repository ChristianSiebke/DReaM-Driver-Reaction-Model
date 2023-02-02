/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "driverperception.h"

#include <qglobal.h>

#include "WorldDataQuery.h"
#include "egoAgent.h"

Common::Vector2d DriverPerception::GetDriverPosition() {
    // driver is located in the center of agent
    double xDistanceRefPointToDriver = egoAgent->GetDistanceReferencePointToLeadingEdge() - (egoAgent->GetLength() / 2);
    Common::Vector2d distanceRefPointToDriver(xDistanceRefPointToDriver, 0);
    distanceRefPointToDriver.Rotate(egoAgent->GetYaw());

    Common::Vector2d driverPosition{egoAgent->GetPositionX(), egoAgent->GetPositionY()};
    driverPosition.Add(distanceRefPointToDriver);
    return driverPosition;
}

void DriverPerception::CalculatePerception(const AgentInterface *driver, std::vector<InternWaypoint> route) {
    const auto &actualEgoAgent = const_cast<AgentInterface *>(driver)->GetEgoAgent();
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    WorldDataQuery helper(*worldData);

    auto referenceLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetReferencePointPosition()->roadId, actualEgoAgent.GetReferencePointPosition()->laneId,
                              actualEgoAgent.GetReferencePointPosition()->roadPosition.s);
    auto referenceLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(referenceLane->GetId());
    auto mainLocatorLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetMainLocatePosition().roadId, actualEgoAgent.GetMainLocatePosition().laneId,
                              actualEgoAgent.GetMainLocatePosition().roadPosition.s);
    auto mainLocatorLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLocatorLane->GetId());
    DReaMRoute::Waypoints dreamRoute;

    std::transform(route.begin(), route.end(), std::back_inserter(dreamRoute), [this](auto element) {
        DReaMRoute::Waypoint result;
        result.roadId = element.roadId;
        result.lane = this->infrastructurePerception->lookupTableRoadNetwork.lanes.at(element.lane);
        result.s = element.s;
        return result;
    });

    DetailedAgentPerception perceptionData;

    // object information
    perceptionData.id = driver->GetId();
    perceptionData.yaw = driver->GetYaw();
    perceptionData.width = driver->GetWidth();
    perceptionData.length = driver->GetLength();
    perceptionData.refPosition = Common::Vector2d(driver->GetPositionX(), driver->GetPositionY());

    // general agent information
    perceptionData.vehicleType = (DReaMDefinitions::AgentVehicleType)driver->GetVehicleModelParameters().vehicleType;
    perceptionData.distanceReferencePointToLeadingEdge = driver->GetDistanceReferencePointToLeadingEdge();
    perceptionData.acceleration = driver->GetAcceleration();
    perceptionData.velocity = driver->GetVelocity(VelocityScope::Absolute);
    perceptionData.lanePosition = {referenceLaneDReaM, referenceLaneDReaM->IsInRoadDirection()
                                                           ? actualEgoAgent.GetReferencePointPosition()->roadPosition.s
                                                           : perceptionData.lanePosition.lane->GetLength() -
                                                                 actualEgoAgent.GetReferencePointPosition()->roadPosition.s};
    perceptionData.movingInLaneDirection = GeneralAgentPerception::IsMovingInLaneDirection(
        perceptionData.lanePosition.lane, perceptionData.yaw, perceptionData.lanePosition.sCoordinate, perceptionData.velocity);
    perceptionData.brakeLight = driver->GetBrakeLight();
    perceptionData.indicatorState = driver->GetIndicatorState();
    perceptionData.nextLane = InfrastructurePerception::NextLane(perceptionData.indicatorState, perceptionData.movingInLaneDirection,
                                                                 perceptionData.lanePosition.lane);
    perceptionData.junctionDistance = GeneralAgentPerception::CalculateJunctionDistance(
        perceptionData, perceptionData.lanePosition.lane->GetRoad(), perceptionData.lanePosition.lane);

    // detailed agent information
    perceptionData.globalDriverPosition = GetDriverPosition();
    perceptionData.steeringWheelAngle = driver->GetSteeringWheelAngle();
    perceptionData.laneWidth = actualEgoAgent.GetLaneWidth();
    perceptionData.mainLocatorInformation = {
        mainLocatorLaneDReaM, // lane ptr
        perceptionData.movingInLaneDirection ? actualEgoAgent.GetPositionLateral()
                                             : -actualEgoAgent.GetPositionLateral(),                              // lateralDisplacement
        actualEgoAgent.GetLaneCurvature(),                                                                        // curvature
        perceptionData.movingInLaneDirection ? actualEgoAgent.GetRelativeYaw() : -actualEgoAgent.GetRelativeYaw() // heading
    };
    perceptionData.route = dreamRoute;

    egoPerception = std::make_shared<DetailedAgentPerception>(perceptionData);
}
