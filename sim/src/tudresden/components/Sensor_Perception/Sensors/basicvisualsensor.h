#ifndef BASICVISUALSENSOR_H
#define BASICVISUALSENSOR_H

#include "Common/Threading/ThreadSafeContainer.h"
#include "aabbtreehandler.h"
#include "common/globalDefinitions.h"
#include "egoAgent.h"
#include "roadnetworksensor.h"
#include "visualsensorinterface.h"

class BasicVisualSensor : public VisualSensorInterface<std::shared_ptr<AgentPerception>> {
public:
    BasicVisualSensor(AgentInterface *egoAgent, WorldInterface *world, std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        VisualSensorInterface(egoAgent, world), infrastructurePerception(infrastructurePerception) {
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    }
    ~BasicVisualSensor() {
        if (aabbTreeHandler.use_count() == 2) {
            aabbTreeHandler->ResetAABBTreeHandler();
        }
    }

    void Trigger(int timestamp, double direction, double distance, double opening) override;

    std::vector<std::shared_ptr<AgentPerception>> GetVisible() override {
        return perceived.RetrieveData();
    }

private:
    void ThreadedAgentPerception(bool useThreads = true);
    void AgentPerceptionThread(unsigned startIndex, unsigned endIndex);

    void ConvertAgent(AgentInterface *agent) {
        AgentPerception data;
        auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());

        // auto agentRoadSearch = worldData->GetRoads().find(agent->GetRoads(MeasurementPoint::Front).front());
        // auto agentRoadId = agentRoadSearch == worldData->GetRoads().end() ? "-999" : agentRoad->first;

        auto mainRoad = worldData->GetRoads().at(agent->GetEgoAgent().GetRoadId());
        auto lanesOnRoad = mainRoad->GetSections().front()->GetLanes(); // TODO adapt for roads with >1 section
        auto mainLane = *std::find_if(lanesOnRoad.begin(), lanesOnRoad.end(),
                                      [agent](auto &obj) { return obj->GetOdId() == agent->GetEgoAgent().GetLaneIdFromRelative(0); });

        auto indicator = agent->GetIndicatorState();

        data.id = agent->GetId();
        data.refPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());
        data.distanceReferencePointToLeadingEdge = agent->GetDistanceReferencePointToLeadingEdge();
        data.laneType = mainLane->GetLaneType();
        data.road = infrastructurePerception->lookupTableRoadNetwork.roads.at(std::stoi(mainRoad->GetId()));
        data.lane = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLane->GetOdId());
        data.velocity = agent->GetVelocity(VelocityScope::Absolute);
        data.acceleration = agent->GetAcceleration();
        data.brakeLight = agent->GetBrakeLight();
        data.indicatorState = indicator;
        data.vehicleType = agent->GetVehicleModelParameters().vehicleType;
        data.yawAngle = agent->GetYaw();
        data.width = agent->GetWidth();
        data.length = agent->GetLength();

        auto dist = agent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, mainRoad->GetId());

        data.sCoordinate = mainLane->GetOdId() < 0 ? dist : mainRoad->GetLength() - dist;
        data.movingInLaneDirection = AgentPerception::IsMovingInLaneDirection(data.lane, data.yawAngle, data.sCoordinate, data.velocity);
        auto junctionDistance = data.CalculateJunctionDistance(data.road, data.lane);
        data.distanceOnJunction = junctionDistance.distanceOnJunction;
        data.distanceToNextJunction = junctionDistance.distanceToNextJunction;
        data.nextLane = InfrastructurePerception::NextLane(data.indicatorState, data.movingInLaneDirection, data.lane);

        perceived.InsertElement(std::make_shared<AgentPerception>(data));
    }

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree;
    ThreadSafeVector<std::shared_ptr<AgentPerception>> perceived;
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    DynamicInfrastructurePerception dynamicInfrastructurePerception;

    double sensorDirection;
    double minViewAngle;
    double maxViewAngle;
    double viewDistance;
    Common::Vector2d zeroVector = Common::Vector2d(0, 0);
    Common::Vector2d driverPos;

    const unsigned subdivisions = 2;
};

#endif // BASICVISUALSENSOR_H
