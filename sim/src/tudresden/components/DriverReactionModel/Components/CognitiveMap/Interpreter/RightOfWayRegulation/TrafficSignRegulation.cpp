/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "TrafficSignRegulation.h"
#include "Helper.h"
namespace RightOfWayRegulation {
RightOfWay TrafficSignRegulationGermany::VehicleVsVehicleROW(const AgentRepresentation &observedAgent,
                                                             const WorldRepresentation &representation,
                                                             const ConflictSituation &conflictSituation) {
    RightOfWay egoPriority(true, false);
    RightOfWay observedAgentPriority(false, true);
    assert(!conflictSituation.oAgentCA->lane->GetPredecessors().empty() && !conflictSituation.egoCA->lane->GetPredecessors().empty());

    const MentalInfrastructure::TrafficSign *oAgentSign = nullptr;
    if (conflictSituation.junction != observedAgent.NextJunction() &&
        Common::AgentTouchesLane(&observedAgent, conflictSituation.oAgentCA->lane->GetPredecessors().front())) {
        oAgentSign =
            representation.trafficSignalMemory->GetRightOfWaySignsForLane(conflictSituation.oAgentCA->lane->GetPredecessors().front());
    }
    else {
        oAgentSign = representation.trafficSignalMemory->GetRightOfWaySignsForLane(observedAgent.GetLanePosition().lane);
    }

    const MentalInfrastructure::TrafficSign *egoSign = nullptr;
    if (conflictSituation.junction != representation.egoAgent->NextJunction() &&
        Common::AgentTouchesLane(representation.egoAgent, conflictSituation.egoCA->lane->GetPredecessors().front())) {
        egoSign = representation.trafficSignalMemory->GetRightOfWaySignsForLane(conflictSituation.egoCA->lane->GetPredecessors().front());
    }
    else {
        egoSign = representation.trafficSignalMemory->GetRightOfWaySignsForLane(representation.egoAgent->GetLanePosition().lane);
    }

    if ((egoSign && oAgentSign)) {
        if (egoSign->GetPriority() == oAgentSign->GetPriority()) {
            return VehicleVsVehicleSameSigns(observedAgent, representation, conflictSituation);
        }
        else {
            return egoSign->GetPriority() > oAgentSign->GetPriority() ? egoPriority : observedAgentPriority;
        }
    }
    else if (egoSign) {
        // if traffic sign of obsevedAgent is not yet detected
        return egoSign->GetPriority() > 0 ? egoPriority : observedAgentPriority;
    }
    else if (oAgentSign) {
        // if traffic sign of egoAgent is not yet detected
        return oAgentSign->GetPriority() > 0 ? observedAgentPriority : egoPriority;
    }
    else {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " error in Traffic Sign Regulation";
        throw std::logic_error(msg);
    }
}
} // namespace RightOfWayRegulation
