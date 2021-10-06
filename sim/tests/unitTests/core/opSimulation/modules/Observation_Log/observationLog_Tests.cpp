/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

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

    const auto& timesteps = cyclics.GetTimeSteps();
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

TEST(RunStatisticCalculation_Test, DetermineEgoCollisionWithEgoCollision_SetsEgoCollisionTrue)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::vector<int> collisionIds{ {0, 1} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(true));
}

TEST(RunStatisticCalculation_Test, DetermineEgoCollisionWithoutEgoCollision_SetsEgoCollisionFalse)
{
    NiceMock<FakeWorld> fakeWorld;

    NiceMock<FakeAgent> fakeEgo;
    ON_CALL(fakeEgo, GetId()).WillByDefault(Return(0));

    ON_CALL(fakeWorld, GetEgoAgent()).WillByDefault(Return(&fakeEgo));

    NiceMock<FakeRunResult> runResult;

    std::vector<int> collisionIds{ {1, 2, 3} };
    ON_CALL(runResult, GetCollisionIds()).WillByDefault(Return(&collisionIds));

    RunStatistic runStatistic{0};

    RunStatisticCalculation::DetermineEgoCollision(runStatistic, runResult, &fakeWorld);

    ASSERT_THAT(runStatistic.EgoCollision, Eq(false));
}
