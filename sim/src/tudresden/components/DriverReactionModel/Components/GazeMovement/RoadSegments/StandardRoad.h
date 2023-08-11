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
};

} // namespace Edge
} // namespace RoadSegments
