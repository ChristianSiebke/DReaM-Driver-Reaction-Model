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
#ifndef DRIVERPERCEPTIONINTERFACE_H
#define DRIVERPERCEPTIONINTERFACE_H

#include "WorldData.h"
#include "common/Helper.h"
#include "simplesensorinterface.h"

struct DriverPositionOnLane {
    double sCoordinate;
    double distanceToNextIntersection;
    double distanceOnIntersection;
};

class AgentPerceptionInterface : protected SimpleSensorInterface {
public:
    AgentPerceptionInterface(const AgentInterface *egoAgent, WorldInterface *world,
                             std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
        SimpleSensorInterface(egoAgent, world), infrastructurePerception(infrastructurePerception) {
    }
    virtual ~AgentPerceptionInterface() = default;

    ///
    /// \brief Calculates the ego perception and returns a pointer to the generated object.
    ///
    virtual void CalculatePerception(const AgentInterface *agent) = 0;

    ///
    /// \brief Returns a pointer to the ego perception, no calculation is performed.
    ///

protected:
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
};

#endif // DRIVERPERCEPTIONINTERFACE_H
