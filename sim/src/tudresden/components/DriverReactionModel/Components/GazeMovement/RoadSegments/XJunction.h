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
#include "RoadSegmentInterface.h"

namespace RoadSegments {
namespace Node {

class XJunction : public Junction {
public:
    XJunction(const WorldRepresentation &worldRepresentation, StochasticsInterface *stochastics, const BehaviourData &behaviourData) :
        Junction(worldRepresentation, stochastics, behaviourData) {
        controlFixPointsOnJunction = CalculateControlFixPointsOnJunction();
        if (controlFixPointsOnRoads.size() != 3) {
            std::string message = __FILE__ " Line: " + std::to_string(__LINE__) +
                                  "the number of control fixation points on incoming roads (X-Junction) is incorrect";
            throw std::runtime_error(message);
        }
    };
    ~XJunction() override = default;

    virtual GazeState ControlGlance(CrossingPhase phase) override;

protected:
    virtual std::vector<Common::Vector2d> CalculateControlFixPointsOnJunction() const override;

    virtual AOIProbabilities LookUpScanAOIProbability(CrossingPhase phase) override;

    virtual AOIProbabilities LookUpControlAOIProbability(CrossingPhase phase) override;

    /**
     * @brief View in the specified direction (ControlAOI) to check for approaching vehicles.
     * @param ControlAOI  view direction
     * @return GazeState
     */
    GazeState ControlGlanceOnRoad(ControlAOI aoi);

    /**
     * @brief View critical points at junctions to watch for oncoming traffic or VRUs
     * @param ControlAOI        view direction
     * @param CrossingPhase     current crossing phase
     * @return GazeState
     */
    GazeState ControlGlanceOnXJunction(ControlAOI aoi, CrossingPhase phase);

    const Common::Vector2d *FixationPointForCGOnXJunction(const std::vector<Common::Vector2d> &controlFixPoints, CrossingPhase phase,
                                                          ControlAOI aoi);
    const Common::Vector2d *FixationPointForCGOnRoad(const std::vector<Common::Vector2d> &controlFixPoints, ControlAOI aoi);
};
} // namespace Node
} // namespace RoadSegments
