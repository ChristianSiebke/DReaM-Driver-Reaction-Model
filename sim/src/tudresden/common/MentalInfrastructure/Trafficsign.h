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

#include "Definitions.h"
#include "TrafficSignal.h"
#include "unordered_map"

namespace MentalInfrastructure {

class TrafficSign : public TrafficSignal {
public:
    TrafficSign(const OdId &openDriveId, DReaMId dreamId, const MentalInfrastructure::Road *road, double s, Common::Vector2d pos, double v,
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
