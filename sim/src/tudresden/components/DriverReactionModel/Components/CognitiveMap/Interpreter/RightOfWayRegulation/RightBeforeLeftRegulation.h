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
#include "RightOfWayRegulationInterface.h"

namespace RightOfWayRegulation {

class RightBeforeLeftRegulationGermany : public RightOfWayRegulationInterface {
  public:
    RightBeforeLeftRegulationGermany() {}
    virtual ~RightBeforeLeftRegulationGermany() override = default;

    RightOfWay VehicleVsVehicleROW(const AgentRepresentation& observedAgent, const WorldRepresentation& representation) override;
};
} // namespace RightOfWayRegulation
