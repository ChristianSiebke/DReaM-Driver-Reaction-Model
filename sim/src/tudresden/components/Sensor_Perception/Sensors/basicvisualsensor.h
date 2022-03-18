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

    void ConvertAgent(const AgentInterface *agent) {
        AgentPerception data;
        const auto &actualEgoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();
        auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
        WorldDataQuery helper(*worldData);
        auto mainRoad = worldData->GetRoads().at(actualEgoAgent.GetRoadId());
        auto mainLane =
            &helper.GetLaneByOdId(actualEgoAgent.GetReferencePointPosition()->roadId, actualEgoAgent.GetReferencePointPosition()->laneId,
                                  actualEgoAgent.GetReferencePointPosition()->roadPosition.s);
        auto indicator = agent->GetIndicatorState();

        data.id = agent->GetId();
        data.refPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());
        data.distanceReferencePointToLeadingEdge = agent->GetDistanceReferencePointToLeadingEdge();
        data.laneType = mainLane->GetLaneType();
        data.road = infrastructurePerception->lookupTableRoadNetwork.roads.at(mainRoad->GetId());
        data.lane = infrastructurePerception->lookupTableRoadNetwork.lanes.at(mainLane->GetId());
        data.velocity = agent->GetVelocity(VelocityScope::Absolute);
        data.acceleration = agent->GetAcceleration();
        data.brakeLight = agent->GetBrakeLight();
        data.indicatorState = indicator;
        data.vehicleType = agent->GetVehicleModelParameters().vehicleType;
        data.yawAngle = agent->GetYaw();
        data.width = agent->GetWidth();
        data.length = agent->GetLength();
        data.sCoordinate = actualEgoAgent.GetReferencePointPosition()->roadPosition.s;
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
