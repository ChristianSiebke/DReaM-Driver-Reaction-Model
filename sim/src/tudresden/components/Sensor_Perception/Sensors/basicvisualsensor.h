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

#include "Common/Threading/ThreadSafeContainer.h"
#include "aabbtreehandler.h"
#include "common/globalDefinitions.h"
#include "egoAgent.h"
#include "visualsensorinterface.h"

class BasicVisualSensor : public VisualSensorInterface<int> {
public:
    BasicVisualSensor(AgentInterface *egoAgent, WorldInterface *world) : VisualSensorInterface(egoAgent, world) {
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    }
    ~BasicVisualSensor() {
        if (aabbTreeHandler.use_count() == 2) {
            aabbTreeHandler->ResetAABBTreeHandler();
        }
    }

    void Trigger(int timestamp, GazeState gazeState) override;

    std::vector<int> GetVisible() override {
        return visible.RetrieveData();
    }

private:
    void ThreadedAgentPerception(bool useThreads = true);
    void AgentPerceptionThread(unsigned startIndex, unsigned endIndex);

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree = nullptr;
    ThreadSafeVector<int> visible;

    double sensorDirection = 0;
    double minViewAngle = 0;
    double maxViewAngle = 0;
    double viewDistance = 0;
    Common::Vector2d zeroVector = Common::Vector2d(0, 0);
    Common::Vector2d startPosUFOV = Common::Vector2d(0, 0);

    const unsigned subdivisions = 2;
};
