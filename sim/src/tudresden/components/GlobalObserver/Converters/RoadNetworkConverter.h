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

/**
 * @brief Class for handling the conversion of base infrastructure to the format used by DReaM. Will populate the InfrastructurePerception
 * provided in the constructor.
 *
 */
class RoadNetworkConverter {
public:
    RoadNetworkConverter(WorldInterface *world, std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        world(world), infrastructurePerception(infrastructurePerception) {
    }

    /**
     * @brief Triggers the internal conversion logic and populates the InfrastructurePerception. Checks whether the conversion was already
     * performed to avoid double conversion.
     *
     */
    void Populate();

private:
    /**
     * @brief Prepares the lookup table for the road network after the conversion.
     *
     */
    void PrepareLookupTableRoadNetwork();

    /**
     * @brief Converts an OWL traffic sign into the internal format.
     *
     * @param road road that the traffic sign is attached to
     * @param sign traffic sign to convert
     * @return converted traffic sign
     */
    const MentalInfrastructure::TrafficSign *ConvertTrafficSign(const MentalInfrastructure::Road *road,
                                                                const OWL::Interfaces::TrafficSign *sign);

    /**
     * @brief Converts an OWL traffic light into the internal format.
     *
     * @param road road that the traffic light is attached to
     * @param trafficLight traffic light to be converted
     * @return converted traffic light
     */
    const MentalInfrastructure::TrafficLight *ConvertTrafficLight(const MentalInfrastructure::Road *road,
                                                                  const OWL::Interfaces::TrafficLight *trafficLight);

    /**
     * @brief Converts an OWL road into the internal format.
     *
     * @param road road to be converted
     * @return converted road
     */
    const MentalInfrastructure::Road *ConvertRoad(const OWL::Interfaces::Road *road);

    /**
     * @brief Converts an OWL lane into the internal format.
     *
     * @param lane lane to be converted
     * @return converted lane
     */
    MentalInfrastructure::Lane *ConvertLane(const OWL::Interfaces::Lane *lane);

    /**
     * @brief Converts & adds all lane geometry to an already converted lane.
     *
     * @param newLane the already converted lane
     * @param lane its OWL counterpart to take the geometry from
     */
    void AddLaneGeometry(MentalInfrastructure::Lane *newLane, const OWL::Interfaces::Lane *lane) const;

    /**
     * @brief Converts an OWL junction into the internal format.
     *
     * @param junction junction to be converted
     * @return converted junction
     */
    const MentalInfrastructure::Junction *ConvertJunction(const OWL::Interfaces::Junction *junction);

    /**
     * @brief Updates all stored traffic lights with their current status.
     *
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
