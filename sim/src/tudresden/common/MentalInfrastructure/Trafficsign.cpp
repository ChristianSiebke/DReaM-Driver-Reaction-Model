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
