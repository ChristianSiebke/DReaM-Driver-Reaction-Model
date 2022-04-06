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

#include <variant>

#include <QCommandLineParser>
#include <qcoreapplication.h>

#include "RouteImporter.h"
#include "agentperceptioninterface.h"
#include "core/opSimulation/framework/commandLineParser.h"
class DriverPerception : public AgentPerceptionInterface {
  public:
      DriverPerception(const AgentInterface *egoAgent, WorldInterface *world,
                       std::shared_ptr<InfrastructurePerception> infrastructurePerception) :
          AgentPerceptionInterface(egoAgent, world, infrastructurePerception) {
          // TODO: waypoints must be passed via the openPASS framework.
          // So far the openPASS framework (AgentInterface/egoAgent) has no
          // function to get the waypoints  -->  redundante import
          auto arguments = QCoreApplication::arguments();
          CommandLineArguments parsedArguments = CommandLineParser::Parse(arguments);
          std::string scenarioConfigPath =
              QCoreApplication::applicationDirPath().toStdString() + "\\" + parsedArguments.configsPath + "\\" + "Scenario.xosc";
          RouteImporter routeImporter(scenarioConfigPath, nullptr);
          auto waypoints = routeImporter.GetDReaMRoute(egoAgent->GetScenarioName());
          std::vector<DReaMRoute::Waypoint> result;
          std::transform(waypoints.begin(), waypoints.end(), std::back_inserter(result), [&infrastructurePerception](auto element) {
              DReaMRoute::Waypoint result;
              if (auto target = std::get_if<Import::RoadPosition>(&element)) {
                  result.lane = infrastructurePerception->GetLane(target->roadId, target->t, target->s);
                  result.s = target->s;
                  result.roadId = target->roadId;
              }
              else if (auto target = std::get_if<Import::LanePosition>(&element)) {
                  result.lane = infrastructurePerception->GetLane(target->roadId, target->laneId, target->s);
                  result.s = target->s;
                  result.roadId = target->roadId;
              }
              else {
                  throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Waypoint is not supported");
              }
              return result;
          });
          route = result;
          //-----
      }
    ~DriverPerception() override = default;

    ///
    /// \brief Calculates what the internal attributes of the agent are at the current point in time and stores this data in an EgoResult.
    /// \return A pointer to the EgoPerception where the data is stored.
    ///
    void CalculatePerception(const AgentInterface* agent) override;

    std::shared_ptr<EgoPerception> GetEgoPerception() { return egoPerception; }

  private:
      Common::Vector2d GetDriverPosition();
      std::shared_ptr<EgoPerception> egoPerception;
      DReaMRoute::Waypoints route;
};

#endif // DRIVERPERCEPTION_H
