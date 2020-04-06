/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "eventNetwork.h"
#include "fakeDataStore.h"
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
    NiceMock<FakeDataStore> mockDataStore;

    std::vector<int> setOf3Ids{1, 2, 3};
    std::vector<int> setOf2Ids{4, 5};
    auto fakeEvent1 = std::make_shared<BasicEvent>(100, "test_event_1", "dont_care", setOf3Ids, setOf2Ids);
    auto fakeEvent2 = std::make_shared<BasicEvent>(200, "test_event_2", "dont_care", setOf2Ids, setOf3Ids);

    SimulationSlave::EventNetwork eventNetwork(&mockDataStore);

    eventNetwork.InsertEvent(fakeEvent1);
    eventNetwork.InsertEvent(fakeEvent2);

    EXPECT_CALL(mockDataStore, PutAcyclic(_, _, _, _)).Times(2);
}

TEST(EventNetwork, GetCategoryWithoutEvent_IsEmpty)
{
    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);
    ASSERT_THAT(eventNetwork.GetActiveEventCategory(EventDefinitions::EventCategory::OpenSCENARIO), IsEmpty());
}

std::shared_ptr<EventInterface> GENERATE_FAKE_CONDITIONAL_EVENT()
{
    return std::make_shared<ConditionalEvent>(0, "dont_care", "dont_care", std::vector<int>{}, std::vector<int>{});
}

TEST(EventNetwork, InsertEventOnEmptyNetwork_AddsToNewCategory)
{
    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());
    ASSERT_THAT(eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(1));
}

TEST(EventNetwork, InsertEventOfAlreadyAddedCategory_AddsToExistingCategory)
{
    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    eventNetwork.GetActiveEvents()->insert({EventDefinitions::EventCategory::OpenSCENARIO, {GENERATE_FAKE_CONDITIONAL_EVENT()}});

    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());

    ASSERT_THAT(eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(2));
}

TEST(EventNetwork, InsertEventOnNewCategory_AddsToNewCategory)
{
    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    eventNetwork.GetActiveEvents()->insert({EventDefinitions::EventCategory::Basic,
                                            {std::make_shared<BasicEvent>(0, "dont_care", "dont_care")}});

    eventNetwork.InsertEvent(GENERATE_FAKE_CONDITIONAL_EVENT());

    ASSERT_THAT(eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::Basic), SizeIs(1));
    ASSERT_THAT(eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO), SizeIs(1));
}

TEST(EventNetwork, RemoveOldEventsWithTwoOldEventsOfTheSameCategory)
{
    int actualTime = 3;

    auto fakeEvent = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEvent, GetEventTime()).Times(3).WillOnce(Return(1)).WillOnce(Return(2)).WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEvent);
    fakeConditionalEvents.push_back(fakeEvent);
    fakeConditionalEvents.push_back(fakeEvent);

    Events fakeEvents{{EventDefinitions::EventCategory::OpenSCENARIO, fakeConditionalEvents}};

    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer resultEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO);

    ASSERT_THAT(resultEventList, SizeIs(1));
}

TEST(EventNetwork, RemoveOldEventsWithTwoOldEventsOfDifferentCategories)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(2).WillOnce(Return(1)).WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEventFirst);
    fakeConditionalEvents.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(2).WillOnce(Return(1)).WillOnce(Return(actualTime));

    EventContainer fakeBasicEvents;
    fakeBasicEvents.push_back(fakeEventSecond);
    fakeBasicEvents.push_back(fakeEventSecond);

    Events fakeEvents{{EventDefinitions::EventCategory::OpenSCENARIO, fakeConditionalEvents},
                      {EventDefinitions::EventCategory::Basic, fakeBasicEvents}};

    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer conditionalEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO);
    EventContainer basicEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(conditionalEvents, SizeIs(1));
    ASSERT_THAT(basicEvents, SizeIs(1));
}

TEST(EventNetwork, RemoveOldEventsWithNoOldEvents)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(1).WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(1).WillOnce(Return(actualTime));

    EventContainer fakeBasicEvents;
    fakeBasicEvents.push_back(fakeEventSecond);

    Events fakeEvents{{EventDefinitions::EventCategory::OpenSCENARIO, fakeConditionalEvents},
                      {EventDefinitions::EventCategory::Basic, fakeBasicEvents}};

    NiceMock<FakeDataStore> fds;
    SimulationSlave::EventNetwork eventNetwork(&fds);

    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer conditionalEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::OpenSCENARIO);
    EventContainer basicEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(conditionalEvents, SizeIs(1));
    ASSERT_THAT(basicEvents, SizeIs(1));
}
