/********************************************************************************
 * Copyright (c) 2019-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "common/events/basicEvent.h"
#include "eventNetwork.h"
#include "fakeDataBuffer.h"
#include "fakeEvent.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::IsEmpty;
using ::testing::Ne;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SizeIs;

TEST(DISABLED_EventNetwork, InsertEvent_LogsEvent)
{
    NiceMock<FakeDataBuffer> mockDataBuffer;

    auto fakeEvent1 = std::make_shared<openpass::events::OpenScenarioEvent>(100, "test_event_1", "dont_care");
    auto fakeEvent2 = std::make_shared<openpass::events::OpenScenarioEvent>(200, "test_event_2", "dont_care");

    core::EventNetwork eventNetwork(&mockDataBuffer);

    eventNetwork.InsertEvent(fakeEvent1);
    eventNetwork.InsertEvent(fakeEvent2);

    EXPECT_CALL(mockDataBuffer, PutAcyclic(_, _, _)).Times(2);
}

TEST(EventNetwork, GetCategoryWithoutEvent_IsEmpty)
{
    NiceMock<FakeDataBuffer> fds;
    core::EventNetwork eventNetwork(&fds);
    ASSERT_THAT(eventNetwork.GetEvents(EventDefinitions::EventCategory::OpenSCENARIO), IsEmpty());
}

std::shared_ptr<EventInterface> GENERATE_FAKE_CONDITIONAL_EVENT()
{
    return std::make_shared<openpass::events::OpenScenarioEvent>(0, "dont_care", "dont_care");
}

TEST(EventNetwork, InsertEventOnEmptyNetwork_AddsToNewCategory)
{
    NiceMock<FakeDataBuffer> fds;
    core::EventNetwork eventNetwork(&fds);

    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());
    ASSERT_THAT(eventNetwork.GetEvents(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(1));
}

TEST(EventNetwork, InsertEventOfAlreadyAddedCategory_AddsToExistingCategory)
{
    NiceMock<FakeDataBuffer> fds;
    core::EventNetwork eventNetwork(&fds);

    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());
    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());

    ASSERT_THAT(eventNetwork.GetEvents(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(2));
}

TEST(EventNetwork, InsertEventOnNewCategory_AddsToNewCategory)
{
    NiceMock<FakeDataBuffer> fds;
    core::EventNetwork eventNetwork(&fds);

    auto fakeOpenPassEvent = std::make_shared<openpass::events::OpenPassEvent>(100, "test_event_1", "dont_care");
    eventNetwork.InsertEvent(fakeOpenPassEvent);
    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());

    ASSERT_THAT(eventNetwork.GetEvents(EventDefinitions::EventCategory::OpenPASS), SizeIs(1));
    ASSERT_THAT(eventNetwork.GetEvents(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(1));
}
