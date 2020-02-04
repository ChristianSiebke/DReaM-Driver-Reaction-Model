/**********************************************
* Copyright (c) 2017 in-tech GmbH
* on behalf of BMW AG
***********************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"
#include "fakeWorld.h"
#include "fakeComponent.h"
#include "fakeAgentBlueprint.h"

#include "schedulerTasks.h"
#include "agentParser.h"
#include "Interfaces/componentInterface.h"
#include "channel.h"
#include "component.h"

#include <map>

using ::testing::_;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::Field;
using ::testing::IsEmpty;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

using namespace SimulationSlave;
using namespace SimulationSlave::Scheduling;

//class FakeTaskProvider : public TaskProviderInterface
//{
//public:
//    MOCK_CONST_METHOD1(GetTrigger, std::function<void(int)>(std::string));
//    MOCK_CONST_METHOD1(GetUpdateOutput, std::function<void(int)>(std::string));
//    MOCK_CONST_METHOD1(GetUpdateInput, std::function<void(int)>(std::string));
//};


TEST(AgentParser, RecurringComponent_IsParsed)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    Agent testAgent(0, &fakeAgentBlueprint, 0, &fakeWorld);

    Channel testChannel(1);
    Component testTargetComponent("", &testAgent);
    testChannel.AddTarget(&testTargetComponent, 0);
    std::map<int, Channel*> testChannels = {{0, &testChannel}};

    auto fakeComponent = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetCycleTime()).WillByDefault(Return(100));
    ON_CALL(*fakeComponent, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetInit()).WillByDefault(Return(false));
    ON_CALL(*fakeComponent, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    ON_CALL(*fakeComponent, ReleaseFromLibrary()).WillByDefault(Return(true));
    testAgent.AddComponent("Compontent1", fakeComponent);

    int currentTime = 0;
    AgentParser agentParser(currentTime);
    agentParser.Parse(testAgent);

    auto nonRecurringTasks = agentParser.GetNonRecurringTasks();
    EXPECT_THAT(nonRecurringTasks, IsEmpty());

    std::list<TaskItem> recurringTasks = agentParser.GetRecurringTasks();
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::cycletime, Eq(100)))) << "cycletime";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Trigger)))) << "taskType Trigger";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Update)))) << "taskType Update";

    EXPECT_EQ(recurringTasks.size(), 3);
    EXPECT_EQ(recurringTasks.front().taskType, TaskType::Trigger);
}

TEST(AgentParser, ThreeRecurringComponents_AreParsed)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    Agent testAgent(0, &fakeAgentBlueprint, 0, &fakeWorld);

    Channel testChannel(1);
    Component testTargetComponent("", &testAgent);
    testChannel.AddTarget(&testTargetComponent, 0);
    std::map<int, Channel*> testChannels = {{0, &testChannel}};

    auto fakeComponent = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent, GetCycleTime()).WillByDefault(Return(100));
    ON_CALL(*fakeComponent, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetInit()).WillByDefault(Return(false));
    ON_CALL(*fakeComponent, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent1", fakeComponent);

    auto fakeComponent2 = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent2, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetCycleTime()).WillByDefault(Return(50));
    ON_CALL(*fakeComponent2, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetInit()).WillByDefault(Return(false));
    ON_CALL(*fakeComponent2, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent2", fakeComponent2);

    auto fakeComponent3 = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent3, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent3, GetCycleTime()).WillByDefault(Return(250));
    ON_CALL(*fakeComponent3, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent3, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent3, GetInit()).WillByDefault(Return(false));
    ON_CALL(*fakeComponent3, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent3", fakeComponent3);

    int currentTime = 0;
    AgentParser agentParser(currentTime);
    agentParser.Parse(testAgent);

    auto nonRecurringTasks = agentParser.GetNonRecurringTasks();
    EXPECT_THAT(nonRecurringTasks, IsEmpty());

    auto recurringTasks = agentParser.GetRecurringTasks();

    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::cycletime, Eq(50)))) << "cycletime";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::cycletime, Eq(100)))) << "cycletime";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::cycletime, Eq(250)))) << "cycletime";

    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Trigger)))) << "taskType Trigger";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Update)))) << "taskType Update";
}

TEST(AgentParser, NonRecurringComponent_IsParsed)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    Agent testAgent(0, &fakeAgentBlueprint, 0, &fakeWorld);

    Channel testChannel(1);
    Component testTargetComponent("", &testAgent);
    testChannel.AddTarget(&testTargetComponent, 0);
    std::map<int, Channel*> testChannels = {{0, &testChannel}};

    auto fakeComponent = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetCycleTime()).WillByDefault(Return(100));
    ON_CALL(*fakeComponent, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetInit()).WillByDefault(Return(true));
    ON_CALL(*fakeComponent, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent1", fakeComponent);

    int currentTime = 0;
    AgentParser agentParser(currentTime);
    agentParser.Parse(testAgent);

    auto recurringTasks = agentParser.GetRecurringTasks();
    EXPECT_THAT(recurringTasks, IsEmpty());

    std::list<TaskItem> nonRecurringTasks = agentParser.GetNonRecurringTasks();
    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::cycletime, Eq(100)))) << "cycletime";
    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Trigger)))) << "taskType Trigger";
    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Update)))) << "taskType Update";

    ASSERT_EQ(nonRecurringTasks.size(), 3);
    ASSERT_EQ(nonRecurringTasks.front().taskType, TaskType::Trigger);
}

TEST(AgentParser, MixedComponents_AreParsedWithRightTaskType)
{
    NiceMock<FakeWorld> fakeWorld;
    NiceMock<FakeAgentBlueprint> fakeAgentBlueprint;
    Agent testAgent(0, &fakeAgentBlueprint, 0, &fakeWorld);

    Channel testChannel(1);
    Component testTargetComponent("", &testAgent);
    testChannel.AddTarget(&testTargetComponent, 0);
    std::map<int, Channel*> testChannels = {{0, &testChannel}};

    auto fakeComponent = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent, GetCycleTime()).WillByDefault(Return(100));
    ON_CALL(*fakeComponent, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent, GetInit()).WillByDefault(Return(false));
    ON_CALL(*fakeComponent, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent1", fakeComponent);

    auto fakeComponent2 = new NiceMock<FakeComponent>();
    ON_CALL(*fakeComponent2, GetPriority()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetCycleTime()).WillByDefault(Return(50));
    ON_CALL(*fakeComponent2, GetOffsetTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetResponseTime()).WillByDefault(Return(0));
    ON_CALL(*fakeComponent2, GetInit()).WillByDefault(Return(true));
    ON_CALL(*fakeComponent2, GetOutputLinks()).WillByDefault(ReturnRef(testChannels));
    testAgent.AddComponent("Compontent2", fakeComponent2);

    int currentTime = 0;
    AgentParser agentParser(currentTime);
    agentParser.Parse(testAgent);

    auto recurringTasks = agentParser.GetRecurringTasks();
    auto nonRecurringTasks = agentParser.GetNonRecurringTasks();

    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::cycletime, Eq(100)))) << "cycletime";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Trigger)))) << "taskType Trigger";
    EXPECT_THAT(recurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Update)))) << "taskType Update";

    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::cycletime, Eq(50)))) << "cycletime";
    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Trigger)))) << "taskType Trigger";
    EXPECT_THAT(nonRecurringTasks, Contains(Field(&TaskItem::taskType, Eq(TaskType::Update)))) << "taskType Update";
}
