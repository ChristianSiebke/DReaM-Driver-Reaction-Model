/*********************************************************************
* Copyright (c) 2018, 2019, 2020 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SceneryConverter.h"

#include "fakeAgent.h"
#include "fakeLane.h"
#include "fakeLaneManager.h"
#include "fakeMovingObject.h"
#include "fakeOdRoad.h"
#include "fakeRoadObject.h"
#include "fakeRoadLaneSection.h"
#include "fakeSection.h"
#include "fakeWorld.h"
#include "fakeWorldData.h"
#include "fakeConnection.h"
#include "fakeJunction.h"

#include "Generators/laneGeometryElementGenerator.h"

using namespace OWL;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgReferee;
using ::testing::Invoke;
using ::testing::Const;
using ::testing::_;

std::tuple<const OWL::Primitive::LaneGeometryJoint*, const OWL::Primitive::LaneGeometryJoint*> CreateSectionPartJointsRect(double length)
{
    OWL::Implementation::Lane lane(nullptr, nullptr);

    auto laneGeometryElement =
        OWL::Testing::LaneGeometryElementGenerator::RectangularLaneGeometryElement(
    { 0.0, 0.0 },  // origin
    0.0,           // width
    length,
    0.0);          // heading

    const OWL::Primitive::LaneGeometryJoint& firstJoint = laneGeometryElement.joints.current;
    const OWL::Primitive::LaneGeometryJoint& secondJoint = laneGeometryElement.joints.next;

    Common::Vector2d leftPoint1 = firstJoint.points.left;
    Common::Vector2d referencePoint1 = firstJoint.points.reference;
    Common::Vector2d rightPoint1 = firstJoint.points.right;
    Common::Vector2d leftPoint2 = secondJoint.points.left;
    Common::Vector2d referencePoint2 = secondJoint.points.reference;
    Common::Vector2d rightPoint2 = secondJoint.points.right;

    lane.AddLaneGeometryJoint(leftPoint1, referencePoint1, rightPoint1, 0.0, 0.0, 0.0);
    lane.AddLaneGeometryJoint(leftPoint2, referencePoint2, rightPoint2, length, 0.0, 0.0);

    return lane.GetNeighbouringJoints(length * 0.5);
}

TEST(CalculateAbsolutCoordinates, Test1)
{
    const OWL::Primitive::LaneGeometryJoint* prevJoint;
    const OWL::Primitive::LaneGeometryJoint* nextJoint;
    std::tie(prevJoint, nextJoint) = CreateSectionPartJointsRect(10);

    ASSERT_NE(prevJoint, nullptr);
    ASSERT_NE(nextJoint, nullptr);

    ASSERT_EQ(prevJoint->points.reference.x, 0.0);
    ASSERT_EQ(nextJoint->points.reference.x, 10);
}

TEST(CalculateAbsolutCoordinates, Test2)
{
    FakeLaneManager laneManager(1, 1, 3.0, {100}, "TestRoadId");
    FakeOdRoad fakeRoadInterface;
    ON_CALL(fakeRoadInterface, GetId()).WillByDefault(Return("ArbitraryRoadOd"));

    //add on call GetInterpolatedPoint in fakemanager

    NiceMock<Fakes::WorldData> worldData;
    ON_CALL(worldData, GetRoads()).WillByDefault(ReturnRef(laneManager.GetRoads()));
    ON_CALL(worldData, GetRoadIdMapping()).WillByDefault(ReturnRef(laneManager.GetRoadIdMapping()));
    ON_CALL(worldData, GetSections()).WillByDefault(ReturnRef(laneManager.GetSections()));
    ON_CALL(worldData, GetLaneIdMapping()).WillByDefault(ReturnRef(laneManager.GetLaneIdMapping()));

    NiceMock<FakeRoadObject> testRoadObject;
    ON_CALL(testRoadObject, GetS()).WillByDefault(Return(5));
    ON_CALL(testRoadObject, GetT()).WillByDefault(Return(0));
    ON_CALL(testRoadObject, GetHdg()).WillByDefault(Return(0));
    World::Localization::Localizer localizer{worldData};

    SceneryConverter converter(nullptr, worldData, localizer, nullptr);

    bool isInWorld;
    double x, y, yaw;

    for(auto it : laneManager.GetSections())
    {
        std::tie(isInWorld, x, y, yaw) = converter.CalculateAbsoluteCoordinates(&fakeRoadInterface, it.second, &testRoadObject);
    }
}

class FakeScenery : public SceneryInterface
{
public:
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(AddRoad, RoadInterface *(const std::string &id));
    MOCK_METHOD1(AddJunction, JunctionInterface *(const std::string &id));
    MOCK_CONST_METHOD0(GetRoads, std::map<std::string, RoadInterface *> &());
    MOCK_CONST_METHOD1(GetRoad, RoadInterface *(const std::string &id));
    MOCK_CONST_METHOD0(GetJunctions, std::map<std::string, JunctionInterface *> &());
    MOCK_CONST_METHOD1(GetJunction, JunctionInterface *(const std::string &id));
};

class FakeRoadLink : public RoadLinkInterface
{
public:
    MOCK_CONST_METHOD0(GetType, RoadLinkType());
    MOCK_CONST_METHOD0(GetElementType, RoadLinkElementType());
    MOCK_CONST_METHOD0(GetElementId, const std::string &());
    MOCK_CONST_METHOD0(GetContactPoint, ContactPointType());
    MOCK_CONST_METHOD0(GetDirection, RoadLinkDirectionType());
    MOCK_CONST_METHOD0(GetSide, RoadLinkSideType());
};

void Connect(const RoadInterface* incomingRoad, const RoadInterface* connectingRoad,
             const RoadInterface* outgoingRoad, ContactPointType incomingContactPoint, ContactPointType outgoingContactPoint,
             std::map<int, int> laneIdMapping)
{

}


TEST(SceneryConverter, RefactoringSafeguard_DoNotDelete)
{
    FakeScenery stubScenery;
    FakeConnection stubConnection;

    FakeOdRoad incomingRoad;
    ON_CALL(stubConnection, GetIncommingRoadId()).WillByDefault(Return("incomingRoadId"));
    ON_CALL(stubScenery, GetRoad("incomingRoadId")).WillByDefault(Return(&incomingRoad));

    FakeOdRoad connectingRoad;
    ON_CALL(stubConnection, GetConnectingRoadId()).WillByDefault(Return("connectingRoadId"));
    ON_CALL(stubScenery, GetRoad("connectingRoadId")).WillByDefault(Return(&connectingRoad));

    FakeRoadLink roadLink;
    std::string outgoingRoadId = "outgoingRoadId";
    ON_CALL(roadLink, GetType()).WillByDefault(Return(RoadLinkType::Successor));
    ON_CALL(roadLink, GetElementId()).WillByDefault(ReturnRef(outgoingRoadId));

    std::list<RoadLinkInterface*> roadLinks = { {&roadLink} };
    ON_CALL(connectingRoad, GetRoadLinks()).WillByDefault(ReturnRef(roadLinks));

    FakeOdRoad outgoingRoad;
    ON_CALL(stubScenery, GetRoad("outgoingRoadId")).WillByDefault(Return(&outgoingRoad));

    FakeRoadLaneSection stubRoadLaneSection;
    std::vector<RoadLaneSectionInterface *> stubLaneSections = { &stubRoadLaneSection };
    ON_CALL(incomingRoad, GetLaneSections()).WillByDefault(ReturnRef(stubLaneSections));
    ON_CALL(connectingRoad, GetLaneSections()).WillByDefault(ReturnRef(stubLaneSections));

    FakeJunction stubJunction;
    std::map<std::string, ConnectionInterface*> stubConnections = {{"", &stubConnection}};
    ON_CALL(stubJunction, GetConnections()).WillByDefault(Return(stubConnections));

    auto [status, error_message] = Internal::ConnectJunction(
            &stubScenery,
            &stubJunction,

            [&](const JunctionInterface*, const RoadInterface *incomingRoad, const RoadInterface *connectingRoad, const RoadInterface *outgoingRoad,
                ContactPointType incomingContactPoint, ContactPointType outgoingContactPoint,
                std::map<int, int> laneIdMapping) {
                Connect(incomingRoad, connectingRoad, outgoingRoad, incomingContactPoint,
                                      outgoingContactPoint, laneIdMapping); });

    ASSERT_THAT(status, true);
}
