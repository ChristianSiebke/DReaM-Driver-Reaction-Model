/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "eventNetwork.h"

#include "fakeEvent.h"

using ::testing::Ne;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SizeIs;

TEST(EventNetwork_UnitTests, InsertEventInNotExistingCategory)
{
    auto fakeEvent = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEvent, GetCategory()).Times(1).WillOnce(Return(EventDefinitions::EventCategory::Conditional));

    SimulationSlave::EventNetwork eventNetwork;

    eventNetwork.InsertEvent(fakeEvent);

    std::list<std::shared_ptr<EventInterface>> resultAgentBasedEventList =
        eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::Conditional);

    ASSERT_EQ(resultAgentBasedEventList.size(), size_t(1));
}

TEST(EventNetwork_UnitTests, InsertEventInExistingCategory)
{
    auto fakeEvent = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEvent, GetCategory()).Times(1).WillOnce(Return(EventDefinitions::EventCategory::Conditional));

    std::list<std::shared_ptr<EventInterface>> fakeAgentBasedEventList;
    fakeAgentBasedEventList.push_back(fakeEvent);

    Events fakeEvents{{EventDefinitions::EventCategory::Conditional, fakeAgentBasedEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetActiveEvents()) = fakeEvents;

    eventNetwork.InsertEvent(fakeEvent);

    std::list<std::shared_ptr<EventInterface>> resultAgentBasedEventList =
        eventNetwork.GetActiveEvents()->at(EventDefinitions::EventCategory::Conditional);

    ASSERT_EQ(resultAgentBasedEventList.size(), size_t(2));
}

TEST(EventNetwork_UnitTests, GetActiveEventCategoryWithOneExisting)
{
    auto fakeEvent = std::make_shared<FakeEvent>();

    EventContainer fakeEventList;
    fakeEventList.push_back(fakeEvent);

    Events fakeEvents {{EventDefinitions::EventCategory::Conditional, fakeEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetActiveEvents()) = fakeEvents;

    EventContainer resultEventList = eventNetwork.GetActiveEventCategory(EventDefinitions::EventCategory::Conditional);

    ASSERT_THAT(resultEventList, SizeIs(Ne(0)));
}

TEST(EventNetwork_UnitTests, GetActiveEventCategoryWithoutExisting)
{
    EventContainer fakeEventList;
    Events fakeEvents {{EventDefinitions::EventCategory::Basic, fakeEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetActiveEvents()) = fakeEvents;

    EventContainer resultEventList = eventNetwork.GetActiveEventCategory(EventDefinitions::EventCategory::Conditional);

    ASSERT_THAT(resultEventList, SizeIs(0));
}

TEST(EventNetwork_UnitTests, RemoveOldEventsWithTwoOldEventsOfTheSameCategory)
{
    int actualTime = 3;

    auto fakeEvent = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEvent, GetEventTime()).Times(3)
                                           .WillOnce(Return(1))
                                           .WillOnce(Return(2))
                                           .WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEvent);
    fakeConditionalEvents.push_back(fakeEvent);
    fakeConditionalEvents.push_back(fakeEvent);

    Events fakeEvents {{EventDefinitions::EventCategory::Conditional, fakeConditionalEvents}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer resultEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Conditional);

    ASSERT_THAT(resultEventList, SizeIs(1));
}

TEST(EventNetwork_UnitTests, RemoveOldEventsWithTwoOldEventsOfDifferentCategories)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(2)
                                           .WillOnce(Return(1))
                                           .WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEventFirst);
    fakeConditionalEvents.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(2)
                                                 .WillOnce(Return(1))
                                                 .WillOnce(Return(actualTime));

    EventContainer fakeBasicEvents;
    fakeBasicEvents.push_back(fakeEventSecond);
    fakeBasicEvents.push_back(fakeEventSecond);

    Events fakeEvents {{EventDefinitions::EventCategory::Conditional, fakeConditionalEvents},
                       {EventDefinitions::EventCategory::Basic, fakeBasicEvents}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer conditionalEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Conditional);
    EventContainer basicEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(conditionalEvents, SizeIs(1));
    ASSERT_THAT(basicEvents, SizeIs(1));
}

TEST(EventNetwork_UnitTests, RemoveOldEventsWithNoOldEvents)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(1)
                                                .WillOnce(Return(actualTime));

    EventContainer fakeConditionalEvents;
    fakeConditionalEvents.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(1)
                                                 .WillOnce(Return(actualTime));

    EventContainer fakeBasicEvents;
    fakeBasicEvents.push_back(fakeEventSecond);

    Events fakeEvents {{EventDefinitions::EventCategory::Conditional, fakeConditionalEvents},
                       {EventDefinitions::EventCategory::Basic, fakeBasicEvents}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer conditionalEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Conditional);
    EventContainer basicEvents = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(conditionalEvents, SizeIs(1));
    ASSERT_THAT(basicEvents, SizeIs(1));
}
