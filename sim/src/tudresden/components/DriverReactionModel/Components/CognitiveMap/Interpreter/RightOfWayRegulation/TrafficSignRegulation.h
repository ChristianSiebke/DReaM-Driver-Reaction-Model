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
#include "RightBeforeLeftRegulation.h"
#include "RightOfWayRegulationInterface.h"
namespace RightOfWayRegulation {

class TrafficSignRegulationGermany : public RightOfWayRegulationInterface {
  public:
    TrafficSignRegulationGermany() {}
    virtual ~TrafficSignRegulationGermany() override = default;

    RightOfWay VehicleVsVehicleROW(const AgentRepresentation& observedAgent, const WorldRepresentation& representation) override;

    virtual RightOfWay VehicleVsVehicleSameSigns(const AgentRepresentation& observedAgent, const WorldRepresentation& representation) {
        return rblRegulation.VehicleVsVehicleROW(observedAgent, representation);
    }

  private:
    RightBeforeLeftRegulationGermany rblRegulation;
};
} // namespace RightOfWayRegulation
