/*********************************************************************
* Copyright (c) 2019, 2020, 2021 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/scenarioInterface.h"

class FakeScenario : public ScenarioInterface
{
public:
    MOCK_METHOD0(GetScenarioEntities,
                 const std::vector<ScenarioEntity*>&());
    MOCK_METHOD0(GetVehicleCatalogPath,
                 const std::string & ());
    MOCK_METHOD1(SetVehicleCatalogPath,
                 void(const std::string&));
    MOCK_METHOD0(GetPedestrianCatalogPath,
                 const std::string & ());
    MOCK_METHOD1(SetPedestrianCatalogPath,
                 void(const std::string&));
    MOCK_METHOD0(GetTrajectoryCatalogPath,
                 const std::string & ());
    MOCK_METHOD1(SetTrajectoryCatalogPath,
                 void(const std::string&));
    MOCK_METHOD0(GetSceneryPath,
                 const std::string & ());
    MOCK_METHOD1(SetSceneryPath,
                 void(const std::string&));
    MOCK_CONST_METHOD0(GetTrafficSignalControllers,
                       const std::vector<openScenario::TrafficSignalController>& ());
    MOCK_METHOD1(AddTrafficSignalController,
                 void (const openScenario::TrafficSignalController& controller));
    MOCK_METHOD1(AddScenarioEntity,
                 void(const ScenarioEntity&));
    MOCK_METHOD1(AddScenarioGroupsByEntityNames,
                 void(const std::map<std::string, std::list<std::string>>&));
    MOCK_METHOD1(AddConditionalEventDetector,
                 void(const openScenario::ConditionalEventDetectorInformation&));
    MOCK_METHOD2(AddAction,
                 void(const openScenario::Action, const std::string));
    MOCK_METHOD0(GetEventDetectorInformations,
                 std::vector<openScenario::ConditionalEventDetectorInformation>&());
    MOCK_METHOD0(GetScenarioGroups,
                 const std::map<std::string, std::vector<ScenarioEntity*>>&());
    MOCK_CONST_METHOD0(GetActions,
                       std::vector<openScenario::ManipulatorInformation>());
    MOCK_CONST_METHOD0(GetEndTime,
                       int());
    MOCK_METHOD1(SetEndTime,
                 void(const double));
    MOCK_CONST_METHOD0(GetEntities,
                       const std::vector<ScenarioEntity>&());
};
