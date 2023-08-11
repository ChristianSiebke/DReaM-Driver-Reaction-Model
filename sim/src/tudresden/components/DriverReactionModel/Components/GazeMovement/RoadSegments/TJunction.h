/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/
#pragma once
#include "RoadSegmentInterface.h"

namespace RoadSegments {
namespace Node {

class TJunction : public Junction {
public:
    TJunction(const WorldRepresentation &worldRepresentation, StochasticsInterface *stochastics, const BehaviourData &behaviourData,
              TJunctionLayout layout);

    virtual ~TJunction() override = default;

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
    GazeState ControlGlanceOnTJunction(ControlAOI aoi, CrossingPhase phase);

    const Common::Vector2d *FixationPointForCGOnTJunction(const std::vector<Common::Vector2d> &controlFixPoints, CrossingPhase phase,
                                                          ControlAOI aoi);
    const Common::Vector2d *FixationPointForCGOnRoad(const std::vector<Common::Vector2d> &controlFixPoints, ControlAOI aoi);

private:
    TJunctionLayout layout;
};
} // namespace Node
} // namespace RoadSegments
