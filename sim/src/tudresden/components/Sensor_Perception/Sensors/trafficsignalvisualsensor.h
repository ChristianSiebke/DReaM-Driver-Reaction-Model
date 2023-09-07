/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once

#include "aabbtreehandler.h"
#include "include/agentInterface.h"
#include "visualsensorinterface.h"

class TrafficSignalVisualSensor : public VisualSensorInterface<OdId> {
public:
    TrafficSignalVisualSensor(AgentInterface *egoAgent, WorldInterface *world) : VisualSensorInterface(egoAgent, world) {
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    }
    ~TrafficSignalVisualSensor() {
        if (aabbTreeHandler.use_count() == 2) {
            aabbTreeHandler->ResetAABBTreeHandler();
        }
    }

    void Trigger(int timestamp, GazeState gazeState) override;

    std::vector<OdId> GetVisible() override {
        return visible;
    }

private:
    void DynamicInfrastructurePerceptionMethod();

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree = nullptr;
    std::vector<OdId> visible;

    double sensorDirection = 0;
    double minViewAngle = 0;
    double maxViewAngle = 0;
    double viewDistance = 0;
    Common::Vector2d startPosUFOV = Common::Vector2d(0, 0);
};