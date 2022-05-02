#include "sensor_perception_logic.h"

#include <Objects/observedstaticobject.h>
#include <Objects/observeddynamicobject.h>

void SensorPerceptionLogic::Trigger(int timestamp, double directionAngle, double sensorDistance, double sensorOpeningAngle,
                                    std::vector<InternWaypoint> route) {
    visualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle);
    perceivedAgents = visualSensor->GetVisible();
    trafficSignalVisualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle);
    perceivedTrafficSignals = trafficSignalVisualSensor->GetVisible();
    roadNetworkSensor->GetRoadNetwork();
    driverPerception->CalculatePerception(driver, route);
}
