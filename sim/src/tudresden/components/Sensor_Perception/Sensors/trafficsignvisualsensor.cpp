#include "trafficsignvisualsensor.h"

void TrafficSignVisualSensor::Trigger(int timestamp) {
    driverPos = Common::Vector2d(egoAgent->GetPositionX(), egoAgent->GetPositionY());
    // TODO retrieve gaze info from somewhere else
    // sensorDirection = egoAgent->GetCurrentGazeState().ufovAngle;
    // minViewAngle = -egoAgent->GetCurrentGazeState().openingAngle / 2.0;
    // maxViewAngle = egoAgent->GetCurrentGazeState().openingAngle / 2.0;
    // viewDistance = egoAgent->GetCurrentGazeState().viewDistance;

    aabbTree = aabbTreeHandler->GetCurrentAABBTree(timestamp);
    DynamicInfrastructurePerceptionMethod();
}

void TrafficSignVisualSensor::Trigger(int timestamp, double direction, double distance, double opening) {
    driverPos = Common::Vector2d(egoAgent->GetPositionX(), egoAgent->GetPositionY());
    minViewAngle = direction - opening / 2.0;
    maxViewAngle = direction + opening / 2.0;
    viewDistance = distance;

    aabbTree = aabbTreeHandler->GetCurrentAABBTree(timestamp);
    DynamicInfrastructurePerceptionMethod();
}

void TrafficSignVisualSensor::DynamicInfrastructurePerceptionMethod() {
    perceived.clear();

    for (const auto &trafficSign : aabbTreeHandler->trafficSigns) {
        auto rayDirection = trafficSign->referencePosition - driverPos;

        const Ray ray(driverPos, rayDirection);
        // transform point to local sensor funnel direction
        rayDirection.Rotate(-sensorDirection);

        // rule out objects that are not in FOV
        auto angle = rayDirection.Angle();

        if (angle < minViewAngle || angle > maxViewAngle)
            continue;

        // rule out points that are beyond viewing distance
        if (rayDirection.Length() > viewDistance)
            continue;

        const auto &queryResult = aabbTree->QueryRay(ray);

        double closestHitDistance = __DBL_MAX__;
        std::shared_ptr<ObservedWorldObject> closest;

        // looping over all elements in the query result and perform more accurate ray intersection test with the polygon instead
        // of the AABB
        for (auto it = queryResult.begin(); it != queryResult.end(); ++it) {
            const auto &obj = std::dynamic_pointer_cast<ObservedWorldObject>(it->first);
            const auto hitResult = obj->IsHitByRay(ray);

            // the object is not actually hit, continue with next object
            if (hitResult < 0)
                continue;

            // agent is hit and there is no object in front of hit
            if (obj->id != egoAgent->GetId() && hitResult < closestHitDistance) {
                closestHitDistance = hitResult;
                closest = obj;
            }
        }

        if (closest == nullptr)
            continue;

        if (closest->id != trafficSign->id)
            continue;

        const auto &tmp2 = std::dynamic_pointer_cast<ObservedTrafficSign>(closest);
        auto owlTrafficSign = aabbTreeHandler->trafficSignsMappingReversed.at(tmp2);

        auto pointerIter = infrastructurePerception->lookupTableRoadNetwork.trafficSigns.find(owlTrafficSign->GetId());
        if (pointerIter != infrastructurePerception->lookupTableRoadNetwork.trafficSigns.end()) {
            perceived.push_back(pointerIter->second);
        }

        // auto pointerFind = infrastructurePerception->trafficSigns.

        // TODO umstellen auf pointer lookup (raw pointer)

        //        MentalInfrastructure::TrafficSign newTrafficSign(
        //            owlTrafficSign->GetId(), owlTrafficSign->GetOpenDriveId(), owlTrafficSign->GetValue(), owlTrafficSign->GetT(),
        //            owlTrafficSign->GetS(),
        //            Common::Vector2d(owlTrafficSign->GetReferencePointPosition().x, owlTrafficSign->GetReferencePointPosition().y),
        //            owlTrafficSign->GetType());

        //        auto owlTrafficSignRoadId = owlTrafficSign->GetRoadId();
        //        auto it = std::find_if(infrastructurePerception->roads.begin(), infrastructurePerception->roads.end(),
        //                               [&owlTrafficSignRoadId](const std::shared_ptr<const MentalInfrastructure::Road>& road) {
        //                                   return road->GetId() == owlTrafficSignRoadId;
        //                               });

        //        if (it == infrastructurePerception->roads.end())
        //            continue;

        //        auto tmp = *it;
        //        newTrafficSign.SetRoad(const_cast<MentalInfrastructure::Road*>(tmp.get()));

        //        perceived.push_back(std::make_shared<MentalInfrastructure::TrafficSign>(newTrafficSign));
    }
}
