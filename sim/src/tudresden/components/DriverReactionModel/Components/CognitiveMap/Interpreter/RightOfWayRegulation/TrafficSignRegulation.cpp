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
RightOfWay TrafficSignRegulationGermany::VehicleVsVehicleROW(const AgentRepresentation& observedAgent,
                                                             const WorldRepresentation& representation) {
    RightOfWay egoPriority(true, false);
    RightOfWay observedAgentPriority(false, true);

    auto oAgentSign = observedAgent.NextROWSign();
    auto egoSign = representation.egoAgent->NextROWSign();

    if ((egoSign && oAgentSign)) {
        if (egoSign->GetPriority() == oAgentSign->GetPriority()) {
            return VehicleVsVehicleSameSigns(observedAgent, representation);
        } else {
            return egoSign->GetPriority() > oAgentSign->GetPriority() ? egoPriority : observedAgentPriority;
        }
    } else if (egoSign) {
        // if traffic sign of obsevedAgent is not yet detected
        return egoSign->GetPriority() > 0 ? egoPriority : observedAgentPriority;
    } else if (oAgentSign) {
        // if traffic sign of egoAgent is not yet detected
        return oAgentSign->GetPriority() > 0 ? observedAgentPriority : egoPriority;
    } else {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " error in Traffic Sign Regulation";
        throw std::logic_error(msg);
    }
}
} // namespace RightOfWayRegulation
