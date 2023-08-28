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
                rightOfWay = PerformRightOfWayDetermination(*observedAgent, representation, *agentInterpretation->conflictSituation);
            }

            interpretation->rightOfWayMap.insert({observedAgent->GetID(), rightOfWay});
            agentInterpretation->rightOfWay.ego = rightOfWay.ego;
            agentInterpretation->rightOfWay.observed = rightOfWay.observed;
        }

        if (interpretation->rightOfWayMap.size() != representation.agentMemory->size()) {
            auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " rightOfWayMap does not have the same size like agents in memory ";
            throw std::logic_error(msg);
        }
    } catch (std::logic_error &e) {
        const std::string message = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " " + e.what();
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
    if (auto rowSign = representation.trafficSignalMemory->GetRightOfWaySignsForLane(representation.egoAgent->GetLanePosition().lane)) {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::TrafficSignRegulationGermany>();
    }
    else {
        rightOfWayRegulation = std::make_unique<RightOfWayRegulation::RightBeforeLeftRegulationGermany>();
    }
}

RightOfWay RightOfWayInterpreter::PerformRightOfWayDetermination(const AgentRepresentation &observedAgent,
                                                                 const WorldRepresentation &representation,
                                                                 const ConflictSituation &conflictSituation) {
    return rightOfWayRegulation->RightOfWayDetermination(observedAgent, representation, conflictSituation);
}

std::optional<JunctionSituation> RightOfWayInterpreter::JunctionSituation(const EgoAgentRepresentation *ego,
                                                                          const AgentRepresentation &observedAgent) const {
    auto egoLane = ego->GetLanePosition().lane;
    auto oAgentLane = observedAgent.GetLanePosition().lane;

    if (egoLane->IsJunctionLane() && oAgentLane->IsJunctionLane()) {
        if (egoLane->GetRoad()->GetJunction() == oAgentLane->GetRoad()->GetJunction()) {
            return JunctionSituation::JUNCTION_A;
        }
    }
    else if (egoLane->IsJunctionLane()) {
        auto junction = egoLane->GetRoad()->GetJunction();
        if (IsMovingTowardsJunction(observedAgent, junction)) {
            return JunctionSituation::JUNCTION_B;
        }
    }
    else if (oAgentLane->IsJunctionLane()) {
        auto junction = oAgentLane->GetRoad()->GetJunction();
        if (IsMovingTowardsJunction(*ego, junction)) {
            return JunctionSituation::JUNCTION_C;
        }
    }
    auto junction = NextJunction(*ego);
    if (IsMovingTowardsJunction(observedAgent, junction)) {
        return JunctionSituation::JUNCTION_D;
    }
    junction = NextJunction(observedAgent);
    if (IsMovingTowardsJunction(*ego, junction)) {
        return JunctionSituation::JUNCTION_D;
    }

    return std::nullopt;
}
const MentalInfrastructure::Junction *RightOfWayInterpreter::NextJunction(const AgentRepresentation &agent) const {
    auto lane = agent.GetNextLane();
    for (unsigned int i = 0; i <= maxNumberLanesExtrapolation; i++) {
        if (lane && lane->IsJunctionLane()) {
            return lane->GetRoad()->GetJunction();
        }
        lane = lane ? lane->NextLane(agent.GetIndicatorState(), agent.IsMovingInLaneDirection()) : nullptr;
    }
    return nullptr;
}

bool RightOfWayInterpreter::IsMovingTowardsJunction(const AgentRepresentation &agent,
                                                    const MentalInfrastructure::Junction *junction) const {
    if (!junction)
        return false;
    auto currentLane = agent.GetLanePosition().lane;
    const MentalInfrastructure::Junction *agentJunction = nullptr;
    for (unsigned int i = 0; i <= maxNumberLanesExtrapolation; i++) {
        if (currentLane && currentLane->IsJunctionLane()) {
            agentJunction = currentLane->GetRoad()->GetJunction();
            if (junction == agentJunction) {
                return true;
            }
        }
        currentLane = currentLane ? currentLane->NextLane(agent.GetIndicatorState(), agent.IsMovingInLaneDirection()) : nullptr;
    }
    return junction == agentJunction;
}

} // namespace Interpreter
