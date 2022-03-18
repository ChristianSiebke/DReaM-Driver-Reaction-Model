#include "basicvisualsensor.h"

#include <thread>

#define MEASURE_TIME false

#if MEASURE_TIME
#include <chrono>
#endif

void BasicVisualSensor::Trigger(int timestamp, double fovAngle, double distance, double opening) {
    // use threads for operations
    bool useThreads = true;

    sensorDirection = fovAngle;
    driverPos = Common::Vector2d(egoAgent->GetPositionX(), egoAgent->GetPositionY());
    minViewAngle = -opening / 2.0;
    maxViewAngle = opening / 2.0;
    viewDistance = distance;

    aabbTree = aabbTreeHandler->GetCurrentAABBTree(timestamp); // this updates the aabb tree (if needed)
    perceived.Clear();

    ThreadedAgentPerception(useThreads);
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
            my_threads[i] = std::thread([this](unsigned s, unsigned e) { AgentPerceptionThread(s, e); }, start, start + batch_size);
        }

        // for all elements that are left
        int start = nb_threads * batch_size;
        my_threads[nb_threads] =
            std::thread([this](unsigned s, unsigned e) { AgentPerceptionThread(s, e); }, start, start + batch_remainder);

        // Wait for the other thread to finish their task
        std::for_each(my_threads.begin(), my_threads.end(), std::mem_fn(&std::thread::join));
    }
    else {
        for (unsigned i = 0; i < nb_elements; ++i) {
            AgentPerceptionThread(i, i + 1);
        }
    }
}

void BasicVisualSensor::AgentPerceptionThread(unsigned startIndex, unsigned endIndex) {
    for (unsigned i = startIndex; i < endIndex; i++) {
        const auto obj = aabbTreeHandler->agentObjects[i];
        const auto agent = aabbTreeHandler->agents[i];

        if (agent->GetId() == egoAgent->GetId())
            continue;

        if (viewDistance == 0.0) {
            continue;
        }

        auto otherPosition = Common::Vector2d(agent->GetPositionX(), agent->GetPositionY());
        if ((otherPosition - driverPos).Length() > viewDistance * 1.1)
            continue;

        auto points = obj->area.outer();

        bool hitAgent = false;

        for (unsigned i = 0; i < points.size() - 1; i++) {
            auto currentPoint = Common::Vector2d(points[i].x(), points[i].y());
            auto nextPoint = Common::Vector2d(points[i + 1].x(), points[i + 1].y());

            auto edge = nextPoint - currentPoint;
            auto distance = edge.Length();
            edge.Norm();

            for (unsigned j = 0; j < subdivisions + 1; j++) {
                auto pointToCheck = currentPoint + (edge * ((distance / subdivisions) * j));
                auto rayDirection = pointToCheck - driverPos;

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

                // querying all objects that can possibly be hit by the ray
                // these are sorted by distance from the driver so the following for finds a solution faster

                const auto &queryResult = aabbTree->QueryRay(ray);

                bool foundOccluding = false;
                auto distanceToPoint = rayDirection.Length();

                for (auto it = queryResult.begin(); it != queryResult.end(); ++it) {
                    const auto &obj = std::dynamic_pointer_cast<ObservedWorldObject>(it->first);
                    const auto hitResult = obj->IsHitByRay(ray);

                    if (hitResult < 0 || obj->id == egoAgent->GetId())
                        continue;

                    if (hitResult < distanceToPoint) {
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
            ConvertAgent(agent);
        }
    }
}
