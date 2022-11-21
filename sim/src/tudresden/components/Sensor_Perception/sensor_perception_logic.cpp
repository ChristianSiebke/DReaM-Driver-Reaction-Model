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
                                    std::vector<InternWaypoint> route) {
    visualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle);
    perceivedAgents = visualSensor->GetVisible();
    trafficSignVisualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle);
    perceivedTrafficSigns = trafficSignVisualSensor->GetVisible();
    roadNetworkSensor->GetRoadNetwork();
    driverPerception->CalculatePerception(driver, route);
}
