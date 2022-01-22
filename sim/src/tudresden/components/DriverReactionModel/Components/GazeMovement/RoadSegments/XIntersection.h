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

class XIntersection : public Intersection {
  public:
    XIntersection(const WorldRepresentation& worldRepresentation, StochasticsInterface* stochastics, const BehaviourData& behaviourData);

    ~XIntersection() override = default;

    virtual GazeState ControlGlance(CrossingPhase phase) override;

  protected:
    virtual AOIProbabilities LookUpScanAOIProbability(CrossingPhase phase) override;

    virtual AOIProbabilities LookUpControlAOIProbability(CrossingPhase phase) override;

    virtual double LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi) override;

    /**
     * @brief View in the specified direction (ControlAOI) to check for approaching vehicles.
     * @param ControlAOI  view direction
     * @return GazeState
     */
    GazeState ControlGlanceOnRoad(ControlAOI aoi);

    /**
     * @brief View critical points at intersections to watch for oncoming traffic or VRUs
     * @param ControlAOI        view direction
     * @param CrossingPhase     current crossing phase
     * @return GazeState
     */
    GazeState ControlGlanceOnXIntersection(ControlAOI aoi, CrossingPhase phase);

    const Common::Vector2d* FixationPointForCGOnXIntersection(const std::vector<Common::Vector2d>& controlFixPoints, CrossingPhase phase,
                                                              ControlAOI aoi);
    const Common::Vector2d* FixationPointForCGOnRoad(const std::vector<Common::Vector2d>& controlFixPoints, ControlAOI aoi);

    void CalculateControlFixPointsOnXintersection();
    void CalculateControlFixPointsOnRoads();

  private:
    double viewingDepthIntoRoad; // how far the driver see along the road.

    std::vector<Common::Vector2d> controlFixPointsOnXIntersection;
    std::vector<Common::Vector2d> controlFixPointsOnRoads;
};
} // namespace Node
} // namespace RoadSegments
