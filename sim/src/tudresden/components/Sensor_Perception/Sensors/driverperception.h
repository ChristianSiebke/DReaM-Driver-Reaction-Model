/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef DRIVERPERCEPTION_H
#define DRIVERPERCEPTION_H

#include <QCommandLineParser>
#include <qcoreapplication.h>

#include "RouteConverter.h"
#include "agentperceptioninterface.h"
class DriverPerception : public AgentPerceptionInterface {
  public:
      DriverPerception(const AgentInterface *egoAgent, WorldInterface *world,
                       std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
          AgentPerceptionInterface(egoAgent, world, infrastructurePerception) {
      }
    ~DriverPerception() override = default;

    ///
    /// \brief Calculates what the internal attributes of the agent are at the current point in time and stores this data in an EgoResult.
    /// \return A pointer to the EgoPerception where the data is stored.
    ///
    void CalculatePerception(const AgentInterface *agent, std::vector<InternWaypoint> route) override;

    std::shared_ptr<EgoPerception> GetEgoPerception() { return egoPerception; }

  private:
      Common::Vector2d GetDriverPosition();
      std::shared_ptr<EgoPerception> egoPerception;
};

#endif // DRIVERPERCEPTION_H
