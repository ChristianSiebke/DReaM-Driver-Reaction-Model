/********************************************************************************
 * Copyright (c) 2018-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cmath>

#include "fakeMovingObject.h"
#include "fakeTrafficSign.h"
#include "fakeLane.h"
#include "Primitives.h"
#include "WorldData.h"

#include "osi3/osi_sensorview.pb.h"
#include "osi3/osi_sensorviewconfiguration.pb.h"

using namespace OWL;

using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SizeIs;
using ::testing::_;

struct SensorViewTest_Data
{
    struct Sensor
    {
        Sensor(double x, double y, double angle_left_abs_degree, double angle_right_abs_degree, double radius)
        : x{x},
          y{y},
          angle_left_abs_rad{angle_left_abs_degree * M_PI / 180.},
          angle_right_abs_rad{angle_right_abs_degree * M_PI / 180.},
          radius{radius}
        {}

        double  x;
        double  y;
        double  angle_left_abs_rad;
        double angle_right_abs_rad;
        double radius;
        friend std::ostream& operator<<(std::ostream& os, const Sensor& obj)
        {
            return os
            << "Position: "              << obj.x << ";" << obj.y
            << " | Opening Angles (deg): " << (obj.angle_left_abs_rad * 180. / M_PI) << ";" << (obj.angle_right_abs_rad * 180. / M_PI)
            << " | Radius: "               << obj.radius;
        }
    };

    struct Object
    {
        double x;
        double y;
        double length;
        double width;
        double rotation;
        friend std::ostream& operator<<(std::ostream& os, const Object& obj)
        {
            return os
            << "Position: "   << obj.x << ";" << obj.y
            << " | Dimension: " << obj.length << ";" << obj.width
            << " | Rotation: "  << obj.rotation;
        }
    };

    std::string testcase;
    Sensor sensor;
    Object object;
    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const SensorViewTest_Data& obj)
     {
        return os
         << "\n[ TESTCASE ] " << obj.testcase
         << "\n[  SENSOR  ] " << obj.sensor
         << "\n[  OBJECT  ] " << obj.object;
     }
};

class SensorViewTestObjectDetection : public ::testing::TestWithParam<SensorViewTest_Data>
{
protected:
    OWL::WorldData worldData{nullptr};
};

TEST_P(SensorViewTestObjectDetection, TestGenerator)
{
    auto data = GetParam();

    Primitive::AbsPosition sensorPosition = {data.sensor.x, data.sensor.y, 0};

    OWL::Fakes::MovingObject object;
    Primitive::AbsPosition  objectPosition = {data.object.x, data.object.y, 0};
    ON_CALL(object,  GetReferencePointPosition()).WillByDefault(Return(objectPosition));
    Primitive::Dimension  objectDimension = {data.object.length, data.object.width, 0};
    ON_CALL(object,  GetDimension()).WillByDefault(Return(objectDimension));
    std::vector<OWL::Interfaces::MovingObject*> fakeMovingObjects{&object};

    auto filteredMoving = worldData.ApplySectorFilter(fakeMovingObjects, sensorPosition, data.sensor.radius, data.sensor.angle_left_abs_rad, data.sensor.angle_right_abs_rad);
    ASSERT_THAT(filteredMoving, SizeIs(1));
}

INSTANTIATE_TEST_SUITE_P(ObjectsTouchingSensorView, SensorViewTestObjectDetection, ::testing::Values(
    SensorViewTest_Data{"Large object in backshadow or regular sensor",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 45.0, -45.0, 10.0},
                        SensorViewTest_Data::Object{-40.0, -0.50, 82.0, 2.0, 3.0}},
    SensorViewTest_Data{"Small object in gap of pacman-style sensor",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 170.0, -170.0, 10.0},
                        SensorViewTest_Data::Object{-6.0, 0.50, 4.0, 0.50, -175.0}},
    SensorViewTest_Data{"Medium object in 'negative' sensorconfig",
                        SensorViewTest_Data::Sensor{-5.0, 3.0, -45.0, 20.0, 20.0},
                        SensorViewTest_Data::Object{25.0, -8.0, 50.0, 5.0, -30.0}}
));

INSTANTIATE_TEST_SUITE_P(SimpleCasesWithSensor60Degree, SensorViewTestObjectDetection, ::testing::Values(
    SensorViewTest_Data{"Small Object touches sensor looking north",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 120.0, 60.0, 10.0},
                        SensorViewTest_Data::Object{-2.0, 10.0, 2.0, 1.0, 90.0}},
    SensorViewTest_Data{"Object on backshadow of sensor looking north",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 120.0, 60.0, 10.0},
                        SensorViewTest_Data::Object{2.0, 8.0, 2.0, 1.0, 175.0}},
    SensorViewTest_Data{"Object within cone of sensor looking north",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 120.0, 60.0, 10.0},
                        SensorViewTest_Data::Object{-3.50, 4.0, 2.0, 1.0, -175.0}},
    SensorViewTest_Data{"Object touches sensor looking south",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -60.0, -120.0, 10.0},
                        SensorViewTest_Data::Object{2.0, -10.0, 2.0, 1.0, -90.0}},
    SensorViewTest_Data{"Object on backshadow of sensor looking south",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -60.0, -120.0, 10.0},
                        SensorViewTest_Data::Object{-2.0, -8.0, 2.0, 1.0, -175.0}},
    SensorViewTest_Data{"Object within cone of sensor looking south",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -60.0, -120.0, 10.0},
                        SensorViewTest_Data::Object{3.50, -4.0, 2.0, 1.0, 175.0}},
    SensorViewTest_Data{"Object touches sensor looking west",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -150.0, 150.0, 10.0},
                        SensorViewTest_Data::Object{-10.0, 2.0, 2.0, 1.0, 0.0}},
    SensorViewTest_Data{"Object on backshadow of sensor looking west",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -150.0, 150.0, 10.0},
                        SensorViewTest_Data::Object{-8.0, -2.0, 2.0, 1.0, 85.0}},
    SensorViewTest_Data{"Object within cone of sensor looking west",
                        SensorViewTest_Data::Sensor{0.0, 0.0, -150.0, 150.0, 10.0},
                        SensorViewTest_Data::Object{-4.0, 3.50, 2.0, 1.0, -265.0}},
    SensorViewTest_Data{"Object touches sensor looking east",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 30.0, -30.0, 10.0},
                        SensorViewTest_Data::Object{10.0, -2.0, 2.0, 1.0, 180.0}},
    SensorViewTest_Data{"Object on backshadow of sensor looking east",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 30.0, -30.0, 10.0},
                        SensorViewTest_Data::Object{8.0, 2.0, 2.0, 1.0, 95.0}},
    SensorViewTest_Data{"Object within cone of sensor looking east",
                        SensorViewTest_Data::Sensor{0.0, 0.0, 30.0, -30.0, 10.0},
                        SensorViewTest_Data::Object{4.0, -3.50, 2.0, 1.0, 445.0}}
));

class TestWorldData : public OWL::WorldData
{
public:
    TestWorldData() :
        OWL::WorldData{nullptr}
    {}

    MOCK_CONST_METHOD1(GetMovingObject, const Interfaces::MovingObject& (Id id));
};

TEST(SensorViewTests, AddHostVehicleToSensorView_SetsHostVehicleAndLaneAssignments)
{
    TestWorldData worldData;
    osi3::SensorView sensorView;

    OWL::Id idLane1 = 101, idLane2 = 102, idLane3 = 103;
    sensorView.mutable_global_ground_truth()->add_lane()->mutable_id()->set_value(idLane1);
    sensorView.mutable_global_ground_truth()->add_lane()->mutable_id()->set_value(idLane2);
    sensorView.mutable_global_ground_truth()->add_lane()->mutable_id()->set_value(idLane3);

    OWL::Fakes::MovingObject hostVehicle;

    ON_CALL(hostVehicle, CopyToGroundTruth(_)).WillByDefault(
                [](osi3::GroundTruth& groundTruth)
    {auto baseMoving = groundTruth.add_moving_object()->mutable_base();
    baseMoving->mutable_position()->set_x(2.0);
    baseMoving->mutable_position()->set_y(3.0);});

    OWL::Fakes::Lane lane1;
    ON_CALL(lane1, GetId()).WillByDefault(Return(idLane1));
    OWL::Fakes::Lane lane3;
    ON_CALL(lane3, GetId()).WillByDefault(Return(idLane3));
    OWL::Interfaces::Lanes laneAssignments{{&lane1, &lane3}};
    ON_CALL(hostVehicle, GetLaneAssignments()).WillByDefault(ReturnRef(laneAssignments));

    const OWL::Id host_id = 11;
    ON_CALL(worldData, GetMovingObject(host_id)).WillByDefault(ReturnRef(hostVehicle));
    worldData.AddHostVehicleToSensorView(host_id, sensorView);

    EXPECT_THAT(sensorView.host_vehicle_id().value(), Eq(host_id));
    EXPECT_THAT(sensorView.global_ground_truth().host_vehicle_id().value(), Eq(host_id));
    EXPECT_THAT(sensorView.host_vehicle_data().location().position().x(), Eq(2.0));
    EXPECT_THAT(sensorView.host_vehicle_data().location().position().y(), Eq(3.0));
    for(const auto& lane : sensorView.global_ground_truth().lane())
    {
        if(lane.id().value() == idLane1 || lane.id().value() == idLane3)
        {
            EXPECT_THAT(lane.classification().is_host_vehicle_lane(), Eq(true));
        }
        else
        {
            EXPECT_THAT(lane.classification().is_host_vehicle_lane(), Eq(false));
        }
    }
}
