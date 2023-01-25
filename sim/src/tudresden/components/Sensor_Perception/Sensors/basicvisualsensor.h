/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef BASICVISUALSENSOR_H
#define BASICVISUALSENSOR_H

#include "Common/Threading/ThreadSafeContainer.h"
#include "aabbtreehandler.h"
#include "common/globalDefinitions.h"
#include "egoAgent.h"
#include "roadnetworksensor.h"
#include "visualsensorinterface.h"

class BasicVisualSensor : public VisualSensorInterface<std::shared_ptr<GeneralAgentPerception>> {
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

    void Trigger(int timestamp, double direction, double distance, double opening, std::optional<Common::Vector2d> mirrorPos, bool godMode) override;

    std::vector<std::shared_ptr<GeneralAgentPerception>> GetVisible() override {
        return perceived.RetrieveData();
    }

private:
    void ThreadedAgentPerception(bool useThreads = true);
    void AgentPerceptionThread(unsigned startIndex, unsigned endIndex);

    void ConvertAgent(const AgentInterface *agent) {
        auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
        const auto &actualEgoAgent = const_cast<AgentInterface *>(agent)->GetEgoAgent();
        WorldDataQuery helper(*worldData);
        auto referenceLane =
            &helper.GetLaneByOdId(actualEgoAgent.GetReferencePointPosition()->roadId, actualEgoAgent.GetReferencePointPosition()->laneId,
                                  actualEgoAgent.GetReferencePointPosition()->roadPosition.s);
        auto referenceLaneDReaM = infrastructurePerception->lookupTableRoadNetwork.lanes.at(referenceLane->GetId());

        GeneralAgentPerception perceptionData;
        // object information
        perceptionData.id = agent->GetId();
        perceptionData.yaw = agent->GetYaw();
        perceptionData.width = agent->GetWidth();
        perceptionData.length = agent->GetLength();
        perceptionData.refPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());

        // general agent information
        perceptionData.vehicleType = (DReaMDefinitions::AgentVehicleType)agent->GetVehicleModelParameters().vehicleType;
        perceptionData.distanceReferencePointToLeadingEdge = agent->GetDistanceReferencePointToLeadingEdge();
        perceptionData.acceleration = agent->GetAcceleration();
        perceptionData.velocity = agent->GetVelocity(VelocityScope::Absolute);
        perceptionData.movingInLaneDirection = GeneralAgentPerception::IsMovingInLaneDirection(
            perceptionData.lanePosition.lane, perceptionData.yaw, perceptionData.lanePosition.sCoordinate, perceptionData.velocity);
        perceptionData.brakeLight = agent->GetBrakeLight();
        perceptionData.indicatorState = agent->GetIndicatorState();
        perceptionData.lanePosition = {referenceLaneDReaM, referenceLaneDReaM->IsInRoadDirection()
                                                               ? actualEgoAgent.GetReferencePointPosition()->roadPosition.s
                                                               : perceptionData.lanePosition.lane->GetLength() -
                                                                     actualEgoAgent.GetReferencePointPosition()->roadPosition.s};
        perceptionData.nextLane = InfrastructurePerception::NextLane(perceptionData.indicatorState, perceptionData.movingInLaneDirection,
                                                                     perceptionData.lanePosition.lane);
        perceptionData.junctionDistance = GeneralAgentPerception::CalculateJunctionDistance(
            perceptionData, perceptionData.lanePosition.lane->GetRoad(), perceptionData.lanePosition.lane);

        perceived.InsertElement(std::make_shared<GeneralAgentPerception>(perceptionData));
    }

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree;
    ThreadSafeVector<std::shared_ptr<GeneralAgentPerception>> perceived;
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
