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
#include <cmath>
#include <tuple>

#include "Common/Helper.h"
#include "Common/PerceptionData.h"
#include "Components/ActionDecision/ActionDecision.h"
#include "Components/ComponentInterface.h"
#include "Components/Navigation.h"
#include "RoadSegments/StandardRoad.h"
#include "RoadSegments/XJunction.h"
#include "core/opSimulation/framework/sampler.h"
#include "qglobal.h"

namespace RoadSegments {
class RoadSegmentInterface;
namespace Node {
class XJunction;
} // namespace Node
namespace Edge {
class StandardRoad;
}
} // namespace RoadSegments
using RoadSegmentInterface = RoadSegments::RoadSegmentInterface;
using XJunction = RoadSegments::Node::XJunction;
using StandardRoad = RoadSegments::Edge::StandardRoad;

namespace GazeMovement {

enum class SegmentType { NONE, XJunction, StandardRoad };

class GazeMovement : public Component::ComponentInterface {
  public:
    GazeMovement(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation, int cycleTime,
                 StochasticsInterface* stochastics, LoggerInterface* loggerInterface, const BehaviourData& behaviourData)
        : ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData), worldRepresentation{worldRepresentation},
          worldInterpretation{worldInterpretation} {}
    GazeMovement(const GazeMovement&) = delete;
    GazeMovement& operator=(const GazeMovement&) = delete;
    ~GazeMovement() override = default;

    void Update() override;

    virtual const WorldRepresentation& GetWorldRepresentation() const override { return worldRepresentation; }

    virtual const WorldInterpretation& GetWorldInterpretation() const override { return worldInterpretation; }

    const GazeState& GetGazeState() { return currentGazeState; }

    const std::vector<Common::Vector2d>& GetSegmentControlFixationPoints() { return roadSegment->GetControlFixationPoints(); }

  private:
    void DetermineGazeState();

    void UpdateUFOVAngle() { currentGazeState.ufovAngle = roadSegment->UpdateUFOVAngle(currentGazeState); }

    void UpdateRoadSegment();

    GazeState PerformControlGaze(CrossingPhase phase) { return roadSegment->ControlGlance(phase); }

    GazeState PerformScanGaze(CrossingPhase phase) { return roadSegment->ScanGlance(phase); }

    GazeState PerformAgentObserveGlance(int agentId) { return roadSegment->AgentObserveGlance(agentId); }

    double ProbabilityToFixateLeadCar() { return roadSegment->GetProbabilityToFixateLeadCar(); }

    double ProbabilityToPerformControlGlance() { return roadSegment->GetProbabilityToPerformControlGlance(); }

    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;

    GazeState currentGazeState;

    std::unique_ptr<RoadSegmentInterface> roadSegment{nullptr};
    SegmentType currentSegmentType = SegmentType::NONE;

    int durationCounter = 0;
};
// namespace RoadSegments
} // namespace GazeMovement
