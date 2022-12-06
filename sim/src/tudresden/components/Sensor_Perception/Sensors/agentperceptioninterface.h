/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
    virtual void CalculatePerception(const AgentInterface *agent, std::vector<InternWaypoint> route) = 0;

    ///
    /// \brief Returns a pointer to the ego perception, no calculation is performed.
    ///

protected:
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
};

#endif // DRIVERPERCEPTIONINTERFACE_H
