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

    void Trigger(int timestamp, GazeState gazeState, std::optional<Common::Vector2d> mirrorPos) override;

    std::vector<OdId> GetVisible() override {
        return visible;
    }

private:
    void DynamicInfrastructurePerceptionMethod();

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree;
    std::vector<OdId> visible;

    double sensorDirection;
    double minViewAngle;
    double maxViewAngle;
    double viewDistance;
    Common::Vector2d driverPos;
};