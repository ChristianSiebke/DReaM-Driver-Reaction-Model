#define OPENPASS_TESTING_ON

#include <functional>
#include <iostream>
#include <list>
#include <set>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "schedulerTasks.h"

using ::testing::_;
using ::testing::ContainerEq;
using ::testing::Contains;
using ::testing::Each;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Field;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SizeIs;

using namespace openpass::scheduling;

int currentTimestamp = 0;

bool TriggerFunc(int time)
{
    std::cout << "triggering task at " << time << std::endl;
    return true;
}

bool UpdateFunc(int id, int time)
{
    std::cout << "updating task at " << time << " for id: " << id << std::endl;
    return true;
}

TEST(Tasks_Test, AddedTask_FillTasksCorrect)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 0, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 50, 0, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    UpdateTaskItem thirdTaskItem{0, 0, 100, 0, updateFunc};
    UpdateTaskItem fourthTaskItem{0, 0, 0, 0, updateFunc};
    UpdateTaskItem fifthTaskItem{0, 10, 100, 0, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};

    Tasks testTasks;
    testTasks.AddTask(firstTaskItem);
    testTasks.AddTask(secondTaskItem);
    testTasks.AddTask(thirdTaskItem);
    testTasks.AddTask(fourthTaskItem);
    testTasks.AddTask(fifthTaskItem);
    testTasks.AddTask(sixthTaskItem);

    std::multiset<TaskItem> tasks = testTasks.tasks;

    ASSERT_THAT(tasks, SizeIs(6));
    ASSERT_THAT(tasks, ElementsAre(secondTaskItem, fifthTaskItem, firstTaskItem, thirdTaskItem, fourthTaskItem, sixthTaskItem));
}

TEST(Tasks_Test, DeletedTasks_FilterTasksCorrect)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 0, 0, triggerFunc};
    TriggerTaskItem secondTaskItem{1, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{1, 0, 50, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{2, 10, 0, 0, triggerFunc};
    TriggerTaskItem fifthTaskItem{3, 0, 100, 0, triggerFunc};
    TriggerTaskItem sixthTaskItem{2, 0, 100, 10, triggerFunc};

    Tasks testTasks;
    testTasks.AddTask(firstTaskItem);
    testTasks.AddTask(secondTaskItem);
    testTasks.AddTask(thirdTaskItem);
    testTasks.AddTask(fourthTaskItem);
    testTasks.AddTask(fifthTaskItem);
    testTasks.AddTask(sixthTaskItem);

    std::multiset<TaskItem> tasks = testTasks.tasks;
    ASSERT_THAT(tasks, SizeIs(6));

    testTasks.DeleteTasks(2);

    tasks = testTasks.tasks;
    ASSERT_THAT(tasks, SizeIs(4));
    ASSERT_EQ((*tasks.begin()), secondTaskItem);
    ASSERT_THAT(tasks, Contains(firstTaskItem));
    ASSERT_THAT(tasks, Contains(thirdTaskItem));
    ASSERT_THAT(tasks, Contains(fifthTaskItem));
}

TEST(Tasks_Test, DeletedLastTasks_FilterTasksCorrect)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 0, 0, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    Tasks testTasks;
    testTasks.AddTask(firstTaskItem);
    testTasks.AddTask(secondTaskItem);

    std::multiset<TaskItem> tasks = testTasks.tasks;

    ASSERT_THAT(tasks, SizeIs(2));
    testTasks.DeleteTasks(0);
    tasks = testTasks.tasks;
    ASSERT_TRUE(tasks.empty());
}

TEST(SchedulerTasks_Test, ScheduleComponentTasks_UpdateScheduledTimestamps)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 25, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::set<int> scheduledTimestamps = testSchedulerTasks.scheduledTimestamps;
    std::set<int> referenceTimestamps = {0, 10, 25, 50, 75, 100, 110, 125, 150, 175, 200};

    ASSERT_THAT(scheduledTimestamps, ContainerEq(referenceTimestamps));
}

TEST(SchedulerTasks_Test, ScheduleComponentTasksWithLargerValueThanUpperBound_UpdateScheduledTimestampsUntilHorizon)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 25, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 10, 250, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::set<int> scheduledTimestamps = testSchedulerTasks.scheduledTimestamps;
    std::set<int> referenceTimestamps = {0, 10, 25, 50, 75, 100, 110, 125, 150, 175, 200};

    ASSERT_THAT(scheduledTimestamps, ContainerEq(referenceTimestamps));
}

TEST(SchedulerTasks_Test, ScheduleComponentTasksWithLargerDelayThanBound_UpdateScheduledTimestampsUntilBound)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 50, 300, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::set<int> scheduledTimestamps = testSchedulerTasks.scheduledTimestamps;
    std::set<int> referenceTimestamps = {0, 10, 100, 110, 200};

    ASSERT_THAT(scheduledTimestamps, ContainerEq(referenceTimestamps));
}

TEST(SchedulerTasks_Test, ScheduleComponentTasksWithMultipleLargerHorizons_UpdateScheduledTimestampsUntilBound)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 250, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 10, 250, 10, triggerFunc};
    TriggerTaskItem fifthTaskItem{0, 10, 300, 0, triggerFunc};
    TriggerTaskItem sixthTaskItem{0, 10, 400, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::set<int> scheduledTimestamps = testSchedulerTasks.scheduledTimestamps;
    std::set<int> referenceTimestamps = {0, 10, 100, 110, 200};

    ASSERT_THAT(scheduledTimestamps, ContainerEq(referenceTimestamps));
}

TEST(SchedulerTasks_Test, ScheduleComponentTasks_FillNonRecurringTasksCorrect)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 0, 0, 0, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasks);

    std::multiset<TaskItem> nonRecurringTasks = testSchedulerTasks.nonRecurringAgentTasks.tasks;

    ASSERT_THAT(nonRecurringTasks, SizeIs(2));
    ASSERT_THAT(nonRecurringTasks, ElementsAre(secondTaskItem, firstTaskItem));
}

TEST(SchedulerTasks_Test, DeleteAgentTasks_FilterAllAffectedTasks)
{
    int removeAgent = 1;
    int normalAgent = 0;
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{normalAgent, 0, 0, 0, updateFunc};
    TriggerTaskItem secondTaskItem{removeAgent, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasksNonRecurring{firstTaskItem, secondTaskItem};

    TriggerTaskItem thirdTaskItem{removeAgent, 10, 100, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{removeAgent, 0, 50, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{normalAgent, 0, 100, 0, updateFunc};
    UpdateTaskItem sixthTaskItem{normalAgent, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasksRecurring{thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    ManipulatorTaskItem firstCommonTaskItem{100, triggerFunc};
    EventDetectorTaskItem secondCommonTaskItem{100, triggerFunc};
    SpawningTaskItem thirdCommonTaskItem{100, triggerFunc};
    ObservationTaskItem fourthCommonTaskItem{100, triggerFunc};
    std::vector<TaskItem> testCommonTasks{firstCommonTaskItem, secondCommonTaskItem, thirdCommonTaskItem, fourthCommonTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, testCommonTasks, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasksRecurring);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasksNonRecurring);

    std::vector<int> removeAgents = {removeAgent};
    testSchedulerTasks.DeleteAgentTasks(removeAgents);

    std::multiset<TaskItem> recurringTasks = testSchedulerTasks.recurringAgentTasks.tasks;
    auto recurringIt = std::find_if(recurringTasks.begin(), recurringTasks.end(),
                                    [removeAgent](const TaskItem &TaskItem) {
                                        return removeAgent == TaskItem.agentId;
                                    });
    ASSERT_TRUE(recurringIt == recurringTasks.end());

    std::multiset<TaskItem> nonRecurringTasks = testSchedulerTasks.nonRecurringAgentTasks.tasks;
    auto nonRecurringIt = std::find_if(nonRecurringTasks.begin(), nonRecurringTasks.end(),
                                       [removeAgent](const TaskItem &TaskItem) {
                                           return removeAgent == TaskItem.agentId;
                                       });
    ASSERT_TRUE(nonRecurringIt == nonRecurringTasks.end());

    std::multiset<TaskItem> spawningTasks = testSchedulerTasks.spawningTasks.tasks;
    auto commonIt = std::find_if(spawningTasks.begin(), spawningTasks.end(),
                                 [removeAgent](const TaskItem &TaskItem) {
                                     return removeAgent == TaskItem.agentId;
                                 });
    ASSERT_TRUE(commonIt == spawningTasks.end());
}

TEST(SchedulerTasks_Test, DeleteAgentTasks_CreateNewScheduledTimestamps)
{
    int removeAgent = 1;
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 100, 0, updateFunc};
    TriggerTaskItem secondTaskItem{removeAgent, 10, 25, 0, triggerFunc};
    UpdateTaskItem thirdTaskItem{0, 0, 100, 0, updateFunc};
    TriggerTaskItem fourthTaskItem{removeAgent, 10, 100, 10, triggerFunc};
    UpdateTaskItem fifthtTaskItem{0, 0, 50, 0, updateFunc};
    TriggerTaskItem sixthTaskItem{removeAgent, 10, 10, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem, fifthtTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);
    std::vector<int> removeAgents = {removeAgent};
    testSchedulerTasks.DeleteAgentTasks(removeAgents);

    std::set<int> scheduledTimestamps = testSchedulerTasks.scheduledTimestamps;
    std::set<int> referenceTimestamps = {0, 50, 100, 150, 200};

    ASSERT_THAT(scheduledTimestamps, ContainerEq(referenceTimestamps));
}

TEST(SchedulerTasks_Test, ScheduleComponentTasks_FillRecurringTasksCorrect)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    TriggerTaskItem firstTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem secondTaskItem{0, 0, 50, 0, triggerFunc};
    UpdateTaskItem thirdTaskItem{0, 0, 100, 0, updateFunc};
    UpdateTaskItem fourthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::multiset<TaskItem> recurringTasks = testSchedulerTasks.recurringAgentTasks.tasks;

    ASSERT_THAT(recurringTasks, SizeIs(4));
    ASSERT_EQ((*recurringTasks.begin()), firstTaskItem);
    ASSERT_THAT(recurringTasks, Contains(secondTaskItem));
    ASSERT_THAT(recurringTasks, Contains(thirdTaskItem));
    ASSERT_THAT(recurringTasks, Contains(fourthTaskItem));
}

TEST(SchedulerTasks_Test, GetNextTimestamp_MakesCorrectStep)
{
    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.scheduledTimestamps = {0, 10, 20, 50, 100, 110, 120, 150, 200};

    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(0), 10);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(10), 20);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(150), 200);
}

TEST(SchedulerTasks_Test, GetNextTimestamp_CreateNewScheduledTimestamps)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 50, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 10, 50, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    // first scheduledTimestamps has to be {0, 10, 50, 100, 110, 150, 200};

    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(200), 210);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(210), 250);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(250), 300);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(300), 310);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(350), 400);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(410), 450);
}

TEST(SchedulerTasks_Test, GetNextTimestampWithUnscheduledTimestamp_MakesCorrectStep)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 50, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 100, 10, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 10, 50, 0, triggerFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    // first scheduledTimestamps has to be {0, 10, 50, 100, 110, 150, 200};

    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(5), 10);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(60), 100);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(120), 150);
    ASSERT_EQ(testSchedulerTasks.GetNextTimestamp(90), 100);
}

TEST(SchedulerTasks_Test, GetAllCurrentTasks_DeliversFilteredTasks)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 0, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasksNonRecurring{firstTaskItem, secondTaskItem};

    TriggerTaskItem thirdTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 0, 25, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{0, 0, 100, 20, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasksRecurring{thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasksRecurring);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasksNonRecurring);

    std::vector<TaskItem> currentTasks = testSchedulerTasks.GetTasks(0);
    ASSERT_THAT(currentTasks, SizeIs(4));
    ASSERT_THAT(currentTasks, Each(Field(&TaskItem::delay, Eq(0))));
}

TEST(SchedulerTasks_Test, GetAllCurrentTasks_DeliversFilteredTasksOnSecondTimestep)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 50, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem thirdTaskItem{0, 0, 25, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 10, 50, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{0, 0, 100, 20, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasks{firstTaskItem, secondTaskItem, thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasks);

    std::vector<TaskItem> currentTasks = testSchedulerTasks.GetTasks(10);
    ASSERT_THAT(currentTasks, SizeIs(1));
    ASSERT_THAT(currentTasks, Each(Field(&TaskItem::delay, Eq(10))));
}

TEST(SchedulerTasks_Test, GetAllCurrentTasksWithUnscheduledTimestamp_DeliversEmptyComponentTasks)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    int unscheduledTimestamp = 1;

    UpdateTaskItem firstTaskItem{0, 0, 0, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasksNonRecurring{firstTaskItem, secondTaskItem};

    TriggerTaskItem thirdTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 0, 25, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{0, 0, 100, 20, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasksRecurring{thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasksRecurring);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasksNonRecurring);

    std::vector<TaskItem> currentTasks = testSchedulerTasks.GetTasks(unscheduledTimestamp);
    ASSERT_THAT(currentTasks, SizeIs(0));
}

TEST(SchedulerTasks_Test, GetAllCurrentTasksWithinFirstBoundaries_DeliversFilteredTasks)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 0, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasksNonRecurring{firstTaskItem, secondTaskItem};

    TriggerTaskItem thirdTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 0, 25, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{0, 0, 100, 20, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasksRecurring{thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasksRecurring);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasksNonRecurring);

    std::vector<TaskItem> currentTasks = testSchedulerTasks.GetTasks(0);
    ASSERT_THAT(currentTasks, SizeIs(4));
    ASSERT_THAT(currentTasks, Contains(firstTaskItem));
    ASSERT_THAT(currentTasks, Contains(secondTaskItem));
    ASSERT_THAT(currentTasks, Contains(thirdTaskItem));
    ASSERT_THAT(currentTasks, Contains(fourthTaskItem));

    currentTasks = testSchedulerTasks.GetTasks(200);
    ASSERT_THAT(currentTasks, SizeIs(2));
    ASSERT_THAT(currentTasks, Contains(thirdTaskItem));
    ASSERT_THAT(currentTasks, Contains(fourthTaskItem));
}

TEST(SchedulerTasks_Test, GetAllCurrentTasksWithTimestampHigherBound_DeliversFilteredTasks)
{
    std::function<bool(void)> triggerFunc = std::bind(&TriggerFunc, std::ref(currentTimestamp));
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem firstTaskItem{0, 0, 0, 0, updateFunc};
    TriggerTaskItem secondTaskItem{0, 10, 0, 0, triggerFunc};
    std::vector<TaskItem> testTasksNonRecurring{firstTaskItem, secondTaskItem};

    TriggerTaskItem thirdTaskItem{0, 10, 100, 0, triggerFunc};
    TriggerTaskItem fourthTaskItem{0, 0, 25, 0, triggerFunc};
    UpdateTaskItem fifthTaskItem{0, 0, 100, 20, updateFunc};
    UpdateTaskItem sixthTaskItem{0, 0, 100, 10, updateFunc};
    std::vector<TaskItem> testTasksRecurring{thirdTaskItem, fourthTaskItem, fifthTaskItem, sixthTaskItem};

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(testTasksRecurring);
    testSchedulerTasks.ScheduleNewNonRecurringTasks(testTasksNonRecurring);

    std::vector<TaskItem> currentTasks = testSchedulerTasks.GetTasks(0); // to pull init tasks
    currentTasks = testSchedulerTasks.GetTasks(300);
    ASSERT_THAT(currentTasks, SizeIs(2));
    ASSERT_THAT(currentTasks, Contains(thirdTaskItem));
    ASSERT_THAT(currentTasks, Contains(fourthTaskItem));
}

TEST(SchedulerTasks_Test, UpdateTasks_ListedInCorrectOrder)
{
    std::function<bool(void)> updateFunc = std::bind(&UpdateFunc, 42, std::ref(currentTimestamp));

    UpdateTaskItem outputTaskItem{0, 0, 0, 0, updateFunc};
    UpdateTaskItem inputTaskItem{1, 0, 0, 0, updateFunc};

    std::vector<TaskItem> taskItems;
    taskItems.push_back(outputTaskItem);
    taskItems.push_back(inputTaskItem); // like in AgentParser

    SchedulerTasks testSchedulerTasks(std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, std::vector<TaskItem>{}, 200);
    testSchedulerTasks.ScheduleNewRecurringTasks(taskItems);

    taskItems = testSchedulerTasks.GetTasks(0);

    ASSERT_THAT(taskItems, ElementsAre(outputTaskItem, inputTaskItem));
}
