/*******************************************************************************
* Copyright (c) 2018 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "observationCyclics.h"
#include "runStatisticCalculation.h"

#include "fakeAgent.h"
#include "fakeWorld.h"
#include "fakeRunResult.h"

using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

TEST(ObservationCyclics_Test, GetHeader_ReturnsCorrectHeader)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(100, "ParameterA", "345");
    cyclics.Insert(100, "ParameterB", "456");

    std::string header = cyclics.GetHeader();
    ASSERT_THAT(header, Eq("ParameterA, ParameterB, ParameterC"));
}

TEST(ObservationCyclics_Test, GetTimeSteps_ReturnsCorrectTimesteps)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(100, "ParameterA", "345");
    cyclics.Insert(150, "ParameterB", "456");

    std::vector<int> timesteps = *cyclics.GetTimeSteps();
    ASSERT_THAT(timesteps, ElementsAre(0, 100, 150));
}

TEST(ObservationCyclics_Test, GetSamplesLineAllSamplesExisting_ReturnsCorrectLine)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(100, "ParameterC", "567");
    cyclics.Insert(100, "ParameterB", "678");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, 345, 234"));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, 678, 567"));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesAddedAfter_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(100, "ParameterA", "234");
    cyclics.Insert(100, "ParameterC", "345");
    cyclics.Insert(200, "ParameterA", "456");
    cyclics.Insert(200, "ParameterC", "567");
    cyclics.Insert(200, "ParameterB", "678");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, , "));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("234, , 345"));
    samplesLine = cyclics.GetSamplesLine(2);
    ASSERT_THAT(samplesLine, Eq("456, 678, 567"));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesMissingInBetween_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(200, "ParameterA", "567");
    cyclics.Insert(200, "ParameterC", "678");
    cyclics.Insert(200, "ParameterB", "789");

    std::string samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, , "));
}

TEST(ObservationCyclics_Test, GetSamplesLineSamplesNotUntilEnd_ReturnsLineWithEmptyString)
{
    ObservationCyclics cyclics;
    cyclics.Insert(0, "ParameterA", "123");
    cyclics.Insert(0, "ParameterC", "234");
    cyclics.Insert(0, "ParameterB", "345");
    cyclics.Insert(100, "ParameterA", "456");
    cyclics.Insert(100, "ParameterB", "678");
    cyclics.Insert(200, "ParameterB", "789");

    std::string samplesLine = cyclics.GetSamplesLine(0);
    ASSERT_THAT(samplesLine, Eq("123, 345, 234"));
    samplesLine = cyclics.GetSamplesLine(1);
    ASSERT_THAT(samplesLine, Eq("456, 678, "));
    samplesLine = cyclics.GetSamplesLine(2);
    ASSERT_THAT(samplesLine, Eq(", 789, "));
}

TEST(RunStatisticCalculation_Test, CalculateTotalDistanceTraveled_SetsCorrectDistances)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetDistanceTraveled()).WillByDefault(Return(100.0));
    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeAgent> fakeAgent1;
    ON_CALL(fakeAgent1, GetDistanceTraveled()).WillByDefault(Return(50.0));
    ON_CALL(fakeAgent1, IsEgoAgent()).WillByDefault(Return(false));

    NiceMock<FakeAgent> fakeAgent2;
    ON_CALL(fakeAgent2, GetDistanceTraveled()).WillByDefault(Return(8.0));
    ON_CALL(fakeAgent2, IsEgoAgent()).WillByDefault(Return(false));

    NiceMock<FakeAgent> fakeAgent3;
    ON_CALL(fakeAgent3, GetDistanceTraveled()).WillByDefault(Return(0.3));
    ON_CALL(fakeAgent3, IsEgoAgent()).WillByDefault(Return(false));

    std::map<int, AgentInterface*> agents{ {0, &fakeEgo}, {1, &fakeAgent1}, {2, &fakeAgent2} };
    std::list<const AgentInterface*> removedAgents{&fakeAgent3};
    ON_CALL(fakeWorld, GetAgents()).WillByDefault(ReturnRef(agents));
    ON_CALL(fakeWorld, GetRemovedAgents()).WillByDefault(ReturnRef(removedAgents));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::CalculateTotalDistanceTraveled(runStatistic, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoDistanceTraveled, DoubleEq(100.0));
    ASSERT_THAT(runStatistic.TotalDistanceTraveled, DoubleEq(158.3));
}

TEST(RunStatisticCalculation_Test, CalculateNumberOfCollisionsEgoAccident)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::list<int> collisionIds{ {0} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::CalculateNumberOfCollisions(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(true));
    ASSERT_THAT(runStatistic.NCollisionsFollowers, Eq(0));
    ASSERT_THAT(runStatistic.NCollisionsArbitrary, Eq(0));
}

TEST(RunStatisticCalculation_Test, CalculateNumberOfCollisionsFollowerAndOtherAccidents)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::list<int> collisionIds{ {1, 2, 3} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};
    runStatistic.GetFollowerIds()->push_back(1);
    runStatistic.GetFollowerIds()->push_back(2);

    RunStatisticCalculation::CalculateNumberOfCollisions(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(false));
    ASSERT_THAT(runStatistic.NCollisionsFollowers, Eq(2));
    ASSERT_THAT(runStatistic.NCollisionsArbitrary, Eq(1));
}
