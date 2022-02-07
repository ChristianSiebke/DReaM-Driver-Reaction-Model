/******************************************************************************
 * Copyright (c) 2020 TU Dresden
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
#include "RightBeforeLeftRegulation.h"

#include "common/Definitions.h"
#include "common/Helper.h"

namespace RightOfWayRegulation {

RightOfWay RightBeforeLeftRegulationGermany::VehicleVsVehicleROW(const AgentRepresentation& observedAgent,
                                                                 const WorldRepresentation& representation) {
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
