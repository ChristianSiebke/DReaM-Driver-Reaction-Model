#include "driverperception.h"

#include <qglobal.h>

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

double DriverPerception::DetermineLateralDisplacement(bool referenceLaneInDirection, const AgentPerception *agent) {
    auto actualEgoAgent = (const EgoAgent *)egoAgent;
    double lateralDisplacement = actualEgoAgent->GetMainLocatePosition().roadPosition.t;

    if (agent->movingInLaneDirection) {
        if (referenceLaneInDirection) {
            return lateralDisplacement;
        }
        else {
            return (-1) * lateralDisplacement;
        }
    }
    else {
        if (referenceLaneInDirection) {
            return (-1) * lateralDisplacement;
        }
        else {
            return lateralDisplacement;
        }
    }
}

double DriverPerception::DetermineRoadHeading(bool referenceLaneInDirection, const AgentPerception *agent) {
    auto actualEgoAgent = (const EgoAgent *)egoAgent;
    double heading = actualEgoAgent->GetMainLocatePosition().roadPosition.hdg;

    bool agentIsMovingInLaneDirection = agent->movingInLaneDirection;

    if (agentIsMovingInLaneDirection) {
        if (!referenceLaneInDirection) {
            if (heading < 0) {
                heading = heading + M_PI;
            }
            else {
                heading = heading - M_PI;
            }
        }
    }
    else {
        if (referenceLaneInDirection) {
            if (heading < 0) {
                heading = heading + M_PI;
            }
            else {
                heading = heading - M_PI;
            }
        }
    }

    while (heading > M_PI) {
        heading = heading - 2 * M_PI;
    }

    while (heading < -M_PI) {
        heading = heading + 2 * M_PI;
    }
    return heading;
}

double DriverPerception::DetermineDriverCurvature(bool laneInRoadDirection) {
    // TODO maybe rework & re-implement --> consider moving direction of agent
    double curvature = 0;

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    auto actualEgoAgent = (const EgoAgent *)egoAgent;

    double roadLength = worldData->GetRoads().at(actualEgoAgent->GetRoadId())->GetLength();

    // curvature calculations:
    if (laneInRoadDirection) {
        // double distanceEndOfLane = roadLength - egoAgent->GetDistanceToStartOfRoad();

        // auto navData = egoAgent->MentalModel_GetNavigationDecision();

        // double step = egoAgent->GetVelocity(VelocityScope::Absolute) * 0.1;
        // double diff = egoAgent->GetDistanceReferencePointToFrontAxle() + step - distanceEndOfLane;

        // if (navData.laneID != UINT64_MAX && diff > 0) {
        //     curvature = egoAgent->GetNextLaneCurvature(navData.odRoadID, navData.odLaneID, diff);
        // }
        // else {
        //     curvature = egoAgent->GetLaneCurvature(0, egoAgent->GetDistanceReferencePointToFrontAxle() + step);
        // }
    }
    else {
        // double distanceEndOfLane = egoAgent->GetDistanceToStartOfRoad();

        // auto navData = egoAgent->MentalModel_GetNavigationDecision();

        // double step = egoAgent->GetVelocity(VelocityScope::Absolute) * 0.1;
        // double diff = egoAgent->GetDistanceReferencePointToFrontAxle() + step - distanceEndOfLane;

        // if (navData.laneID != UINT64_MAX && diff > 0) {
        //     curvature = egoAgent->GetNextLaneCurvature(navData.odRoadID, navData.odLaneID, diff);
        // }
        // else {
        //     curvature = egoAgent->GetLaneCurvature(0, (-1) * (egoAgent->GetDistanceReferencePointToFrontAxle() + step));
        // }
    }

    return curvature;
}

void DriverPerception::CalculatePerception(const AgentInterface *driver) {
    auto actualEgoAgent = (const EgoAgent *)driver;

    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

    auto mainRoad = worldData->GetRoads().at(actualEgoAgent->GetRoadId());
    OWL::Interfaces::Lane *mainLane;
    for (auto [owlId, lane] : worldData->GetLanes()) {
        if (lane->GetOdId() == actualEgoAgent->GetLaneIdFromRelative(0) && lane->GetRoad().GetId() == mainRoad->GetId()) {
            mainLane = lane;
            break;
        }
    }
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
    data.laneWidth = actualEgoAgent->GetLaneWidth();
    data.steeringWheelAngle = driver->GetSteeringWheelAngle();

    auto dist = driver->GetDistanceToStartOfRoad(MeasurementPoint::Reference, mainRoad->GetId());

    data.sCoordinate = mainLane->GetOdId() < 0 ? dist : mainRoad->GetLength() - dist;
    data.movingInLaneDirection = AgentPerception::IsMovingInLaneDirection(data.lane, data.yawAngle, data.sCoordinate, data.velocity);
    auto junctionDistance = data.CalculateJunctionDistance(data.road, data.lane);
    data.distanceOnJunction = junctionDistance.distanceOnJunction;
    data.distanceToNextJunction = junctionDistance.distanceToNextJunction;
    data.nextLane = InfrastructurePerception::NextLane(data.indicatorState, data.movingInLaneDirection, data.lane);
    data.driverPosition = GetDriverPosition();

    // if the next lane is in the same direction as the current lane (they meet end to start or start to end)
    // bool nextLaneInDirection = driver->GetFrontMiddlePointLaneId() != InvalidId
    //                                ? worldData->GetLanes().at(driver->GetFrontMiddlePointLaneId())->IsInStreamDirection()
    //                                : true;

    // TODO observe functionality for the following 3 statements
    data.heading = actualEgoAgent->GetLaneDirection();               // DetermineRoadHeading(nextLaneInDirection, &data);
    data.lateralDisplacement = actualEgoAgent->GetPositionLateral(); // DetermineLateralDisplacement(nextLaneInDirection, &data);
    data.curvature = actualEgoAgent->GetLaneCurvature();             // DetermineDriverCurvature(mainLane->IsInStreamDirection());

    egoPerception = std::make_shared<EgoPerception>(data);
}
