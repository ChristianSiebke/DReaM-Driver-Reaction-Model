/*********************************************************************
* Copyright (c) 2017, 2018, 2019, 2020 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "AgentAdapter.h"
#include "fakeMovingObject.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::IsNull;
using ::testing::_;
using ::testing::Eq;
using ::testing::Ge;
using ::testing::Le;
using ::testing::Lt;
using ::testing::AllOf;
using ::testing::DoubleEq;
using ::testing::ElementsAreArray;

TEST(MovingObject_Tests, SetAndGetReferencePointPosition_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(7.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetAndGetReferencePointPositionWithYawChangeInBetween_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(7.0);
    movingObject.SetYaw(0.5);
    movingObject.SetReferencePointPosition(position);
    movingObject.SetYaw(0.7);
    OWL::Primitive::AbsPosition resultPosition = movingObject.GetReferencePointPosition();
    ASSERT_THAT(resultPosition.x, DoubleEq(position.x));
    ASSERT_THAT(resultPosition.y, DoubleEq(position.y));
    ASSERT_THAT(resultPosition.z, DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetReferencePointPosition_SetsCorrectPositionOnOSIObject)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetBoundingBoxCenterToRear(-2.0);
    movingObject.SetYaw(M_PI * 0.25);
    movingObject.SetReferencePointPosition(position);
    auto resultPosition = osiObject.base().position();
    ASSERT_THAT(resultPosition.x(), DoubleEq(position.x + std::sqrt(2)));
    ASSERT_THAT(resultPosition.y(), DoubleEq(position.y + std::sqrt(2)));
    ASSERT_THAT(resultPosition.z(), DoubleEq(position.z));
}

TEST(MovingObject_Tests, SetAgentType_MapsCorrectOSIType)
{
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);

    const std::vector<std::pair<AgentVehicleType, osi3::MovingObject_VehicleClassification_Type>> expectedVehicleTypes = 
    {{AgentVehicleType::Car, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MEDIUM_CAR},
     {AgentVehicleType::Motorbike, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_MOTORBIKE},
     {AgentVehicleType::Bicycle, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_BICYCLE},
     {AgentVehicleType::Truck, osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_HEAVY_TRUCK}};

    for (const auto & [agentVehicleType, expectedOsiVehicleType] : expectedVehicleTypes)
    {
        movingObject.SetType(agentVehicleType);

        ASSERT_THAT(osiObject.type(), osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE);
        ASSERT_THAT(osiObject.vehicle_classification().type(), expectedOsiVehicleType);
    }

    movingObject.SetType(AgentVehicleType::Pedestrian);
    ASSERT_THAT(osiObject.type(), osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN);
}

struct CalculateBoundingBoxData
{
    double yaw;
    double roll;
    std::vector<std::pair<double,double>> expectedResult;
};

class CalculateBoundingBox_Tests : public testing::Test,
        public ::testing::WithParamInterface<CalculateBoundingBoxData>
{
};

class TestWorldObject : public WorldObjectAdapter
{
public:
    TestWorldObject (OWL::Interfaces::WorldObject& baseTrafficObject) :
        WorldObjectAdapter(baseTrafficObject)
    {}

    virtual double GetLaneRemainder(const std::string& roadId, Side) const{};
    virtual ObjectTypeOSI GetType() const {}
    virtual const ObjectPosition& GetObjectPosition() const {}
    virtual double GetDistanceToStartOfRoad(MeasurementPoint mp, std::string roadId) const {return 0;}
    virtual double GetVelocity(VelocityScope velocityScope) const {return 0;}
    virtual bool Locate() {return false;}
    virtual void Unlocate() {};
};

TEST_P(CalculateBoundingBox_Tests, CalculateBoundingBox_ReturnCorrectPoints)
{
    const auto& data = GetParam();
    OWL::Fakes::MovingObject movingObject;
    OWL::Primitive::AbsPosition position{10, 20, 0};
    ON_CALL(movingObject, GetReferencePointPosition()).WillByDefault(Return(position));
    OWL::Primitive::Dimension dimension{6.0, 2.0, 1.6};
    ON_CALL(movingObject, GetDimension()).WillByDefault(Return(dimension));
    OWL::Primitive::AbsOrientation orientation{data.yaw, 0, data.roll};
    ON_CALL(movingObject, GetAbsOrientation()).WillByDefault(Return(orientation));

    TestWorldObject object(movingObject);

    auto result = object.GetBoundingBox2D();

    std::vector<std::pair<double,double>> resultPoints;
    for (const point_t point : result.outer())
    {
        resultPoints.emplace_back(bg::get<0>(point), bg::get<1>(point));
    }
    resultPoints.pop_back(); //in boost the last point is equal to the first

    ASSERT_THAT(resultPoints, ElementsAreArray(data.expectedResult));
}

INSTANTIATE_TEST_SUITE_P(BoundingBoxTest, CalculateBoundingBox_Tests,
                        testing::Values(
//!                      yaw     roll   expectedResult
CalculateBoundingBoxData{0.0,     0.0, {{7.0, 19.0},{7.0, 21.0}, {13.0, 21.0}, {13.0, 19.0}}},
CalculateBoundingBoxData{M_PI_2,  0.0, {{11.0, 17.0},{9.0, 17.0}, {9.0, 23.0}, {11.0, 23.0}}},
CalculateBoundingBoxData{0.0,  M_PI_4, {{7.0, 20 - 2.6*M_SQRT1_2},{7.0, 20.0 + M_SQRT1_2}, {13.0, 20.0  + M_SQRT1_2}, {13.0, 20.0 - 2.6*M_SQRT1_2}}},
CalculateBoundingBoxData{0.0, -M_PI_4, {{7.0, 20 - M_SQRT1_2},{7.0, 20.0 + 2.6*M_SQRT1_2}, {13.0, 20.0  + 2.6*M_SQRT1_2}, {13.0, 20.0 - M_SQRT1_2}}}
));
