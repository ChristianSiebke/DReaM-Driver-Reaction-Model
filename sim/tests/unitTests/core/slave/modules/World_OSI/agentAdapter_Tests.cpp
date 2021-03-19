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

TEST(MovingObject_Tests, SetAndGetReferencePointPosition_ReturnsCorrectPosition)
{
    OWL::Primitive::AbsPosition position;
    position.x = 100.0;
    position.y = 150.0;
    position.z = 10.0;
    osi3::MovingObject osiObject;
    OWL::Implementation::MovingObject movingObject(&osiObject, nullptr);
    movingObject.SetLength(8.0);
    movingObject.SetDistanceReferencPointToLeadingEdge(7.0);
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
    movingObject.SetDistanceReferencPointToLeadingEdge(7.0);
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
    movingObject.SetDistanceReferencPointToLeadingEdge(6.0);
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