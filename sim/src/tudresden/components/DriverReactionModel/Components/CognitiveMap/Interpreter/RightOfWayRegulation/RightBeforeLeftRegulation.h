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
#include "RightOfWayRegulationInterface.h"

namespace RightOfWayRegulation {

class RightBeforeLeftRegulationGermany : public RightOfWayRegulationInterface {
  public:
    RightBeforeLeftRegulationGermany() {}
    virtual ~RightBeforeLeftRegulationGermany() override = default;

    RightOfWay VehicleVsVehicleROW(const AgentRepresentation& observedAgent, const WorldRepresentation& representation) override;
};
} // namespace RightOfWayRegulation
