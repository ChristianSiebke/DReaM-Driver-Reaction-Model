/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "agentDataPublisher.h"
#include "dontCare.h"
#include "fakeCallback.h"
#include "fakeDataStore.h"
#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "scheduler/timeKeeper.h"

using ::testing::_;
using ::testing::DontCare;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::ReturnRef;

using namespace openpass::datastore;
using namespace openpass::publisher;
using namespace openpass::type;

struct TimeManipulatior : public openpass::scheduling::TimeKeeper
{
    static void Set(int faketime)
    {
        time = faketime;
    }
};

TEST(AgentDataPublisher, CallingPublish_ForwardsParametersToDataStore)
{
    FakeDataStore fakeDataStore;

    const Timestamp timestamp = 3;
    const EntityId agentId = 1;
    const Key key = "theKey";
    const Value value = 2;

    auto adp = std::make_unique<AgentDataPublisher>(&fakeDataStore, agentId);

    TimeManipulatior::Set(timestamp);
    EXPECT_CALL(fakeDataStore, PutCyclic(timestamp, agentId, key, value));

    ASSERT_THAT(adp, NotNull());

    adp->Publish(key, value);
}

TEST(AgentDataPublisher, UpdatedScheulderTime_IsAccessibleByPublisher)
{
    FakeDataStore fakeDataStore;

    const EntityId agentId = 1;
    const Key key = "theKey";
    const Value value = 2;
    const Timestamp timestep1 = 1;
    const Timestamp timestep2 = 2;

    auto adp = std::make_unique<AgentDataPublisher>(&fakeDataStore, agentId);

    EXPECT_CALL(fakeDataStore, PutCyclic(timestep1, _, _, _));
    EXPECT_CALL(fakeDataStore, PutCyclic(timestep2, _, _, _));

    ASSERT_THAT(adp, NotNull());

    TimeManipulatior::Set(timestep1);
    adp->Publish(key, value);

    TimeManipulatior::Set(timestep2);
    adp->Publish(key, value);
}

TEST(AgentDataPublisher, PublishingSameTimestampTwice_Succeeds)
{
    FakeDataStore fakeDataStore;

    const EntityId agentId = 1;
    const Key key = "theKey";
    const Value value = 2;
    const Timestamp timestep = 1;

    auto adp = std::make_unique<AgentDataPublisher>(&fakeDataStore, agentId);

    EXPECT_CALL(fakeDataStore, PutCyclic(timestep, _, _, _)).Times(2);

    ASSERT_THAT(adp, NotNull());

    TimeManipulatior::Set(timestep);
    ASSERT_NO_THROW(adp->Publish(key, value));
    ASSERT_NO_THROW(adp->Publish(key, value));
}
