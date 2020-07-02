/*******************************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"
#include "sceneryImporter.h"

#include "fakeOdRoad.h"
#include "fakeJunction.h"
#include "fakeConnection.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::SizeIs;
using ::testing::NiceMock;

using namespace Importer;

TEST(SceneryImporter_UnitTests, ParseRoadTypes)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "    <type s=\"100.1\" type=\"motorway\" />"
    "    <type s=\"100.2\" type=\"town\" />"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;


    EXPECT_CALL(mockRoad, AddRoadType(_)).Times(2);
    ASSERT_NO_THROW(SceneryImporter::ParseRoadTypes(documentRoot, &mockRoad));
}


TEST(SceneryImporter_UnitTests, ParseJunctionConnections)
{
    QDomElement documentRoot = documentRootFromString(
    "<junction>"
    "     <connection id=\"0\" incomingRoad=\"44\" connectingRoad=\"48\" contactPoint=\"start\">"
    "            <laneLink from=\"-5\" to=\"-1\"/>"
    "            <laneLink from=\"-7\" to=\"-2\"/>"
    "            <laneLink from=\"-8\" to=\"-3\"/>"
    "      </connection>"
    "</junction>");

    NiceMock<FakeJunction> mockJunction;
    NiceMock<FakeConnection> mockConnection;


    EXPECT_CALL(mockJunction, AddConnection("0","44","48",ContactPointType::Start)).Times(1).WillOnce(Return(&mockConnection));
    EXPECT_CALL(mockConnection, AddLink(-5,-1)).Times(1);
    EXPECT_CALL(mockConnection, AddLink(-7,-2)).Times(1);
    EXPECT_CALL(mockConnection, AddLink(-8,-3)).Times(1);


    ASSERT_NO_THROW(SceneryImporter::ParseJunctionConnections(documentRoot,&mockJunction));
}


TEST(SceneryImporter_UnitTests, ParseSignalsWithThreeValidSignals_DetectsThreeSignals)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <signals>"
    "    <signal s=\"100.1\" t=\"1.1\" id=\"id0\" name=\"n0\" dynamic=\"no\"  orientation=\"+\"    zOffset=\"0.1\" country=\"c1\" type=\"-1\" subtype=\"none\" value=\"0.123\"               height=\"1.01\" width=\"0.11\" text=\"t1\" hOffset=\"1e-3\" pitch=\"1e-2\"/>"
    "    <signal s=\"200.2\" t=\"2.2\" id=\"id1\" name=\"n1\" dynamic=\"no\"  orientation=\"-\"    zOffset=\"0.2\" country=\"c2\" type=\"none\" subtype=\"-1\"   value=\"0.456\" unit=\"mph\"  height=\"2.02\" width=\"0.22\" text=\"t2\" hOffset=\"2e-3\" pitch=\"2e-2\"/>"
    "    <signal s=\"300.3\" t=\"3.3\" id=\"id2\" name=\"n2\" dynamic=\"yes\" orientation=\"none\" zOffset=\"0.3\" country=\"c3\" type=\"274\" subtype=\"100\" value=\"0.789\" unit=\"km/h\" height=\"3.03\" width=\"0.33\" text=\"t3\" hOffset=\"3e-3\" pitch=\"3e-2\"/>"
    "  </signals>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    //EXPECT_CALL(mockRoad, AddRoadSignal(_)).Times(3);
    ASSERT_NO_THROW(SceneryImporter::ParseSignals(documentRoot, &mockRoad));
}


struct SignalInterceptor
{
    static RoadSignalSpecification signal;

    static void intercept(const RoadSignalSpecification& signal)
    {
        SignalInterceptor::signal = signal;
    }
};

RoadSignalSpecification SignalInterceptor::signal;

//TODO: Pimp Tests with parameterized interface to test for the individual settings, such as dynamic = yes/no or unit available or not, etc...
TEST(SceneryImporter_UnitTests, ParseSignalsWithValidSignal_ReturnsCorrectValues)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <signals>"
    "    <signal s=\"100.1\" t=\"1.1\" id=\"id0\" name=\"n0\" dynamic=\"no\" orientation=\"-\" zOffset=\"0.1\" country=\"c1\" type=\"none\" subtype=\"none\" value=\"0.123\"  height=\"1.01\" width=\"0.11\" text=\"t1\" hOffset=\"1e-3\" pitch=\"1e-2\">"
    "      <validity fromLane=\"-3\" toLane=\"-2\" />"
    "     </signal>"
    "  </signals>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ON_CALL(mockRoad, AddRoadSignal(_)).WillByDefault(Invoke(SignalInterceptor::intercept));

    ASSERT_NO_THROW(SceneryImporter::ParseSignals(documentRoot, &mockRoad));

    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.s, 100.1);
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.t, 1.1);
    EXPECT_EQ(SignalInterceptor::signal.id, "id0");
    EXPECT_EQ(SignalInterceptor::signal.name, "n0");
    EXPECT_EQ(SignalInterceptor::signal.orientation, "-");
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.zOffset, 0.1);
    EXPECT_EQ(SignalInterceptor::signal.country, "c1");
    EXPECT_EQ(SignalInterceptor::signal.type, "none");
    EXPECT_EQ(SignalInterceptor::signal.subtype, "none");
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.value, 0.123);
    EXPECT_EQ(SignalInterceptor::signal.unit, RoadSignalUnit::Undefined);
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.height, 1.01);
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.width, 0.11);
    EXPECT_EQ(SignalInterceptor::signal.text, "t1");
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.hOffset, 1e-3);
    EXPECT_DOUBLE_EQ(SignalInterceptor::signal.pitch, 1e-2);
    EXPECT_EQ(SignalInterceptor::signal.validity.lanes[0], -3);
    EXPECT_EQ(SignalInterceptor::signal.validity.lanes[1], -2);
}

TEST(SceneryImporter_UnitTests, ParseSignalsWithMissingField_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <signals>"
    "    <signal s=\"100.1\" t=\"1.1\" id=\"id0\" name=\"n0\" dynamic=\"no\" zOffset=\"0.1\" country=\"c1\" type=\"none\" subtype=\"none\" value=\"0.123\" height=\"1.01\" width=\"0.11\" text=\"t1\" hOffset=\"1e-3\" pitch=\"1e-2\"/>"
    "  </signals>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseSignals(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseSignalsWithInvalidField_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <signals>"
    "    <signal s=\"-100.1\" t=\"x1\" id=\"id0\" name=\"n0\" dynamic=\"no\" zOffset=\"0.1\" country=\"c1\" type=\"\" subtype=\"\" value=\"0.123\" height=\"1.01\" width=\"0.11\" text=\"t1\" hOffset=\"1e-3\" pitch=\"1e-2\"/>"
    "  </signals>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseSignals(documentRoot, &mockRoad), std::runtime_error);
}


TEST(SceneryImporter_UnitTests, ValidityWithThreeIncreasingLaneNumbers_DeliversThreeValidityEntries)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <validity fromLane=\"0\" toLane=\"2\" />"
    "</root>");

    RoadElementValidity validitiy;

    ASSERT_NO_THROW(SceneryImporter::ParseElementValidity(documentRoot, validitiy));
    ASSERT_THAT(validitiy.lanes, SizeIs(3));
    EXPECT_FALSE(validitiy.all);
    EXPECT_EQ(validitiy.lanes[0], 0);
    EXPECT_EQ(validitiy.lanes[1], 1);
    EXPECT_EQ(validitiy.lanes[2], 2);
}

TEST(SceneryImporter_UnitTests, ValidityWithThreeDecreasingLaneNumbers_DeliversThreeValidityEntries)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <validity fromLane=\"0\" toLane=\"-2\" />"
    "</root>");

    RoadElementValidity validitiy;

    ASSERT_NO_THROW(SceneryImporter::ParseElementValidity(documentRoot, validitiy));
    ASSERT_THAT(validitiy.lanes, SizeIs(3));
    EXPECT_FALSE(validitiy.all);
    EXPECT_EQ(validitiy.lanes[0], -2);
    EXPECT_EQ(validitiy.lanes[1], -1);
    EXPECT_EQ(validitiy.lanes[2], 0);
}

TEST(SceneryImporter_UnitTests, ValidityWithMissingFromLane_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <validity toLane=\"0\" />"
    "</root>");

    RoadElementValidity validitiy;

    ASSERT_THROW(SceneryImporter::ParseElementValidity(documentRoot, validitiy), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ValidityWithMissingToLane_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <validity fromLane=\"0\" />"
    "</root>");

    RoadElementValidity validitiy;

    ASSERT_THROW(SceneryImporter::ParseElementValidity(documentRoot, validitiy), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, NoValidity_SetsAllValidityFlag)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "</root>");

    RoadElementValidity validitiy;

    ASSERT_NO_THROW(SceneryImporter::ParseElementValidity(documentRoot, validitiy));
    EXPECT_TRUE(validitiy.all);
    EXPECT_THAT(validitiy.lanes, SizeIs(0));
}

TEST(SceneryImporter_UnitTests, ParseObject_ThreeBarriersAreDetected)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"100\" t=\"0\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"10\" width=\"2\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b02\" s=\"110\" t=\"1\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"11\" width=\"3\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b03\" s=\"120\" t=\"2\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"12\" width=\"4\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    //EXPECT_CALL(mockRoad, AddRoadObject(_)).Times(3);
    ASSERT_NO_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad));
}

struct ObjectInterceptor
{
    static RoadObjectSpecification object;

    static void intercept(const RoadObjectSpecification& object)
    {
        ObjectInterceptor::object = object;
    }
};

RoadObjectSpecification ObjectInterceptor::object;

//TODO: Pimp Tests with parameterized interface to test for the individual settings, such as dynamic = yes/no or unit available or not, etc...
TEST(SceneryImporter_UnitTests, ParseObjectsWithValidObject_ReturnsCorrectValues)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b03\" s=\"120\" t=\"2\" zOffset=\"0\" validLength=\"0\" orientation=\"+\" length=\"12\" width=\"4\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ON_CALL(mockRoad, AddRoadObject(_)).WillByDefault(Invoke(ObjectInterceptor::intercept));
    ASSERT_NO_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad));

    EXPECT_EQ(ObjectInterceptor::object.type, RoadObjectType::barrier);
    EXPECT_EQ(ObjectInterceptor::object.name, "obstacle");
    EXPECT_EQ(ObjectInterceptor::object.id, "b03");
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.s, 120);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.t, 2);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.zOffset, 0);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.validLength, 0);
    EXPECT_EQ(ObjectInterceptor::object.orientation, RoadElementOrientation::positive);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.width, 4);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.length, 12);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.radius, 0);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.height, 2);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.hdg, 0);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.pitch, 0);
    EXPECT_DOUBLE_EQ(ObjectInterceptor::object.roll, 0);
}

TEST(SceneryImporter_UnitTests, ParseObjectsWithMissingField_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"100\" t=\"0\" zOffset=\"0\" orientation=\"none\" length=\"10\" width=\"2\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");         // validLength is missing

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseObjectssWithInvalidS_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"-100\" t=\"10\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"10\" width=\"2\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");   // s is negative

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseObjectssWithInvalidDimensions_Fails)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"100\" t=\"10\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"-1.0\" width=\"-1.0\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");   // length, width = 0

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseObjectsWithZeroLength_ThrowsException)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"100\" t=\"10\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"0.0\" width=\"2.0\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseObjectsWithZeroWidth_ThrowsException)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <objects>"
    "    <object type=\"barrier\" name=\"obstacle\" id=\"b01\" s=\"100\" t=\"10\" zOffset=\"0\" validLength=\"0\" orientation=\"none\" length=\"2.0\" width=\"0.0\" height=\"2\" hdg=\"0\" pitch=\"0\" roll=\"0\" />"
    "  </objects>"
    "</root>");

    NiceMock<FakeOdRoad> mockRoad;

    ASSERT_THROW(SceneryImporter::ParseObjects(documentRoot, &mockRoad), std::runtime_error);
}

TEST(SceneryImporter_UnitTests, ParseRepeatWithNoOverridingOfOptionalParameters)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <repeat s=\"100\" length=\"200\" distance=\"50\"/>"
    "</root>");

    RoadObjectSpecification object;
    object.type = RoadObjectType::obstacle;
    object.name = "Leitplanke";
    object.id = "";
    object.s = 0;
    object.t = 1;
    object.zOffset = 0;
    object.validLength = 0;
    object.orientation = RoadElementOrientation::negative;
    object.length = 2.0;
    object.width = 1.0;
    object.height = 0;
    object.hdg = 0;
    object.pitch = 0;
    object.roll = 0;
    object.radius = 0;
    std::list<RoadObjectSpecification> objectRepitions;

    objectRepitions = SceneryImporter::ParseObjectRepeat(documentRoot, object);

    for(auto object : objectRepitions)
    {
      ASSERT_EQ(object.t, 1);
      ASSERT_EQ(object.length, 2.0);
      ASSERT_EQ(object.width, 1.0);
      ASSERT_EQ(object.height, 0);
      ASSERT_EQ(object.zOffset, 0);
    }
}

TEST(SceneryImporter_UnitTests, ParseRepeatTestOverridingOfObjectRepeat)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <repeat s=\"100\" length=\"200\" distance=\"50\"/>"
    "</root>");

    RoadObjectSpecification object;
    object.s = 0;
    object.radius = 0;
    object.length = 2.0;
    object.width = 1.0;
    std::list<RoadObjectSpecification> objectRepitions;

    objectRepitions = SceneryImporter::ParseObjectRepeat(documentRoot, object);

    int distance = 50;
    int sStart = 100;
    int count = 0;
    for(auto object : objectRepitions)
    {
      ASSERT_EQ(object.s, (sStart + count++ * distance));
    }
}

TEST(SceneryImporter_UnitTests, ParseRepeatOverridesAllOptionalParameters)
{
    QDomElement documentRoot = documentRootFromString(
    "<root>"
    "  <repeat s=\"100\" length=\"200\" distance=\"50\" tStart=\"1.5\" tEnd=\"2\" widthStart=\"2\" widthEnd=\"3\" heightStart=\"1\" heightEnd=\"2\" zOffsetStart=\"0\" zOffsetEnd=\"0.1\" />"
    "</root>");

    RoadObjectSpecification object;
    object.type = RoadObjectType::obstacle;
    object.name = "Leitplanke";
    object.id = "";
    object.s = 0;
    object.t = 1;
    object.zOffset = 0.1;
    object.validLength = 0;
    object.orientation = RoadElementOrientation::negative;
    object.length = 5;
    object.width = 5;
    object.height = 5;
    object.hdg = 0;
    object.pitch = 0;
    object.roll = 0;
    object.radius = 0;
    std::list<RoadObjectSpecification> objectRepitions;

    objectRepitions = SceneryImporter::ParseObjectRepeat(documentRoot, object);

    RoadObjectSpecification firstObject = objectRepitions.front();
    EXPECT_NEAR(firstObject.t, 1.5, 1e-4);
    EXPECT_NEAR(firstObject.s, 100, 1e-4);
    EXPECT_NEAR(firstObject.width, 2, 1e-4);
    EXPECT_NEAR(firstObject.height, 1, 1e-4);
    EXPECT_NEAR(firstObject.zOffset, 0, 1e-4);

    ASSERT_EQ(firstObject.type, RoadObjectType::obstacle);
    ASSERT_EQ(firstObject.length, 5);
    ASSERT_EQ(firstObject.hdg, 0);
}
