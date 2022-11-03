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

#include "TrafficSignal.h"
#include "common/worldDefinitions.h"

namespace MentalInfrastructure {

class TrafficLight : public TrafficSignal {
public:
    TrafficLight(OdId openDriveId, DReaMId dreamId, const MentalInfrastructure::Road *road, double s, Common::Vector2d pos,
                 CommonTrafficLight::Type type) :
        TrafficSignal(openDriveId, dreamId, road, s, pos), type(type) {
    }
    ~TrafficLight() = default;

    const Common::Vector2d GetPosition() const {
        return position;
    }

    double GetS() const {
        return s;
    }

    const MentalInfrastructure::Road *GetRoad() const {
        return road;
    }

    void SetState(CommonTrafficLight::State newState) {
        state = newState;
    }

    CommonTrafficLight::State GetState() const {
        return state;
    }

    CommonTrafficLight::Type GetType() const {
        return type;
    }

private:
    CommonTrafficLight::State state{CommonTrafficLight::State::Off};
    CommonTrafficLight::Type type;

    const MentalInfrastructure::Road *road;
    double s;
    Common::Vector2d position;
};
} // namespace MentalInfrastructure
