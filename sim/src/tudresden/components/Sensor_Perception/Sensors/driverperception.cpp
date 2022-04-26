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
    auto mainLocatorLane =
        &helper.GetLaneByOdId(actualEgoAgent.GetMainLocatePosition().roadId, actualEgoAgent.GetMainLocatePosition().laneId,
                              actualEgoAgent.GetMainLocatePosition().roadPosition.s);
    auto indicator = driver->GetIndicatorState();
    DReaMRoute::Waypoints dreamRoute;

    std::transform(route.begin(), route.end(), std::back_inserter(dreamRoute), [this](auto element) {
        DReaMRoute::Waypoint result;
        result.roadId = element.roadId;
        result.lane = this->infrastructurePerception->lookupTableRoadNetwork.lanes.at(element.lane);
        result.s = element.s;
        return result;
    });

    EgoPerception data;
    data.route = dreamRoute;
    data.id = driver->GetId();
    data.refPosition = Common::Vector2d(driver->GetPositionX(), driver->GetPositionY());
    data.distanceReferencePointToLeadingEdge = driver->GetDistanceReferencePointToLeadingEdge();
    data.lane = infrastructurePerception->lookupTableRoadNetwork.lanes.at(referenceLane->GetId());
    data.road = data.lane->GetRoad();
    data.mainLocatorLane = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLocatorLane->GetId());
    data.laneType = referenceLane->GetLaneType();
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
