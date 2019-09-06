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

TEST(EventNetwork_UnitTests, GetActiveEventCategoryWithOneExisting)
{
    auto fakeEvent = std::make_shared<FakeEvent>();

    EventContainer fakeEventList;
    fakeEventList.push_back(fakeEvent);

    Events fakeEvents {{EventDefinitions::EventCategory::AgentBasedManipulation, fakeEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetActiveEvents()) = fakeEvents;

    EventContainer resultEventList = eventNetwork.GetActiveEventCategory(EventDefinitions::EventCategory::AgentBasedManipulation);

    ASSERT_THAT(resultEventList, SizeIs(Ne(0)));
}

TEST(EventNetwork_UnitTests, GetActiveEventCategoryWithoutExisting)
{
    EventContainer fakeEventList;
    Events fakeEvents {{EventDefinitions::EventCategory::Basic, fakeEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetActiveEvents()) = fakeEvents;

    EventContainer resultEventList = eventNetwork.GetActiveEventCategory(EventDefinitions::EventCategory::AgentBasedManipulation);

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

    EventContainer fakeAgentBasedEventList;
    fakeAgentBasedEventList.push_back(fakeEvent);
    fakeAgentBasedEventList.push_back(fakeEvent);
    fakeAgentBasedEventList.push_back(fakeEvent);

    Events fakeEvents {{EventDefinitions::EventCategory::AgentBasedManipulation, fakeAgentBasedEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer resultEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::AgentBasedManipulation);

    ASSERT_THAT(resultEventList, SizeIs(1));
}

TEST(EventNetwork_UnitTests, RemoveOldEventsWithTwoOldEventsOfDifferentCategories)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(2)
                                           .WillOnce(Return(1))
                                           .WillOnce(Return(actualTime));

    EventContainer fakeAgentBasedEventList;
    fakeAgentBasedEventList.push_back(fakeEventFirst);
    fakeAgentBasedEventList.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(2)
                                                 .WillOnce(Return(1))
                                                 .WillOnce(Return(actualTime));

    EventContainer fakeBasicEventList;
    fakeBasicEventList.push_back(fakeEventSecond);
    fakeBasicEventList.push_back(fakeEventSecond);

    Events fakeEvents {{EventDefinitions::EventCategory::AgentBasedManipulation, fakeAgentBasedEventList},
                       {EventDefinitions::EventCategory::Basic, fakeBasicEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer resultAgentBasedEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::AgentBasedManipulation);
    EventContainer resultBasicEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(resultAgentBasedEventList, SizeIs(1));
    ASSERT_THAT(resultBasicEventList, SizeIs(1));
}

TEST(EventNetwork_UnitTests, RemoveOldEventsWithNoOldEvents)
{
    int actualTime = 2;

    auto fakeEventFirst = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventFirst, GetEventTime()).Times(1)
                                                .WillOnce(Return(actualTime));

    EventContainer fakeAgentBasedEventList;
    fakeAgentBasedEventList.push_back(fakeEventFirst);

    auto fakeEventSecond = std::make_shared<NiceMock<FakeEvent>>();
    EXPECT_CALL(*fakeEventSecond, GetEventTime()).Times(1)
                                                 .WillOnce(Return(actualTime));

    EventContainer fakeBasicEventList;
    fakeBasicEventList.push_back(fakeEventSecond);

    Events fakeEvents {{EventDefinitions::EventCategory::AgentBasedManipulation, fakeAgentBasedEventList},
                       {EventDefinitions::EventCategory::Basic, fakeBasicEventList}};

    SimulationSlave::EventNetwork eventNetwork;
    *(eventNetwork.GetArchivedEvents()) = fakeEvents;

    eventNetwork.RemoveOldEvents(actualTime);

    EventContainer resultAgentBasedEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::AgentBasedManipulation);
    EventContainer resultBasicEventList = eventNetwork.GetArchivedEvents()->at(EventDefinitions::EventCategory::Basic);

    ASSERT_THAT(resultAgentBasedEventList, SizeIs(1));
    ASSERT_THAT(resultBasicEventList, SizeIs(1));
}
