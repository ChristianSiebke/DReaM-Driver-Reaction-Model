/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "AgentAdapter.h"

#include "fakeWorld.h"
#include "fakeSection.h"
#include "fakeLane.h"
#include "fakeWorldData.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeAgent.h"

#include "Generators/laneGeometryElementGenerator.h"

using namespace OWL;

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;


struct Entity
{
    AgentInterface& agent;
    Fakes::MovingObject& movingObject;
};

class AgentManager
{
    std::list<AgentAdapter*> agents;
    const std::unordered_map<OWL::Id, OWL::Interfaces::Road*> roads;

public:
    FakeWorld fakeWorld;
    Fakes::WorldData fakeWorldData;
    std::vector<Fakes::MovingObject*> fakeMovingObjects;

    AgentManager()
    {
        ON_CALL(fakeWorld, GetWorldData()).
        WillByDefault(Return(static_cast<void*>(&fakeWorldData)));
        ON_CALL(fakeWorldData, GetRoads()).
        WillByDefault(ReturnRef(roads));
    }

    Entity Create(double distanceToRoadStart, double distanceToRoadEnd)
    {
        // do not change order, as agent adapter needs the faked world data

        auto fakeMovingObject = new Fakes::MovingObject();
        ON_CALL(fakeWorldData, AddMovingObject(_, _)).WillByDefault(ReturnRef(*fakeMovingObject));
        World::Localization::Localizer localizer{fakeWorldData};

        auto agent = new AgentAdapter(&fakeWorld, nullptr, &fakeWorldData, localizer);
        fakeMovingObject->SetLinkedObjectForTesting(static_cast<void*>(agent));

        ON_CALL(*fakeMovingObject, GetDistance(OWL::MeasurementPoint::RoadStart,_)).WillByDefault(Return(distanceToRoadStart));
        ON_CALL(*fakeMovingObject, GetDistance(OWL::MeasurementPoint::RoadEnd,_)).WillByDefault(Return(distanceToRoadEnd));

        agents.push_back(agent);
        fakeMovingObjects.push_back(fakeMovingObject);

        return { *agent, *fakeMovingObject };
    }

    ~AgentManager()
    {
        for (auto agent : agents) { delete agent; }
        for (auto fakeMovingObject : fakeMovingObjects) { delete fakeMovingObject; }
    }
};
