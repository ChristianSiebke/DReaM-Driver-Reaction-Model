/******************************************************************************
 * Copyright (c) 2021 TU Dresden
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
#ifndef DRIVERPERCEPTION_H
#define DRIVERPERCEPTION_H

#include "agentperceptioninterface.h"

class DriverPerception : public AgentPerceptionInterface {
  public:
    DriverPerception(const AgentInterface* egoAgent, WorldInterface* world,
                     std::shared_ptr<InfrastructurePerception> infrastructurePerception)
        : AgentPerceptionInterface(egoAgent, world, infrastructurePerception) {}
    ~DriverPerception() override = default;

    ///
    /// \brief Calculates what the internal attributes of the agent are at the current point in time and stores this data in an EgoResult.
    /// \return A pointer to the EgoPerception where the data is stored.
    ///
    void CalculatePerception(const AgentInterface* agent) override;

    std::shared_ptr<EgoPerception> GetEgoPerception() { return egoPerception; }

  private:
    Common::Vector2d GetDriverPosition();

    double DetermineLateralDisplacement(bool referenceLaneInDirection, const AgentPerception* agent);

    double DetermineRoadHeading(bool referenceLaneInDirection, const AgentPerception* agent);

    ///
    /// \brief Helper method to set the value of the curvature of the road at the position of the driver.
    ///
    double DetermineDriverCurvature(bool laneInRoadDirection);

  private:
    std::shared_ptr<EgoPerception> egoPerception;
};

#endif // DRIVERPERCEPTION_H
