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

#include "Sensors/aabbtreehandler.h"
#include "Sensors/basicvisualsensor.h"
#include "Sensors/trafficsignalvisualsensor.h"
#include "include/agentInterface.h"

struct GazeState;

typedef boost::geometry::model::d2::point_xy<double> BoostVector2d;
typedef boost::geometry::model::polygon<BoostVector2d> Polygon2d;

///
/// @brief Helper class to encapsulate all methods that should be executed on Trigger() of the Senso_Perception_Implementation.
///
class SensorPerceptionLogic {
public:
    SensorPerceptionLogic(AgentInterface *agent, WorldInterface *world) : driver{agent}, world{world} {
        aabbTreeHandler = AABBTreeHandler::GetInstance(world);
        visualSensor = std::make_shared<BasicVisualSensor>(agent, world);
        trafficSignalVisualSensor = std::make_shared<TrafficSignalVisualSensor>(agent, world);
    }

~SensorPerceptionLogic() {
    if (aabbTreeHandler.use_count() == 2) {
        aabbTreeHandler->ResetAABBTreeHandler();
    }
}

///
/// \brief Starts the calculation of Infrastructure, DriverPerception and VisualPerception.
///
void Trigger(int timestamp, GazeState gazeState);

///
/// \brief Returns a pointer to the visual sensor, no calculation is performed.
///
std::shared_ptr<VisualSensorInterface<int>> GetVisualSensor() const {
return visualSensor;
}

///
/// \brief Returns the list of currently visible agents, no calculation is performed.
///
const std::vector<int> GetVisibleAgents() const {
return visualSensor->GetVisible();
}

///
/// \brief Returns the list of currently visible traffic signs, no calculation is performed.
///
const std::vector<OdId> GetVisibleTrafficSignals() const {
return trafficSignalVisualSensor->GetVisible();
}

private:
    std::shared_ptr<VisualSensorInterface<int>> visualSensor;
    std::shared_ptr<VisualSensorInterface<OdId>> trafficSignalVisualSensor;
    std::shared_ptr<AABBTreeHandler> aabbTreeHandler;

    AgentInterface *driver;
    WorldInterface *world;
};