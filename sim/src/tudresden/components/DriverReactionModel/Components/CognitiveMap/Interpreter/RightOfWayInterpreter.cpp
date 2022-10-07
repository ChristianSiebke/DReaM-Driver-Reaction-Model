/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "RightOfWayInterpreter.h"

#include "RightOfWayRegulation/RightBeforeLeftRegulation.h"
#include "RightOfWayRegulation/TrafficSignRegulation.h"
#include "common/Helper.h"

namespace Interpreter {

void RightOfWayInterpreter::Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) {
    try {
        UpdateRightOfWayRegulation(representation);
        interpretation->rightOfWayMap.clear();

        for (const auto& observedAgent : *representation.agentMemory) {
            RightOfWay rightOfWay{true, true};

            auto junctionSituation = JunctionSituation(representation.egoAgent, *observedAgent);
            auto &agentInterpretation = interpretation->interpretedAgents.at(observedAgent->GetID());
            if (agentInterpretation->conflictSituation && junctionSituation) {
                rightOfWay = PerformRightOfWayDetermination(*observedAgent, representation);
            }

            interpretation->rightOfWayMap.insert({observedAgent->GetID(), rightOfWay});
            agentInterpretation->rightOfWay.ego = rightOfWay.ego;
            agentInterpretation->rightOfWay.observed = rightOfWay.observed;
        }

        if (interpretation->rightOfWayMap.size() != representation.agentMemory->size()) {
            auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " rightOfWayMap does not have the same size like agents in memory ";
            throw std::logic_error(msg);
        }
    } catch (std::logic_error e) {
        std::string message = e.what();
        Log(message, error);
        throw std::logic_error(message);

    } catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update right of way failed";
        Log(message, error);
        throw std::logic_error(message);
    }
}

void RightOfWayInterpreter::UpdateRightOfWayRegulation(const WorldRepresentation& representation) {
    if (auto rowSign = representation.egoAgent->NextROWSign()) {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::TrafficSignRegulationGermany>();
    } else {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::RightBeforeLeftRegulationGermany>();
    }
}

RightOfWay RightOfWayInterpreter::PerformRightOfWayDetermination(const AgentRepresentation& observedAgent,
                                                                 const WorldRepresentation& representation) {
    return rightOfWayRegulation->RightOfWayDetermination(observedAgent, representation);
}

std::optional<JunctionSituation> RightOfWayInterpreter::JunctionSituation(const EgoAgentRepresentation *ego,
                                                                          const AgentRepresentation &observedAgent) const {
    auto egoRoad = ego->GetRoad();
    auto oAgentRoad = observedAgent.GetRoad();

    if (egoRoad->IsOnJunction() && oAgentRoad->IsOnJunction()) {
        if (egoRoad->GetJunction() == oAgentRoad->GetJunction()) {
            return JunctionSituation::JUNCTION_A;
        }
    }
    else if (egoRoad->IsOnJunction()) {
        auto junction = egoRoad->GetJunction();
        if (IsMovingTowardsJunction(observedAgent, junction)) {
            return JunctionSituation::JUNCTION_B;
        }
    }
    else if (oAgentRoad->IsOnJunction()) {
        auto junction = oAgentRoad->GetJunction();
        if (IsMovingTowardsJunction(*ego, junction)) {
            return JunctionSituation::JUNCTION_C;
        }
    }

    const auto &egoLane = ego->GetLane();
    if (ego->IsMovingInLaneDirection()) {
        if (egoLane->IsInRoadDirection() && egoRoad->IsSuccessorJunction()) {
            auto junction = dynamic_cast<const MentalInfrastructure::Junction *>(egoRoad->GetSuccessor());
            if (IsMovingTowardsJunction(observedAgent, junction)) {
                return JunctionSituation::JUNCTION_D;
            }
        }
        else if (!egoLane->IsInRoadDirection() && egoRoad->IsPredecessorJunction()) {
            auto junction = dynamic_cast<const MentalInfrastructure::Junction *>(egoRoad->GetPredecessor());
            if (IsMovingTowardsJunction(observedAgent, junction)) {
                return JunctionSituation::JUNCTION_D;
            }
        }
    }
    else {
        if (egoLane->IsInRoadDirection() && egoRoad->IsPredecessorJunction()) {
            auto junction = dynamic_cast<const MentalInfrastructure::Junction *>(egoRoad->GetPredecessor());
            if (IsMovingTowardsJunction(observedAgent, junction)) {
                return JunctionSituation::JUNCTION_D;
            }
        }
        else if (!egoLane->IsInRoadDirection() && egoRoad->IsSuccessorJunction()) {
            auto junction = dynamic_cast<const MentalInfrastructure::Junction *>(egoRoad->GetSuccessor());
            if (IsMovingTowardsJunction(observedAgent, junction)) {
                return JunctionSituation::JUNCTION_D;
            }
        }
    }
    return std::nullopt;
}

bool RightOfWayInterpreter::IsMovingTowardsJunction(const AgentRepresentation &agent,
                                                    const MentalInfrastructure::Junction *junction) const {
    if (!junction)
        return false;

    auto roadOfAgent = agent.GetRoad();
    const auto &currentLane = agent.GetLane();
    if (currentLane->IsInRoadDirection()) {
        if (agent.IsMovingInLaneDirection()) {
            if (roadOfAgent->IsSuccessorJunction()) {
                return roadOfAgent->GetSuccessor()->GetOpenDriveId() == junction->GetOpenDriveId();
            }
        }
        else {
            if (roadOfAgent->IsPredecessorJunction()) {
                return roadOfAgent->GetPredecessor()->GetOpenDriveId() == junction->GetOpenDriveId();
            }
        }
    }
    else {
        if (agent.IsMovingInLaneDirection()) {
            if (roadOfAgent->IsPredecessorJunction()) {
                return roadOfAgent->GetPredecessor()->GetOpenDriveId() == junction->GetOpenDriveId();
            }
        }
        else {
            if (roadOfAgent->IsSuccessorJunction()) {
                return roadOfAgent->GetSuccessor()->GetOpenDriveId() == junction->GetOpenDriveId();
            }
        }
    }
    return false;
}

} // namespace Interpreter
