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
#include "Common/Helper.h"
#include "core/opSimulation/framework/sampler.h"
#include "Common/Definitions.h"
#include "Common/BehaviourData.h"

using AOIProbabilities = std::vector<std::pair<int, double>>;

using ObservationAOI = AgentVehicleType;

struct FixationTarget {
    Common::Vector2d fixationPoint{-999, -999};
    int fixationAgent{-999};
};

struct GazeState {
    // includes gaze type and fixated AOI
    std::pair<GazeType, int> fixationState{GazeType::NONE, static_cast<int>(ScanAOI::NONE)};
    FixationTarget target;
    double ufovAngle{-999};
    double openingAngle{-999};
    double viewDistance{100}; // TODO calculate
    int fixationDuration{-999};
};

namespace RoadSegments {

class RoadSegmentInterface {
  public:
    RoadSegmentInterface(const WorldRepresentation& worldRepresentation, StochasticsInterface* stochastics, const BehaviourData& behaviourData)
        : worldRepresentation{worldRepresentation}, stochastics{stochastics}, behaviourData{behaviourData} {}

    virtual ~RoadSegmentInterface() = default;

    virtual GazeState AgentObserveGlance(int agentId);

    virtual GazeState ScanGlance(CrossingPhase phase);

    virtual GazeState ControlGlance(CrossingPhase phase) = 0;

    double UpdateUFOVAngle(GazeState currentGazeState);

    virtual double GetProbabilityToFixateLeadCar() { return probabilityFixateLeadCar; }

    virtual double GetProbabilityToPerformControlGlance() { return probabilityControlGlance; }

    virtual const std::vector<Common::Vector2d>& GetControlFixationPoints() { return controlFixationPoints; }

  protected:
    virtual AOIProbabilities LookUpScanAOIProbability(CrossingPhase phase) = 0;

    virtual AOIProbabilities LookUpControlAOIProbability(CrossingPhase phase) = 0;

    virtual double LookUpFixationDuration(CrossingPhase phase, ScanAOI aoi) = 0;
    /****************************************
     * HELPER METHODS
     ****************************************/
    virtual AOIProbabilities ScaleProbabilitiesToOneAndEliminateNegativeProbabilities(AOIProbabilities aoiProbs);

    virtual Common::Vector2d CalculateForesightVector(double foresightRange);

    double ScanUFOVAngle(ScanAOI aoi);
    double CalculateGlobalViewingAngle(Common::Vector2d viewVector);

    // have to be set by subclasses!
    double probabilityFixateLeadCar = 0;
    double probabilityControlGlance = 0;

    //!  all possible control points that could be fixated during the control
    //!  glance
    std::vector<Common::Vector2d> controlFixationPoints;
    const WorldRepresentation& worldRepresentation;
    StochasticsInterface* stochastics;
    const BehaviourData& behaviourData;
};

namespace Node {

class Junction : public RoadSegmentInterface {
public:
    using RoadSegmentInterface::RoadSegmentInterface;
    virtual ~Junction() = default;

protected:
    std::vector<const MentalInfrastructure::Lane *> CornerSidewalkLanesOfJunction(const MentalInfrastructure::Junction *currentJunction);

    const MentalInfrastructure::Lane *OncomingStraightConnectionLane(const MentalInfrastructure::Junction *currentJunction);
    void SortControlFixPoints(std::vector<Common::Vector2d> &controlFixPointsOnXJunction);
};
} // namespace Node

namespace Edge {

class Road : public RoadSegmentInterface {
  public:
    using RoadSegmentInterface::RoadSegmentInterface;
    virtual ~Road() = default;
};
} // namespace Edge
} // namespace RoadSegments
