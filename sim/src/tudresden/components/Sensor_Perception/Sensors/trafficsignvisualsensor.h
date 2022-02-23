#ifndef TRAFFICSIGNVISUALSENSOR_H
#define TRAFFICSIGNVISUALSENSOR_H

#include "aabbtreehandler.h"
#include "include/agentInterface.h"
#include "roadnetworksensor.h"
#include "visualsensorinterface.h"

class TrafficSignVisualSensor : public VisualSensorInterface<const MentalInfrastructure::TrafficSign *> {
public:
    TrafficSignVisualSensor(AgentInterface *egoAgent, WorldInterface *world,
                            std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        VisualSensorInterface(egoAgent, world), infrastructurePerception(infrastructurePerception) {
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    }
    ~TrafficSignVisualSensor() {
        if (aabbTreeHandler.use_count() == 2) {
            aabbTreeHandler->ResetAABBTreeHandler();
        }
    }

    void Trigger(int timestamp, double direction, double distance, double opening) override;

    std::vector<const MentalInfrastructure::TrafficSign *> GetVisible() override {
        return perceived;
    }

private:
    void DynamicInfrastructurePerceptionMethod();

private:
    OWL::WorldData *worldData;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;
    std::shared_ptr<AABBTree> aabbTree;
    std::vector<const MentalInfrastructure::TrafficSign *> perceived;
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;

    double sensorDirection;
    double minViewAngle;
    double maxViewAngle;
    double viewDistance;
    Common::Vector2d driverPos;
};

#endif // TRAFFICSIGNVISUALSENSOR_H
