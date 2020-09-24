/*******************************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "scenario.h"
#include <algorithm>
#include <qglobal.h>

namespace Configuration {

const std::string& Scenario::GetVehicleCatalogPath()
{
    return vehicleCatalogPath;
}

void Scenario::SetVehicleCatalogPath(const std::string& catalogPath)
{
    this->vehicleCatalogPath = catalogPath;
}

const std::string& Scenario::GetPedestrianCatalogPath()
{
    return pedestrianCatalogPath;
}

void Scenario::SetPedestrianCatalogPath(const std::string& catalogPath)
{
    this->pedestrianCatalogPath = catalogPath;
}

const std::string& Scenario::GetTrajectoryCatalogPath()
{
    return trajectoryCatalogPath;
}

void Scenario::SetTrajectoryCatalogPath(const std::string& catalogPath)
{
    trajectoryCatalogPath = catalogPath;
}

const std::string& Scenario::GetSceneryPath()
{
    return sceneryPath;
}

void Scenario::SetSceneryPath(const std::string& sceneryPath)
{
    this->sceneryPath = sceneryPath;
}

void Scenario::AddScenarioEntity(const ScenarioEntity& entity)
{
    entities.push_back(entity);
}

void Scenario::AddScenarioGroupsByEntityNames(const std::map<std::string, std::list<std::string>> &groupDefinitions)
{
    std::vector<ScenarioEntity*> groupEntities;
    for (auto groupDefinition : groupDefinitions)
    {
        for (const auto &memberName : groupDefinition.second)
        {
            const auto groupEntityIterator = std::find_if(entities.begin(),
                                                          entities.end(),
                                                          [&memberName](const auto entity)
                                                          {
                                                            return entity.name == memberName;
                                                          });
            groupEntities.push_back(&(*groupEntityIterator));
        }

        scenarioGroups.insert({groupDefinition.first, groupEntities});
    }
}

const std::vector<ScenarioEntity> &Scenario::GetEntities() const
{
    return entities;
}

const std::vector<ScenarioEntity*> &Scenario::GetScenarioEntities()
{
    try
    {
        return scenarioGroups.at("ScenarioAgents");
    }
    catch (const std::out_of_range& err)
    {
        Q_UNUSED(err);
        throw std::runtime_error("ScenarioAgents group not found.");
    }
}

const std::map<std::string, std::vector<ScenarioEntity*>> &Scenario::GetScenarioGroups()
{
    return scenarioGroups;
}

void Scenario::AddConditionalEventDetector(const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation)
{
    eventDetectorInformations.emplace_back(eventDetectorInformation);
}

void Scenario::AddAction(const openScenario::Action action, const std::string eventName)
{
    actions.emplace_back(action, eventName);
}

const std::vector<openScenario::ConditionalEventDetectorInformation>& Scenario::GetEventDetectorInformations()
{
    return eventDetectorInformations;
}

std::vector<openScenario::ManipulatorInformation> Scenario::GetActions() const
{
    return actions;
}

int Scenario::GetEndTime() const
{
    // we add plus one here to align with the "greater_than" rule
    // Time is parsed in seconds, but we use ms internally (* 1000)
    return static_cast<int>((std::rint(endTimeInSeconds * 1000))) + 1;
}

void Scenario::SetEndTime(const double endTime)
{
    this->endTimeInSeconds = endTime;
}

openScenario::EnvironmentAction Scenario::GetEnvironment()
{
    return environment;
}

void Scenario::SetEnvironment(const openScenario::EnvironmentAction environment)
{
    this->environment = environment;
}

} // namespace SimulationSlave
