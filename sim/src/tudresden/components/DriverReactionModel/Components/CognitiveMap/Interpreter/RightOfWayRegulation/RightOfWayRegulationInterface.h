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
