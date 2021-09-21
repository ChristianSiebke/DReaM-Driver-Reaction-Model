/*******************************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
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
#include "fakeDataBuffer.h"
#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::DontCare;
using ::testing::NiceMock;
using ::testing::NotNull;
using ::testing::Return;
using ::testing::ReturnRef;

using namespace openpass::databuffer;
using namespace openpass::publisher;
using namespace openpass::type;

TEST(AgentDataPublisher, CallingPublish_ForwardsParametersToDataBuffer)
{
    FakeDataBuffer fakeDataBuffer;

    const EntityId agentId = 1;
    const Key key = "theKey";
    const Value value = 2;

    auto adp = std::make_unique<AgentDataPublisher>(&fakeDataBuffer, agentId);

    EXPECT_CALL(fakeDataBuffer, PutCyclic(agentId, key, value));

    ASSERT_THAT(adp, NotNull());

    adp->Publish(key, value);
}
