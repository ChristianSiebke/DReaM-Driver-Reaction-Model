/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "RightBeforeLeftRegulation.h"

#include "common/Definitions.h"
#include "common/Helper.h"

namespace RightOfWayRegulation {

RightOfWay RightBeforeLeftRegulationGermany::VehicleVsVehicleROW(const AgentRepresentation& observedAgent,
                                                                 const WorldRepresentation& representation,const ConflictSituation &conflictSituation) {
    RightOfWay rightOfWay(true, true);
    if (representation.egoAgent->ObservedVehicleCameFromRight(observedAgent)) {
        rightOfWay.ego = false;
        rightOfWay.observed = true;
        return rightOfWay;
    } else {
        if (observedAgent.ObservedVehicleCameFromRight(*representation.egoAgent)) {
            rightOfWay.ego = true;
            rightOfWay.observed = false;
            return rightOfWay;
        } else {
            // agents vis-a-vis
            if (representation.egoAgent->GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                if (observedAgent.GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                    rightOfWay.ego = false;
                    rightOfWay.observed = false;
                    return rightOfWay;
                } else {
                    rightOfWay.ego = false;
                    rightOfWay.observed = true;
                    return rightOfWay;
                }
            } else if (observedAgent.GetIndicatorState() == IndicatorState::IndicatorState_Left) {
                rightOfWay.ego = true;
                rightOfWay.observed = false;
                return rightOfWay;
            }
        }
        return rightOfWay;
    }
}

}; // namespace RightOfWayRegulation
