/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "basicvisualsensor.h"

#include <thread>

#include "Common/TimeMeasurement.hpp"

TimeMeasurement timeMeasureAABB("AABB.cpp");
TimeMeasurement timeMeasureBasicSensor("BasicSensor.cpp");

void BasicVisualSensor::Trigger(int timestamp, GazeState gazeState) {
    // use threads for operations
    bool useThreads = false;

    sensorDirection = gazeState.directionUFOV;
    startPosUFOV = gazeState.startPosUFOV;
    minViewAngle = (-gazeState.openingAngle * (M_PI / 180)) / 2.0; // TODO: switch output visualization to radiant
    maxViewAngle = (gazeState.openingAngle * (M_PI / 180)) / 2.0;  // TODO: switch output visualization to radiant

    viewDistance = gazeState.viewDistance;
    timeMeasureAABB.StartTimePoint("Trigger AABB");
    aabbTree = aabbTreeHandler->GetCurrentAABBTree(timestamp); // this updates the aabb tree (if needed)
    timeMeasureAABB.EndTimePoint();

    visible.Clear();
    timeMeasureBasicSensor.StartTimePoint("Trigger BasicSensor");
    ThreadedAgentPerception(useThreads);
    timeMeasureBasicSensor.EndTimePoint();
}

void BasicVisualSensor::ThreadedAgentPerception(bool useThreads) {
    auto nb_elements = aabbTreeHandler->agents.size();

    if (useThreads) {
        unsigned nb_threads_hint = std::thread::hardware_concurrency();
        unsigned nb_threads = nb_threads_hint == 0 ? 8 : (nb_threads_hint);

        unsigned batch_size = nb_elements / nb_threads;
        unsigned batch_remainder = nb_elements % nb_threads;

        std::vector<std::thread> my_threads(nb_threads + 1);

        // Multithread execution
        for (unsigned i = 0; i < nb_threads; ++i) {
            unsigned start = i * batch_size;
            my_threads.at(i) = std::thread([this](unsigned s, unsigned e) { AgentPerceptionThread(s, e); }, start, start + batch_size);
        }

        // for all elements that are left
        int start = nb_threads * batch_size;
        my_threads.at(nb_threads) =
            std::thread([this](unsigned s, unsigned e) { AgentPerceptionThread(s, e); }, start, start + batch_remainder);

        // Wait for the other thread to finish their task
        std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
    }
    else {
        AgentPerceptionThread(0, aabbTreeHandler->agents.size());
    }
}

void BasicVisualSensor::AgentPerceptionThread(unsigned startIndex, unsigned endIndex) {
    for (unsigned k = startIndex; k < endIndex; k++) {
        const auto obj = aabbTreeHandler->agentObjects.at(k);
        const auto agent = aabbTreeHandler->agents.at(k);

        if (agent->GetId() == egoAgent->GetId())
            continue;

        if (viewDistance == 0.0)
            continue;

        auto otherPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());
        if ((otherPosition - startPosUFOV).Length() > viewDistance * 1.1)
            continue;

        auto points = obj->area.outer();
        bool hitAgent = false;

        for (unsigned i = 0; i < points.size() - 1; i++) {
            auto currentPoint = Common::Vector2d(points.at(i).x(), points.at(i).y());
            auto nextPoint = Common::Vector2d(points.at(i + 1).x(), points.at(i + 1).y());

            auto edge = nextPoint - currentPoint;
            auto distance = edge.Length();
            edge.Norm();

            for (unsigned j = 0; j < subdivisions + 1; j++) {
                auto pointToCheck = currentPoint + (edge * ((distance / subdivisions) * j));
                auto rayDirection = pointToCheck - startPosUFOV;

                const Ray ray(startPosUFOV, rayDirection);
                // transform point to local sensor funnel direction
                rayDirection.Rotate(-sensorDirection);

                // rule out objects that are not in FOV
                auto angle = rayDirection.Angle();

                if (angle < minViewAngle || angle > maxViewAngle)
                    continue;

                // rule out points that are beyond viewing distance
                if (rayDirection.Length() > viewDistance)
                    continue;

                // querying all objects that can possibly be hit by the ray
                // these are sorted by distance from the driver so the following for finds a solution faster

                const auto &queryResult = aabbTree->QueryRay(ray);

                bool foundOccluding = false;
                auto distanceToPoint = rayDirection.Length();

                for (auto it = queryResult.begin(); it != queryResult.end(); ++it) {
                    const auto obji = std::dynamic_pointer_cast<ObservedWorldObject>(it->first);
                    const auto hitResult = obji->IsHitByRay(ray);

                    if (hitResult < 0 || obji->id == egoAgent->GetId()) {
                        continue;
                    }

                    if (hitResult < distanceToPoint && agent->GetId() != obji->id) {
                        foundOccluding = true;
                        break;
                    }
                }

                hitAgent = !foundOccluding;

                if (hitAgent)
                    break;
            }

            if (hitAgent)
                break;
        }

        if (hitAgent) {
            // add it to the output list
            visible.InsertElement(agent->GetId());
        }
    }
}
