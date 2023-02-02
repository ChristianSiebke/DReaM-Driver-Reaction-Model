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

#include <atomic>

#include "WorldData.h"
#include "common/PerceptionData.h"

namespace GlobalObserver::Converters {
// TODO documentation
class RoadNetworkConverter {
public:
    RoadNetworkConverter(WorldInterface *world, std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        world(world), infrastructurePerception(infrastructurePerception) {
    }

    void Populate();

private:
    void PrepareLookupTableRoadNetwork();

    const MentalInfrastructure::TrafficSign *ConvertTrafficSign(const MentalInfrastructure::Road *road,
                                                                const OWL::Interfaces::TrafficSign *sign);

    /**
     * @brief Converts an OWL traffic light into an internally usable one.
     *
     * @param road road that the traffic light is attached to
     * @param trafficLight traffic light to be converted
     * @return converted traffic light
     */
    const MentalInfrastructure::TrafficLight *ConvertTrafficLight(const MentalInfrastructure::Road *road,
                                                                  const OWL::Interfaces::TrafficLight *trafficLight);

    const MentalInfrastructure::Road *ConvertRoad(const OWL::Interfaces::Road *road);

    MentalInfrastructure::Lane *ConvertLane(const OWL::Interfaces::Lane *lane);

    void AddLaneGeometry(MentalInfrastructure::Lane *newLane, const OWL::Interfaces::Lane *lane) const;

    const MentalInfrastructure::Junction *ConvertJunction(const OWL::Interfaces::Junction *junction);

    /**
     * @brief Updates all stored traffic lights with their current status.
     */
    void UpdateTrafficLights();

    /**
     * @brief Creates a unique ID by incrementing a locally stored variable and returning it.
     *
     * @return the newly incremented ID
     */
    DReaMId GenerateUniqueId() {
        return lastId++;
    }

private:
    WorldInterface *world;                // the world all agents are in
    bool infrastructureConverted = false; // if the perception data has already been generated
    std::atomic<DReaMId> lastId{0};
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
};
} // namespace GlobalObserver::Converters
