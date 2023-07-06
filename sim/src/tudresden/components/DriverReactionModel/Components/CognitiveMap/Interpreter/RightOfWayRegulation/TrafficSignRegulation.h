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

    RightOfWay VehicleVsVehicleROW(const AgentRepresentation &observedAgent, const WorldRepresentation &representation,
                                   const ConflictSituation &conflictSituation) override;

    virtual RightOfWay VehicleVsVehicleSameSigns(const AgentRepresentation &observedAgent, const WorldRepresentation &representation,
                                                 const ConflictSituation &conflictSituation) {
        return rblRegulation.VehicleVsVehicleROW(observedAgent, representation, conflictSituation);
    }

  private:
    RightBeforeLeftRegulationGermany rblRegulation;
};
} // namespace RightOfWayRegulation
