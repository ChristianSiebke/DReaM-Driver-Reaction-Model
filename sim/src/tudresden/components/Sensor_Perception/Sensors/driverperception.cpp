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

void DriverPerception::CalculatePerception(const AgentInterface *driver) {
    const auto &actualEgoAgent = const_cast<AgentInterface *>(driver)->GetEgoAgent();
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    WorldDataQuery helper(*worldData);
    auto mainRoad = worldData->GetRoads().at(actualEgoAgent.GetRoadId());
    auto mainLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetReferencePointPosition()->roadId, actualEgoAgent.GetReferencePointPosition()->laneId,
                              actualEgoAgent.GetReferencePointPosition()->roadPosition.s);

    auto indicator = driver->GetIndicatorState();
    EgoPerception data;
    data.id = driver->GetId();
    data.refPosition = Common::Vector2d(driver->GetPositionX(), driver->GetPositionY());
    data.distanceReferencePointToLeadingEdge = driver->GetDistanceReferencePointToLeadingEdge();
    data.road = infrastructurePerception->lookupTableRoadNetwork.roads.at(mainRoad->GetId());
    data.lane = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLane->GetId());
    data.laneType = mainLane->GetLaneType();
    data.velocity = driver->GetVelocity(VelocityScope::Absolute);
    data.acceleration = driver->GetAcceleration();
    data.brakeLight = driver->GetBrakeLight();
    data.indicatorState = indicator;
    data.vehicleType = driver->GetVehicleModelParameters().vehicleType;
    data.yawAngle = driver->GetYaw();
    data.width = driver->GetWidth();
    data.length = driver->GetLength();
    data.laneWidth = actualEgoAgent.GetLaneWidth();
    data.steeringWheelAngle = driver->GetSteeringWheelAngle();
    data.sCoordinate = actualEgoAgent.GetReferencePointPosition()->roadPosition.s;
    data.movingInLaneDirection = AgentPerception::IsMovingInLaneDirection(data.lane, data.yawAngle, data.sCoordinate, data.velocity);
    auto junctionDistance = data.CalculateJunctionDistance(data.road, data.lane);
    data.distanceOnJunction = junctionDistance.distanceOnJunction;
    data.distanceToNextJunction = junctionDistance.distanceToNextJunction;
    data.nextLane = InfrastructurePerception::NextLane(data.indicatorState, data.movingInLaneDirection, data.lane);
    data.driverPosition = GetDriverPosition();
    data.heading = data.movingInLaneDirection ? actualEgoAgent.GetRelativeYaw() : -actualEgoAgent.GetRelativeYaw();
    data.lateralDisplacement = data.movingInLaneDirection ? actualEgoAgent.GetPositionLateral() : -actualEgoAgent.GetPositionLateral();
    data.curvature = actualEgoAgent.GetLaneCurvature();

    egoPerception = std::make_shared<EgoPerception>(data);
}
