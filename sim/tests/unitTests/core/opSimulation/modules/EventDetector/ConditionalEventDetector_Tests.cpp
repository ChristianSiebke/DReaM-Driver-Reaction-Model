/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

//#include "../deps/thirdParty/include/google/protobuf/stubs/callback.h"

#include "dontCare.h"
#include "fakeAgent.h"
#include "fakeCallback.h"
#include "fakeEgoAgent.h"
#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"

#include "core/opSimulation/modelElements/parameters.h"
#include "ConditionalEventDetector.h"

using namespace testing;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::NiceMock;
using ::testing::_;

struct ConditionalTimeTriggerTest_Data
{
    int timeToTriggerEventDetector;
    int numEventsInserted;
};

class ConditionalTimeTriggerTest: public ::TestWithParam<ConditionalTimeTriggerTest_Data>
{
};

TEST_P(ConditionalTimeTriggerTest, TriggerEventInsertion_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;

    const std::vector<std::string> actors {"test"};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    auto testCondition = openScenario::SimulationTimeCondition(openScenario::Rule::GreaterThan,
                                                               1.0);
    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    NiceMock<FakeAgent> mockAgent;
    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName(_)).WillByDefault(Return(&mockAgent));

    FakeEventNetwork mockEventNetwork;

    ConditionalEventDetector eventDetector(&mockWorld,
                                           testConditionalEventDetectorInformation,
                                           &mockEventNetwork,
                                           nullptr,
                                           nullptr);

    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().numEventsInserted);
    eventDetector.Trigger(GetParam().timeToTriggerEventDetector);
}

INSTANTIATE_TEST_CASE_P(ConditionalTimeTrigger_AppropriatelyInsertsEventsIntoNetwork,
                        ConditionalTimeTriggerTest,
                        ::testing::Values(
                            ConditionalTimeTriggerTest_Data{800, 0},
                            ConditionalTimeTriggerTest_Data{1000, 0},
                            ConditionalTimeTriggerTest_Data{1200, 1}
                        ));

// Condition - ByEntity
// Condition - ByEntity - ReachPosition
struct ReachPositionCondition_Data
{
    double tolerance{};
    double sCoordinateOfTargetPosition{};
    std::string roadId{};

    bool errorExpected{};
    size_t numberOfTriggeringAgents{};
    size_t numberOfActingAgents{};
};

class Invocationcontainer
{
public:
    Invocationcontainer() = default;

    void StoreEvent (std::shared_ptr<EventInterface> inputEvent)
    {
        event = std::dynamic_pointer_cast<openpass::events::OpenScenarioEvent>(inputEvent);
    }

    std::shared_ptr<openpass::events::OpenScenarioEvent> event{};
};


class ReachPositionConditionTest: public ::TestWithParam<ReachPositionCondition_Data>
{
};

TEST_P(ReachPositionConditionTest, TriggerEventInsertion_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {"mockAgent1", "mockAgent2"};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{actors};
    bool errorOccurred = false;
    try
    {
        openScenario::RoadPosition roadPosition;
        roadPosition.s = GetParam().sCoordinateOfTargetPosition;
        roadPosition.roadId = GetParam().roadId;

        auto testCondition = openScenario::ReachPositionCondition(testTriggeringEntitites,
                                                                      GetParam().tolerance,
                                                                      roadPosition);
        testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

        NiceMock<FakeAgent> mockAgent1;
        ObjectPosition positionAgent1{{{"fakeRoad", GlobalRoadPosition{"fakeRoad", DontCare<int>(),95.0, DontCare<double>(), DontCare<double>()}}},{},{}};
        ON_CALL(mockAgent1, GetRoads(_)).WillByDefault(Return(std::vector<std::string>{"fakeRoad"}));
        ON_CALL(mockAgent1, GetObjectPosition()).WillByDefault(ReturnRef(positionAgent1));

        NiceMock<FakeAgent> mockAgent2;
        ObjectPosition positionAgent2{{{"fakeRoad", GlobalRoadPosition{"fakeRoad", DontCare<int>(),115.0, DontCare<double>(), DontCare<double>()}}},{},{}};
        ON_CALL(mockAgent2, GetRoads(_)).WillByDefault(Return(std::vector<std::string>{"fakeRoad"}));
        ON_CALL(mockAgent2, GetObjectPosition()).WillByDefault(ReturnRef(positionAgent2));

        NiceMock<FakeWorld> mockWorld;
        ON_CALL(mockWorld, GetAgentByName("mockAgent1")).WillByDefault(Return(&mockAgent1));
        ON_CALL(mockWorld, GetAgentByName("mockAgent2")).WillByDefault(Return(&mockAgent2));

        openpass::type::TriggeringEntities triggering {};

        Invocationcontainer container{};

        FakeEventNetwork mockEventNetwork;
        ON_CALL(mockEventNetwork, InsertEvent(_)).WillByDefault(Invoke(&container, &Invocationcontainer::StoreEvent));


        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);

        if(GetParam().numberOfActingAgents > 0)
        {
            ASSERT_EQ(container.event->GetTriggeringAgents().entities.size(), GetParam().numberOfTriggeringAgents);
            ASSERT_EQ(container.event->GetActingAgents().entities.size(), GetParam().numberOfActingAgents);
        }
        else
        {
            ASSERT_THAT(container.event, nullptr);
        }
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().errorExpected);
}

INSTANTIATE_TEST_CASE_P(ReachPositionCondition_AppropriatelyInsertsEventsIntoNetwork,
                        ReachPositionConditionTest,
                        ::testing::Values(
                            ReachPositionCondition_Data{20.0 , -1.0 , "fakeRoad" , true,  0, 0},
                            ReachPositionCondition_Data{-20.0, 10.0 , "fakeRoad" , true,  0, 0},
                            ReachPositionCondition_Data{20.0 , 100.0, "fakeRoad" , false, 2, 2},
                            ReachPositionCondition_Data{5.0  , 100  , "fakeRoad" , false, 1, 2},
                            ReachPositionCondition_Data{4.0  , 100.0, "fakeRoad" , false, 0, 0},
                            ReachPositionCondition_Data{20.0 , 100.0, "fakeRoad2", false, 0, 0},
                            ReachPositionCondition_Data{0.0  , 100.0, "fakeRoad" , false, 0, 0}
                        ));

// Condition - ByEntity - RelativeSpeed

struct RelativeSpeedCondition_Data
{
    std::string entityName{};
    double range{};
    openScenario::Rule rule{};

    double triggeringAgentVelocity{};
    double triggeringAgentTwoVelocity{};
    double referenceAgentVelocity{};

    bool expectError{};
    int expectNumberOfEvents{};
};

class RelativeSpeedConditionTest: public ::TestWithParam<RelativeSpeedCondition_Data>
{
};

TEST_P(RelativeSpeedConditionTest, TriggerEventInsertion_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{"triggeringAgent1",
                                                           "triggeringAgent2"};
    auto testCondition = openScenario::RelativeSpeedCondition(testTriggeringEntitites,
                                                              GetParam().entityName,
                                                              GetParam().range,
                                                              GetParam().rule);

    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    NiceMock<FakeAgent> triggeringAgent;
    ON_CALL(triggeringAgent, GetVelocity()).WillByDefault(Return(GetParam().triggeringAgentVelocity));

    NiceMock<FakeAgent> triggeringAgentTwo;
    ON_CALL(triggeringAgentTwo, GetVelocity()).WillByDefault(Return(GetParam().triggeringAgentTwoVelocity));

    NiceMock<FakeAgent> referenceAgent;
    ON_CALL(referenceAgent, GetVelocity()).WillByDefault(Return(GetParam().referenceAgentVelocity));

    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName("notExisting")).WillByDefault(Return(nullptr));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent1")).WillByDefault(Return(&triggeringAgent));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent2")).WillByDefault(Return(&triggeringAgentTwo));
    ON_CALL(mockWorld, GetAgentByName("referenceAgent")).WillByDefault(Return(&referenceAgent));

    FakeEventNetwork mockEventNetwork;
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().expectNumberOfEvents);

    bool errorOccurred = false;

    try
    {
        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().expectError);
}

INSTANTIATE_TEST_CASE_P(RelativeSpeedCondition_AppropriatelyInsertsEventsIntoNetwork,
                        RelativeSpeedConditionTest,
                        ::testing::Values(
                            RelativeSpeedCondition_Data{"notExisting"      , DontCare<double>(), DontCare<openScenario::Rule>()   , 20.0, 20.0, DontCare<double>(), true  , 0},
                            RelativeSpeedCondition_Data{"referenceAgent"   , -10.0             , DontCare<openScenario::Rule>()   , 0.0,  0.0,  20.0              , false , 1},
                            RelativeSpeedCondition_Data{"referenceAgent"   , 10.0              , openScenario::Rule::LessThan     , 10.0, 10.0, 19.9              , false , 1},
                            RelativeSpeedCondition_Data{"referenceAgent"   , 10.0              , openScenario::Rule::LessThan     , 20.0, 20.0, 10.0              , false , 0},
                            RelativeSpeedCondition_Data{"referenceAgent"   , 10.0              , openScenario::Rule::EqualTo      , 20.0, 20.0, 10.0              , false , 1},
                            RelativeSpeedCondition_Data{"referenceAgent"   , 10.0              , openScenario::Rule::GreaterThan  , 10.0, 10.0, 20.0              , false , 0},
                            RelativeSpeedCondition_Data{"referenceAgent"   , 10.0              , openScenario::Rule::GreaterThan  , 20.1, 20.1, 10.0              , false , 1}
                        ));

// Condition - ByEntity - RelativeLane

struct RelativeLaneCondition_Data
{
    std::string entityName{};
    int deltaLane{};
    double deltaS{};
    double tolerance{};

    int triggeringAgentLane{};
    double triggeringAgentSCoordinate{};

    int referenceAgentLane{};
    double referenceAgentSCoordinate{};

    bool expectError{};
    int expectNumberOfEvents{};
};

class RelativeLaneConditionTest: public ::TestWithParam<RelativeLaneCondition_Data>
{
};

TEST_P(RelativeLaneConditionTest, TriggerEventInsertion_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{"triggeringAgent"};

    openScenario::RelativeLanePosition relativePosition;
    relativePosition.entityRef = GetParam().entityName;
    relativePosition.dLane = GetParam().deltaLane;
    relativePosition.ds = GetParam().deltaS;
    auto testCondition = openScenario::ReachPositionCondition(testTriggeringEntitites,
                                                             GetParam().tolerance,
                                                              relativePosition);
    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    const std::string roadId = "SomeRoad";
    NiceMock<FakeAgent> triggeringAgent;
    ObjectPosition triggeringAgentPosition{{{"SomeRoad", GlobalRoadPosition{"SomeRoad", GetParam().triggeringAgentLane, GetParam().triggeringAgentSCoordinate, DontCare<double>(), DontCare<double>()}}},{},{}};
    ON_CALL(triggeringAgent, GetObjectPosition()).WillByDefault(ReturnRef(triggeringAgentPosition));
    ON_CALL(triggeringAgent, GetRoads(_)).WillByDefault(Return(std::vector<std::string>{roadId}));

    NiceMock<FakeAgent> referenceAgentOnSameRoad;
    ObjectPosition referenceAgentPosition{{{"SomeRoad", GlobalRoadPosition{"SomeRoad", GetParam().referenceAgentLane, GetParam().referenceAgentSCoordinate, DontCare<double>(), DontCare<double>()}}},{},{}};
    ON_CALL(referenceAgentOnSameRoad, GetObjectPosition()).WillByDefault(ReturnRef(referenceAgentPosition));
    ON_CALL(referenceAgentOnSameRoad, GetRoads(_)).WillByDefault(Return(std::vector<std::string>{roadId}));

    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName("notExisting")).WillByDefault(Return(nullptr));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent")).WillByDefault(Return(&triggeringAgent));
    ON_CALL(mockWorld, GetAgentByName("referenceAgentOnSameRoad")).WillByDefault(Return(&referenceAgentOnSameRoad));

    FakeEventNetwork mockEventNetwork;
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().expectNumberOfEvents);

    bool errorOccurred = false;

    try
    {
        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().expectError);
}

INSTANTIATE_TEST_CASE_P(RelativeLaneCondition_AppropriatelyInsertsEventsIntoNetwork,
                        RelativeLaneConditionTest,
                        ::testing::Values(
                            // -----------------------| entityName               | deltaLane      | deltaS            | tolerance         | triggeringAgentLane | triggeringAgentSCoordinate | referenceAgentLane | referenceAgentSCoordinate | expectError | expectNumberOfEvents
                            RelativeLaneCondition_Data{"notExisting"             , DontCare<int>(), DontCare<double>(), DontCare<double>(), DontCare<int>()     , DontCare<double>()         , DontCare<int>()    , DontCare<double>()        , true        , 0                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , 0                   , 25.0                       , -1                 , 0.0                       , false       , 0                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , -3                  , 25.0                       , -1                 , 0.0                       , false       , 0                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , -2                  , 25.0                       , -1                 , 0.0                       , false       , 1                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , -2                  , 20.0                       , -1                 , 0.0                       , false       , 1                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , -2                  , 40.0                       , -1                 , 0.0                       , false       , 1                    },
                            RelativeLaneCondition_Data{"referenceAgentOnSameRoad", -1             , 30.0              , 10.0              , -2                  , 45.0                       , -1                 , 0.0                       , false       , 0                    }
                        ));

// Condition - ByEntity - TimeToCollision

struct TimeToCollisionCondition_Data
{
    std::string entityName{};
    double targetTTC{};
    openScenario::Rule rule{};

    bool expectError{};
    int expectNumberOfEvents{};
};

class TimeToCollisionConditionTest: public ::TestWithParam<TimeToCollisionCondition_Data>
{
};

TEST_P(TimeToCollisionConditionTest, TriggerEventInsertion_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{"triggeringAgent"};
    auto testCondition = openScenario::TimeToCollisionCondition(testTriggeringEntitites,
                                                                GetParam().entityName,
                                                                GetParam().targetTTC,
                                                                GetParam().rule);
    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    NiceMock<FakeAgent> triggeringAgent;
    ON_CALL(triggeringAgent, GetPositionX()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetYawRate()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetYawAcceleration()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetVelocity(VelocityScope::DirectionX)).WillByDefault(Return(10.0));
    ON_CALL(triggeringAgent, GetVelocity(VelocityScope::DirectionY)).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetAcceleration()).WillByDefault(Return(0.0));
    ON_CALL(triggeringAgent, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(triggeringAgent, GetWidth()).WillByDefault(Return(1.0));
    ON_CALL(triggeringAgent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));

    NiceMock<FakeAgent> referenceAgent;
    ON_CALL(referenceAgent, GetPositionX()).WillByDefault(Return(22.0));
    ON_CALL(referenceAgent, GetPositionY()).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetYaw()).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetYawRate()).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetYawAcceleration()).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetVelocity(VelocityScope::DirectionX)).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetVelocity(VelocityScope::DirectionY)).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetAcceleration()).WillByDefault(Return(0.0));
    ON_CALL(referenceAgent, GetLength()).WillByDefault(Return(2.0));
    ON_CALL(referenceAgent, GetWidth()).WillByDefault(Return(1.0));
    ON_CALL(referenceAgent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(1.0));


    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName("notExisting")).WillByDefault(Return(nullptr));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent")).WillByDefault(Return(&triggeringAgent));
    ON_CALL(mockWorld, GetAgentByName("referenceAgent")).WillByDefault(Return(&referenceAgent));

    FakeEventNetwork mockEventNetwork;
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().expectNumberOfEvents);

    bool errorOccurred = false;

    try
    {
        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().expectError);
}

INSTANTIATE_TEST_CASE_P(TimeToCollisionCondition_AppropriatelyInsertsEventsIntoNetwork,
                        TimeToCollisionConditionTest,
                        ::testing::Values(
                            // ------------------------- | entityName      | targetTTC | rule                              | expectError | expectNumberOfEvents |
                            TimeToCollisionCondition_Data{"notExisting"    , 1.0       , openScenario::Rule::LessThan      , true        , 0                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::GreaterThan   , false       , 1                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::LessThan      , false       , 0                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::EqualTo       , false       , 0                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 2.0       , openScenario::Rule::EqualTo       , false       , 1                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::GreaterThan   , false       , 0                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::LessThan      , false       , 1                    },
                            TimeToCollisionCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::EqualTo       , false       , 0                    }
                        ));

// Condition - ByEntity - TimeHeadway

struct TimeHeadwayCondition_Data
{
    std::string entityName{};
    double targetTHW{};
    openScenario::Rule rule{};

    bool expectError{};
    int expectNumberOfEvents{};
};

class TimeHeadwayConditionTest: public ::TestWithParam<TimeHeadwayCondition_Data>
{
};

TEST_P(TimeHeadwayConditionTest, TriggerEventInsertionFreeSpaceTrue_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{"triggeringAgent"};
    auto testCondition = openScenario::TimeHeadwayCondition(testTriggeringEntitites,
                                                            GetParam().entityName,
                                                            GetParam().targetTHW,
                                                            true,
                                                            GetParam().rule);
    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    NiceMock<FakeAgent> triggeringAgent;
    NiceMock<FakeEgoAgent> triggeringEgoAgent;
    NiceMock<FakeAgent> referenceAgent;

    LongitudinalDistance fakeNetDistance;
    fakeNetDistance.netDistance = 20.0;
    fakeNetDistance.referencePoint = std::numeric_limits<double>::infinity();

    ON_CALL(triggeringAgent, GetEgoAgent()).WillByDefault(ReturnRef(triggeringEgoAgent));
    ON_CALL(triggeringAgent, GetVelocity(VelocityScope::Longitudinal)).WillByDefault(Return(10.0));
    ON_CALL(triggeringEgoAgent, GetDistanceToObject(&referenceAgent)).WillByDefault(Return(fakeNetDistance));

    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName("notExisting")).WillByDefault(Return(nullptr));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent")).WillByDefault(Return(&triggeringAgent));
    ON_CALL(mockWorld, GetAgentByName("referenceAgent")).WillByDefault(Return(&referenceAgent));

    FakeEventNetwork mockEventNetwork;
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().expectNumberOfEvents);

    bool errorOccurred = false;

    try
    {
        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().expectError);
}

TEST_P(TimeHeadwayConditionTest, TriggerEventInsertionFreeSpaceFalse_AddsEventIfNecessary)
{
    openScenario::ConditionalEventDetectorInformation testConditionalEventDetectorInformation;
    testConditionalEventDetectorInformation.numberOfExecutions = -1;
    testConditionalEventDetectorInformation.actorInformation.actorIsTriggeringEntity = true;

    const std::vector<std::string> actors {};
    testConditionalEventDetectorInformation.actorInformation.actors.emplace(actors);

    const std::vector<std::string> testTriggeringEntitites{"triggeringAgent"};
    auto testCondition = openScenario::TimeHeadwayCondition(testTriggeringEntitites,
                                                            GetParam().entityName,
                                                            GetParam().targetTHW,
                                                            false,
                                                            GetParam().rule);
    testConditionalEventDetectorInformation.conditions.emplace_back(testCondition);

    NiceMock<FakeAgent> triggeringAgent;
    NiceMock<FakeEgoAgent> triggeringEgoAgent;
    NiceMock<FakeAgent> referenceAgent;

    LongitudinalDistance fakeNetDistance;
    fakeNetDistance.netDistance = std::numeric_limits<double>::infinity();
    fakeNetDistance.referencePoint = 20.0;

    ON_CALL(triggeringAgent, GetEgoAgent()).WillByDefault(ReturnRef(triggeringEgoAgent));
    ON_CALL(triggeringAgent, GetVelocity(VelocityScope::Longitudinal)).WillByDefault(Return(10.0));
    ON_CALL(triggeringEgoAgent, GetDistanceToObject(&referenceAgent)).WillByDefault(Return(fakeNetDistance));

    NiceMock<FakeWorld> mockWorld;
    ON_CALL(mockWorld, GetAgentByName("notExisting")).WillByDefault(Return(nullptr));
    ON_CALL(mockWorld, GetAgentByName("triggeringAgent")).WillByDefault(Return(&triggeringAgent));
    ON_CALL(mockWorld, GetAgentByName("referenceAgent")).WillByDefault(Return(&referenceAgent));

    FakeEventNetwork mockEventNetwork;
    EXPECT_CALL(mockEventNetwork, InsertEvent(_)).Times(GetParam().expectNumberOfEvents);

    bool errorOccurred = false;

    try
    {
        ConditionalEventDetector eventDetector(&mockWorld,
                                               testConditionalEventDetectorInformation,
                                               &mockEventNetwork,
                                               nullptr,
                                               nullptr);

        eventDetector.Trigger(0);
    }
    catch (...)
    {
        errorOccurred = true;
    }

    ASSERT_EQ(errorOccurred, GetParam().expectError);
}

INSTANTIATE_TEST_CASE_P(TimeHeadwayCondition_AppropriatelyInsertsEventsIntoNetwork,
                        TimeHeadwayConditionTest,
                        ::testing::Values(
                            // --------------------- | entityName      | targetTHW | rule                              | expectError | expectNumberOfEvents |
                            TimeHeadwayCondition_Data{"notExisting"    , 1.0       , openScenario::Rule::LessThan      , true        , 0                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::GreaterThan   , false       , 1                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::LessThan      , false       , 0                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 1.0       , openScenario::Rule::EqualTo       , false       , 0                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 2.0       , openScenario::Rule::EqualTo       , false       , 1                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::GreaterThan   , false       , 0                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::LessThan      , false       , 1                    },
                            TimeHeadwayCondition_Data{"referenceAgent" , 3.0       , openScenario::Rule::EqualTo       , false       , 0                    }
                        ));
