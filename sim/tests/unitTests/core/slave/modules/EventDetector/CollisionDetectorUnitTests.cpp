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

#include "CollisionDetector.h"

#include "fakeWorld.h"
#include "fakeAgent.h"
#include "fakeWorldObject.h"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

TEST(CollisionDetector_UnitTests, TestCalculateAgentGeometry)
{
    std::map<int, AgentInterface*> stubAgents;
    std::vector<const TrafficObjectInterface*> stubObjects;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetAgents()).WillByDefault(ReturnRef(stubAgents));
    ON_CALL(fakeWorld, GetTrafficObjects()).WillByDefault(ReturnRef(stubObjects));

    NiceMock<FakeWorldObject> fakeWorldObject;
    ON_CALL(fakeWorldObject, GetLength()).WillByDefault(Return(4.0));
    ON_CALL(fakeWorldObject, GetWidth()).WillByDefault(Return(2.0));
    ON_CALL(fakeWorldObject, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
    ON_CALL(fakeWorldObject, GetYaw()).WillByDefault(Return(0.0));

    const Common::Vector2d position {0.0, 0.0};

    std::array<Common::Vector2d, 4> resultCorners;
    std::array<Common::Vector2d, 2> resultNormals;

    CollisionDetector detector{&fakeWorld, nullptr, nullptr, nullptr};
    detector.CalculateWorldObjectGeometry(&fakeWorldObject, position, resultCorners, resultNormals);

    ASSERT_EQ(resultCorners[UpperLeft].x, -1.0);
    ASSERT_EQ(resultCorners[UpperLeft].y, 1.0);

    ASSERT_EQ(resultCorners[UpperRight].x, 3.0);
    ASSERT_EQ(resultCorners[UpperRight].y, 1.0);

    ASSERT_EQ(resultCorners[LowerRight].x, 3.0);
    ASSERT_EQ(resultCorners[LowerRight].y, -1.0);

    ASSERT_EQ(resultCorners[LowerLeft].x, -1.0);
    ASSERT_EQ(resultCorners[LowerLeft].y, -1.0);

    ASSERT_EQ(resultNormals[0].x, 4.0);
    ASSERT_EQ(resultNormals[0].y, 0.0);

    ASSERT_EQ(resultNormals[1].x, 0.0);
    ASSERT_EQ(resultNormals[1].y, 2.0);
}

TEST(CollisionDetector_UnitTests, TestGetMinMax4)
{
    double resultMin;
    double resultMax;

    std::array<double, 4> testArray {2.0, 3.0, 3.0, 5.0};

    std::map<int, AgentInterface*> stubAgents;
    std::vector<const TrafficObjectInterface*> stubObjects;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetAgents()).WillByDefault(ReturnRef(stubAgents));
    ON_CALL(fakeWorld, GetTrafficObjects()).WillByDefault(ReturnRef(stubObjects));

    CollisionDetector detector{&fakeWorld, nullptr, nullptr, nullptr};
    detector.GetMinMax4(testArray, resultMax, resultMin);

    ASSERT_EQ(resultMin, 2.0);
    ASSERT_EQ(resultMax, 5.0);
}

TEST(CollisionDetector_UnitTests, TestGetMinMax2)
{
    double resultMin;
    double resultMax;

    std::array<double, 2> testArray {2.0, 3.0};

    std::map<int, AgentInterface*> stubAgents;
    std::vector<const TrafficObjectInterface*> stubObjects;

    NiceMock<FakeWorld> fakeWorld;
    ON_CALL(fakeWorld, GetAgents()).WillByDefault(ReturnRef(stubAgents));
    ON_CALL(fakeWorld, GetTrafficObjects()).WillByDefault(ReturnRef(stubObjects));

    CollisionDetector detector{&fakeWorld, nullptr, nullptr, nullptr};
    detector.GetMinMax2(testArray, resultMax, resultMin);

    ASSERT_EQ(resultMin, 2.0);
    ASSERT_EQ(resultMax, 3.0);
}
