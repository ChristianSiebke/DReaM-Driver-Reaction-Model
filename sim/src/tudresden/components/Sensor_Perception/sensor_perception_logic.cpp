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

void SensorPerceptionLogic::Trigger(int timestamp, GazeState gazeState, std::optional<Common::Vector2d> mirrorPos) {
    visualSensor->Trigger(timestamp, gazeState, mirrorPos);
    trafficSignalVisualSensor->Trigger(timestamp, gazeState, mirrorPos);
}
