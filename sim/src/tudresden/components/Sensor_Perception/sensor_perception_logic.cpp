#include "sensor_perception_logic.h"

#include <Objects/observedstaticobject.h>
#include <Objects/observeddynamicobject.h>

#define MEASURE_TIME false

#if MEASURE_TIME
#include <chrono>
#endif

void SensorPerceptionLogic::Trigger(int timestamp, double directionAngle, double sensorDistance, double sensorOpeningAngle) {
    visualSensor->Trigger(timestamp, directionAngle, sensorDistance, sensorOpeningAngle);
    perceivedAgents = visualSensor->GetVisible();
    roadNetworkSensor->GetRoadNetwork();
    driverPerception->CalculatePerception(driver);
}

void SensorPerceptionLogic::Trigger(int timestamp) {

#if MEASURE_TIME
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;
    auto t1 = high_resolution_clock::now();
#endif

    visualSensor->Trigger(timestamp);
    perceivedAgents = visualSensor->GetVisible();

    trafficSignVisualSensor->Trigger(timestamp);
    perceivedTrafficSigns = trafficSignVisualSensor->GetVisible();

#if MEASURE_TIME
    auto t3 = high_resolution_clock::now();
#endif

    roadNetworkSensor->GetRoadNetwork();

#if MEASURE_TIME
    auto t4 = high_resolution_clock::now();
#endif

    driverPerception->CalculatePerception(driver);

#if MEASURE_TIME
    auto t5 = high_resolution_clock::now();

    auto ns_int_total = duration_cast<nanoseconds>(t5 - t1).count();
    if (ns_int_total > 30000) {
        auto ns_int_visualsensor = duration_cast<nanoseconds>(t3 - t1).count();
        auto ns_int_roadNetworkSensor = duration_cast<nanoseconds>(t4 - t3).count();
        auto ns_int_driverPerception = duration_cast<nanoseconds>(t5 - t4).count();

        std::cout << "[MEASURE TIME] Timestamp: " << timestamp << std::endl;
        std::cout << "[MEASURE TIME] SensorPerception::Trigger() took " << ns_int_total << "ns: " << std::endl;
        std::cout << "[MEASURE TIME] \tVisualSensor: " << ns_int_visualsensor << "ns, " << std::endl;
        std::cout << "[MEASURE TIME] \tRoadNetworkSensor: " << ns_int_roadNetworkSensor << "ns, " << std::endl;
        std::cout << "[MEASURE TIME] \tDriverPerception: " << ns_int_driverPerception << "ns, " << std::endl;
        std::cout << std::endl;
    }
#endif
}
