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
#include <cmath>
#include <tuple>

#include "Common/Helper.h"
#include "Common/PerceptionData.h"
#include "Components/ComponentInterface.h"
#include "Components/LateralDecision.h"
#include "Components/LongitudinalDecision/LongitudinalDecision.h"
#include "RoadSegments/StandardRoad.h"
#include "RoadSegments/TJunction.h"
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
using TJunction = RoadSegments::Node::TJunction;
using StandardRoad = RoadSegments::Edge::StandardRoad;

namespace GazeMovement {

enum class SegmentType { NONE, XJunction, TJunction, StandardRoad };

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

    const GazeState& GetGazeState() { return currentGazeState; }

    const std::vector<Common::Vector2d>& GetSegmentControlFixationPoints() { return roadSegment->GetControlFixationPoints(); }

  private:
      GazeState DetermineGazeState();

      void UpdateUFOV() {
          currentGazeState.directionUFOV = roadSegment->UpdateUFOVAngle(currentGazeState);
          currentGazeState.startPosUFOV = roadSegment->UpdateStartPosUFOV(currentGazeState);
      }

    void UpdateRoadSegment();

    GazeState PerformControlGaze(CrossingPhase phase) { return roadSegment->ControlGlance(phase); }

    GazeState PerformScanGaze(CrossingPhase phase) { return roadSegment->ScanGlance(phase); }

    GazeState PerformAgentObserveGlance(int agentId) { return roadSegment->AgentObserveGlance(agentId); }
    GazeState PerformShoulderCheckRight(ScanAOI gaze) {
          return roadSegment->ShoulderCheckRight(gaze);
    };
    GazeState PerformShoulderCheckLeft(ScanAOI gaze) {
          return roadSegment->ShoulderCheckLeft(gaze);
    };
    GazeState PerformMirrowGaze(ScanAOI gaze) {
          return roadSegment->MirrowGaze(gaze);
    };

    double ProbabilityToFixateLeadCar() { return roadSegment->GetProbabilityToFixateLeadCar(); }

    double ProbabilityToPerformControlGlance() { return roadSegment->GetProbabilityToPerformControlGlance(); }

    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;

    GazeState currentGazeState;

    std::unique_ptr<RoadSegmentInterface> roadSegment{nullptr};
    SegmentType currentSegmentType = SegmentType::NONE;
    bool turningAtJunctionShoulderCheckDecision = false;

    int durationCounter = 0;
};
// namespace RoadSegments
} // namespace GazeMovement
