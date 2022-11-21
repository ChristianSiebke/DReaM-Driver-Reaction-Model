/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Trafficsign.h"

namespace MentalInfrastructure {

int TrafficSign::Priority(TrafficSignType sign) {
    switch (sign) {
    case TrafficSignType::RightOfWayNextIntersection:
    case TrafficSignType::RightOfWayBegin:
        return 1;
    case TrafficSignType::Stop:
    case TrafficSignType::GiveWay:
        return -1;
    default:
        return 0;
    }
};

} // namespace MentalInfrastructure
