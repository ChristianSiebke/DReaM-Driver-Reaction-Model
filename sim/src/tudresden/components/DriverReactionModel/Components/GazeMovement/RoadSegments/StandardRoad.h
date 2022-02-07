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
namespace Edge {

class StandardRoad : public Road {
  public:
    StandardRoad(const WorldRepresentation& worldRepresentation, StochasticsInterface* stochastics, const BehaviourData& behaviourData)
        : Road(worldRepresentation, stochastics, behaviourData) {
        probabilityFixateLeadCar = behaviourData.gmBehaviour.std_probabilityFixateLeadCar;
        probabilityControlGlance = behaviourData.gmBehaviour.std_probabilityControlGlance;
    }

    ~StandardRoad() override = default;

    virtual GazeState ControlGlance(CrossingPhase phase) override;

  protected:
    virtual AOIProbabilities LookUpScanAOIProbability(CrossingPhase phase) override;

    virtual AOIProbabilities LookUpControlAOIProbability(CrossingPhase phase) override;

    virtual double LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi) override;
};

} // namespace Edge
} // namespace RoadSegments
