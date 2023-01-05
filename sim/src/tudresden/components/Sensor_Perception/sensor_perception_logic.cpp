/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "sensor_perception_logic.h"

#include <Objects/observedstaticobject.h>
#include <Objects/observeddynamicobject.h>

void SensorPerceptionLogic::Trigger(int timestamp, double directionAngle, double sensorDistance, double sensorOpeningAngle,
                                    std::optional<Common::Vector2d> mirrorPos, bool godMode, std::vector<InternWaypoint> route) {
    visualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle, mirrorPos, godMode);
    perceivedAgents = visualSensor->GetVisible();
    trafficSignalVisualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle, mirrorPos, godMode);
    perceivedTrafficSignals = trafficSignalVisualSensor->GetVisible();
    roadNetworkSensor->GetRoadNetwork();
    driverPerception->CalculatePerception(driver, route);
}
