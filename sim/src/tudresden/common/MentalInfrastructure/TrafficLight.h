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
#pragma once

#include "Definitions.h"
#include "TrafficSignal.h"

namespace MentalInfrastructure {

class TrafficLight : public TrafficSignal {
public:
    TrafficLight(OdId openDriveId, DReaMId dreamId, const MentalInfrastructure::Road *road, double s, Common::Vector2d pos,
                 TrafficLightType type) :
        TrafficSignal(openDriveId, dreamId, road, s, pos), type(type) {
    }
    ~TrafficLight() = default;

    void SetState(TrafficLightState newState) {
        state = newState;
    }

    TrafficLightState GetState() const {
        return state;
    }

    TrafficLightType GetType() const {
        return type;
    }

private:
    TrafficLightState state{TrafficLightState::Off};
    TrafficLightType type;
};
} // namespace MentalInfrastructure
