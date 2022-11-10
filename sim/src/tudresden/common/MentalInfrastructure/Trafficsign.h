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
#include "unordered_map"

namespace MentalInfrastructure {

class TrafficSign : public TrafficSignal {
public:
    TrafficSign(OdId openDriveId, DReaMId dreamId, const MentalInfrastructure::Road *road, double s, Common::Vector2d pos, double v,
                CommonTrafficSign::Type commonType) :
        TrafficSignal(openDriveId, dreamId, road, s, pos), value(v) {
        type = (trafficSignMapping.at(commonType));
        priority = Priority(type);
    }
    ~TrafficSign() = default;

    double GetValue() const {
        return value;
    }

    TrafficSignType GetType() const {
        return type;
    }

    int GetPriority() const {
        return priority;
    }

private:
    int Priority(TrafficSignType sign);

    TrafficSignType type;
    int priority;
    double value;
};
} // namespace MentalInfrastructure
