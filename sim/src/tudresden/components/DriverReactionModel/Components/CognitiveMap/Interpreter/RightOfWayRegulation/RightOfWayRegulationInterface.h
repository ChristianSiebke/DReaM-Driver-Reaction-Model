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
#include "Common/WorldRepresentation.h"
namespace RightOfWayRegulation {

class RightOfWayRegulationInterface {
  public:
    RightOfWayRegulationInterface() {}
    virtual ~RightOfWayRegulationInterface() = default;

    virtual RightOfWay RightOfWayDetermination(const AgentRepresentation& observedAgent, const WorldRepresentation& representation);

  protected:
    template <typename T> bool IsVehicle(const T& agentdata);
    template <typename T> bool IsPedestrian(const T& agentdata);
    virtual bool PedestrianVsVehicleROW(const AgentRepresentation& pedestrianAgent, const AgentRepresentation& vehicleAgent);
    virtual RightOfWay VehicleVsVehicleROW(const AgentRepresentation& observedAgent, const WorldRepresentation& representation) = 0;
};
} // namespace RightOfWayRegulation
