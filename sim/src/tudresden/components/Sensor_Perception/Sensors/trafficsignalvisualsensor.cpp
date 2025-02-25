/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "trafficsignalvisualsensor.h"

void TrafficSignalVisualSensor::Trigger(int timestamp, GazeState gazeState) {
    sensorDirection = gazeState.directionUFOV;
    startPosUFOV = gazeState.startPosUFOV;
    minViewAngle = (-gazeState.openingAngle * (M_PI / 180)) / 2.0; // TODO: switch output visualization to radiant
    maxViewAngle = (gazeState.openingAngle * (M_PI / 180)) / 2.0;  // TODO: switch output visualization to radiant

    viewDistance = gazeState.viewDistance;

    aabbTree = aabbTreeHandler->GetCurrentAABBTree(timestamp);
    DynamicInfrastructurePerceptionMethod();
}

void TrafficSignalVisualSensor::DynamicInfrastructurePerceptionMethod() {
    visible.clear();
    for (const auto &trafficSignal : aabbTreeHandler->trafficSignals) {
        auto signalId = aabbTreeHandler->trafficSignalsMappingReversed.at(trafficSignal);
        visible.push_back(signalId);
    }

    //  for (const auto &trafficSignal : aabbTreeHandler->trafficSignals) {
    //      auto rayDirection = trafficSignal->referencePosition - startPosUFOV;
    //
    //      const Ray ray(startPosUFOV, rayDirection);
    //      // transform point to local sensor funnel direction
    //      rayDirection.Rotate(-sensorDirection);
    //
    //      // rule out objects that are not in FOV
    //      auto angle = rayDirection.Angle();
    //
    //      if (angle < minViewAngle || angle > maxViewAngle)
    //          continue;
    //
    //      // rule out points that are beyond viewing distance
    //      if (rayDirection.Length() > viewDistance)
    //          continue;
    //
    //      const auto &queryResult = aabbTree->QueryRay(ray);
    //
    //      double closestHitDistance = __DBL_MAX__;
    //      std::shared_ptr<ObservedWorldObject> closest;
    //
    //      // looping over all elements in the query result and perform more accurate ray intersection test with the polygon instead
    //      // of the AABB
    //      for (auto it = queryResult.begin(); it != queryResult.end(); ++it) {
    //          const auto &obj = std::dynamic_pointer_cast<ObservedWorldObject>(it->first);
    //          const auto hitResult = obj->IsHitByRay(ray);
    //
    //          // the object is not actually hit, continue with next object
    //          if (hitResult < 0)
    //              continue;
    //
    //          // agent is hit and there is no object in front of hit
    //          if (obj->id != egoAgent->GetId() && hitResult < closestHitDistance) {
    //              closestHitDistance = hitResult;
    //              closest = obj;
    //          }
    //      }
    //
    //      if (closest == nullptr)
    //          continue;
    //
    //      if (closest->id != trafficSignal->id)
    //          continue;
    //
    //      const auto &tmp2 = std::dynamic_pointer_cast<ObservedTrafficSignal>(closest);
    //      auto signalId = aabbTreeHandler->trafficSignalsMappingReversed.at(tmp2);
    //
    //      visible.push_back(signalId);
    //  }
}
