/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include <QtGlobal>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fakeAgent.h"
#include "fakePublisher.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "fakeWorldData.h"

#include "sensorGeometric2D.h"

#include "Common/boostGeometryCommon.h"

using ::testing::_;
using ::testing::Contains;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::StrEq;
using ::testing::Ne;

double constexpr EPSILON = 1e-9;

bool IsEqual(double lhs, double rhs)
{
    return std::abs(lhs - rhs) < EPSILON;
}

class MovingObjectParameter
{
public:
    MovingObjectParameter(unsigned int id,
                          Common::Vector2d position,
                          Common::Vector2d velocity,
                          Common::Vector2d acceleration,
                          double yaw) :
        id(id),
        position(position),
        velocity(velocity),
        acceleration(acceleration),
        yaw(yaw)
    {}

    MovingObjectParameter (const osi3::DetectedMovingObject& movingObject)
    {
        id = movingObject.header().ground_truth_id(0).value();
        position = Common::Vector2d(movingObject.base().position().x(), movingObject.base().position().y());
        velocity = Common::Vector2d(movingObject.base().velocity().x(), movingObject.base().velocity().y());
        acceleration = Common::Vector2d(movingObject.base().acceleration().x(), movingObject.base().acceleration().y());
        yaw = movingObject.base().orientation().yaw();
    }

    bool operator==(const MovingObjectParameter& rhs) const
    {
        if (id != rhs.id)
        {
            return false;
        }

        if (position != rhs.position ||
            velocity != rhs.velocity ||
            acceleration != rhs.acceleration)
        {
            return false;
        }

        if (!IsEqual(yaw, rhs.yaw))
        {
            return false;
        }

        return true;
    }

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const MovingObjectParameter& obj)
    {
        os << "id: " << obj.id << ", "
           << "position: (" << obj.position.x << ", " << obj.position.y << "), "
           << "velocity: (" << obj.velocity.x << ", " << obj.velocity.y << "), "
           << "acceleration: (" << obj.acceleration.x << ", " << obj.acceleration.y << "), "
           << "yaw: " << obj.yaw;

        return os;
    }

    unsigned int id;
    Common::Vector2d position;
    Common::Vector2d velocity;
    Common::Vector2d acceleration;
    double yaw;
};

class StationaryObjectParameter
{
public:
    StationaryObjectParameter(unsigned int id,
                          Common::Vector2d position,
                          double yaw) :
        id(id),
        position(position),
        yaw(yaw)
    {}

    StationaryObjectParameter (const osi3::DetectedStationaryObject& stationaryObject)
    {
        id = stationaryObject.header().ground_truth_id(0).value();
        position = Common::Vector2d(stationaryObject.base().position().x(), stationaryObject.base().position().y());
        yaw = stationaryObject.base().orientation().yaw();
    }

    bool operator==(const StationaryObjectParameter& rhs)
    {
        if (id != rhs.id)
        {
            return false;
        }

        if (position != rhs.position)
        {
            return false;
        }

        if (!IsEqual(yaw, rhs.yaw))
        {
            return false;
        }

        return true;
    }

    /// \brief This stream will be shown in case the test fails
    friend std::ostream& operator<<(std::ostream& os, const StationaryObjectParameter& obj)
    {
        os << "id: " << obj.id << ", "
           << "position: (" << obj.position.x << ", " << obj.position.y << "), "
           << "yaw: " << obj.yaw;

        return os;
    }

    unsigned int id;
    Common::Vector2d position;
    double yaw;
};

class DetectObjects_Data
{
public:
    double detectionRange;
    double openingAngleH;
    double mountingPositionLongitudinal;
    double mountingPositionLateral;
    double sensorYaw;
    double vehicleX;
    double vehicleY;
    double vehicleYaw;
    bool   enableVisualObstruction;
    std::vector<MovingObjectParameter> movingObjects;
    std::vector<StationaryObjectParameter> stationaryObjects;

    std::vector<MovingObjectParameter> expectedMovingObjects;
    std::vector<StationaryObjectParameter> expectedStationaryObjects;

    std::vector<OWL::Id> expectedVisibleMovingObjectIds;
    std::vector<OWL::Id> expectedDetectedMovingObjectIds;
};

class DetectObjects : public::testing::TestWithParam<DetectObjects_Data>
{
public:
    DetectObjects()
    {
        ON_CALL(fakeStochastics, GetUniformDistributed(_, _)).WillByDefault(Return(1));
        ON_CALL(fakeStochastics, GetLogNormalDistributed(_, _)).WillByDefault(Return(1));

        fakeDoubles = {{"FailureProbability", 0}, {"Latency", 0},
                       {"RequiredPercentageOfVisibleArea", 0.4},{"Height", 0.0},
                       {"Pitch", 0.0}, {"Roll", 0.0}};

        ON_CALL(fakeParameters, GetParametersDouble()).WillByDefault(ReturnRef(fakeDoubles));

        fakeInts = {{"Id", 0}};
        ON_CALL(fakeParameters, GetParametersInt()).WillByDefault(ReturnRef(fakeInts));

        fakeBools = {};
        ON_CALL(fakeParameters, GetParametersBool()).WillByDefault(ReturnRef(fakeBools));

        ON_CALL(fakeWorldInterface, GetWorldData()).WillByDefault(Return(&fakeWorldData));

    }

    osi3::SensorView sensorView;
    NiceMock<FakeStochastics> fakeStochastics;
    NiceMock<FakeParameter> fakeParameters;
    NiceMock<FakePublisher> fakePublisher;
    NiceMock<FakeWorld> fakeWorldInterface;
    NiceMock<OWL::Fakes::WorldData> fakeWorldData;
    NiceMock<FakeAgent> fakeAgent;
    std::map<std::string, double> fakeDoubles;
    std::map<std::string, int> fakeInts;
    std::map<std::string, bool> fakeBools;
};

void AddMovingObjectToSensorView (osi3::SensorView &sensorView, MovingObjectParameter &objectParameter)
{
    osi3::MovingObject* movingObject = sensorView.mutable_global_ground_truth()->add_moving_object();
    movingObject->mutable_base()->mutable_position()->set_x(objectParameter.position.x);
    movingObject->mutable_base()->mutable_position()->set_y(objectParameter.position.y);
    movingObject->mutable_base()->mutable_dimension()->set_length(5);
    movingObject->mutable_base()->mutable_dimension()->set_width(2);
    movingObject->mutable_vehicle_attributes()->mutable_bbcenter_to_rear()->set_x(-0.5);
    movingObject->mutable_base()->mutable_orientation()->set_yaw(objectParameter.yaw);
    movingObject->mutable_base()->mutable_velocity()->set_x(objectParameter.velocity.x);
    movingObject->mutable_base()->mutable_velocity()->set_y(objectParameter.velocity.y);
    movingObject->mutable_base()->mutable_acceleration()->set_x(objectParameter.acceleration.x);
    movingObject->mutable_base()->mutable_acceleration()->set_y(objectParameter.acceleration.y);
    movingObject->mutable_id()->set_value(objectParameter.id);
}

void AddStationaryObjectToSensorView (osi3::SensorView &sensorView, StationaryObjectParameter &objectParameter)
{
    osi3::StationaryObject* stationaryObject = sensorView.mutable_global_ground_truth()->add_stationary_object();
    stationaryObject->mutable_base()->mutable_position()->set_x(objectParameter.position.x);
    stationaryObject->mutable_base()->mutable_position()->set_y(objectParameter.position.y);
    stationaryObject->mutable_base()->mutable_dimension()->set_length(5);
    stationaryObject->mutable_base()->mutable_dimension()->set_width(2);
    stationaryObject->mutable_base()->mutable_orientation()->set_yaw(objectParameter.yaw);
    stationaryObject->mutable_id()->set_value(objectParameter.id);
}

TEST_P(DetectObjects, StoresSensorDataWithDetectedObjects)
{
    auto data = GetParam();
    fakeDoubles["DetectionRange"] = data.detectionRange;
    fakeDoubles["OpeningAngleH"] = data.openingAngleH;
    fakeDoubles["Longitudinal"] = data.mountingPositionLongitudinal;
    fakeDoubles["Lateral"] = data.mountingPositionLateral;
    fakeDoubles["Yaw"] = data.sensorYaw;
    fakeBools["EnableVisualObstruction"] = data.enableVisualObstruction;
    sensorView.mutable_host_vehicle_id()->set_value(1);
    MovingObjectParameter hostVehicle{1, {data.vehicleX, data.vehicleY}, {10.0, 5.0}, {-2.0, 3.0}, data.vehicleYaw};
    AddMovingObjectToSensorView(sensorView, hostVehicle);
    for (auto object : data.movingObjects)
    {
        AddMovingObjectToSensorView(sensorView, object);
    }
    for (auto object : data.stationaryObjects)
    {
        AddStationaryObjectToSensorView(sensorView, object);
    }
    ON_CALL(fakeWorldData, GetSensorView(_,_)).WillByDefault(Return(sensorView));
    SensorGeometric2D sensor(
                "",
                false,
                0,
                0,
                0,
                0,
                &fakeStochastics,
                &fakeWorldInterface,
                &fakeParameters,
                &fakePublisher,
                nullptr,
                &fakeAgent);

    SensorDetectionResults results = sensor.DetectObjects();

    const osi3::SensorData& sensorData = sensor.getSensorData();
    ASSERT_THAT(sensorData.moving_object_size(), Eq(data.expectedMovingObjects.size()));
    ASSERT_THAT(sensorData.stationary_object_size(), Eq(data.expectedStationaryObjects.size()));
    for (auto & movingObject : sensorData.moving_object())
    {
        ASSERT_THAT(data.expectedMovingObjects, Contains(movingObject));
    }

    for(const auto id : data.expectedVisibleMovingObjectIds)
    {
        const auto iterToMatchingId = std::find_if(results.visibleMovingObjects.begin(),
                                                   results.visibleMovingObjects.end(),
                                                   [id](const auto object) -> bool
        {
            return object.id().value() == id;
        });

        ASSERT_THAT(iterToMatchingId, Ne(results.visibleMovingObjects.end()));
    }
    for (const auto id : data.expectedDetectedMovingObjectIds)
    {
        const auto iterToMatchingId = std::find_if(results.detectedMovingObjects.begin(),
                                                   results.detectedMovingObjects.end(),
                                                   [id](const auto object) -> bool
        {
            return object.id().value() == id;
        });

        ASSERT_THAT(iterToMatchingId, Ne(results.detectedMovingObjects.end()));
    }
}

MovingObjectParameter testMovingObject2{2, {110.0, 100.0}, {5.0, 7.0}, {-0.2, 0.3}, 0.5};
MovingObjectParameter testMovingObject3{3, {150.0, 54.0}, {6.0, 8.0}, {0.0, 0.0}, 0.0};
MovingObjectParameter testMovingObject4{4, {130.0, 403.0}, {7.0, 9.0}, {-0.1, 0.4}, 0.0};
MovingObjectParameter testMovingObject5{5, {101.0, 130.0}, {8.0, 10.0}, {-0.3, 0.5}, 0.0};
MovingObjectParameter testMovingObject6{6, {70.0, 134.0}, {9.0, 11.0}, {-0.3, 0.5}, -0.5};
MovingObjectParameter testMovingObject7{7, {0.0, 50.0}, {10.0, 12.0}, {-0.2, 0.2}, 0.0};
std::vector<MovingObjectParameter> testMovingObjects{testMovingObject2, testMovingObject3, testMovingObject4, testMovingObject5, testMovingObject6, testMovingObject7};

MovingObjectParameter expectedMovingObject2_a{2, {10.5, 0.0}, {-5.0, 2.0}, {1.8, -2.7}, 0.5};
MovingObjectParameter expectedMovingObject3_a{3, {50.5, -46.0}, {-4.0, 3.0}, {2.0, -3.0}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_a{expectedMovingObject2_a, expectedMovingObject3_a};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_a{expectedMovingObject2_a.id, expectedMovingObject3_a.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_a{expectedMovingObject2_a.id, expectedMovingObject3_a.id};

MovingObjectParameter expectedMovingObject5_b{5, {1.5, 30.0}, {-2.0, 5.0}, {1.7, -2.5}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_b{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_b{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_b{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id};

MovingObjectParameter expectedMovingObject6_c{6, {-29.5, 34.0}, {-1.0, 6.0}, {1.7, -2.5}, -0.5};
std::vector<MovingObjectParameter> expectedMovingObjects_c{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b, expectedMovingObject6_c};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_c{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_c{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id};

MovingObjectParameter expectedMovingObject7_d{7, {-99.5, -50.0}, {0.0, 7.0}, {1.8, -2.8}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjects_d{expectedMovingObject2_a, expectedMovingObject3_a, expectedMovingObject5_b, expectedMovingObject6_c, expectedMovingObject7_d};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_d{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id, expectedMovingObject7_d.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_d{expectedMovingObject2_a.id, expectedMovingObject3_a.id, expectedMovingObject5_b.id, expectedMovingObject6_c.id, expectedMovingObject7_d.id};

MovingObjectParameter expectedMovingObject2_e{2, {10.0, 0.5}, {-5.0, 2.0}, {1.8, -2.7}, 0.5 - M_PI * 0.5};
MovingObjectParameter expectedMovingObject3_e{3, {50.0, -45.5}, {-4.0, 3.0}, {2.0, -3.0}, -M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjects_e{expectedMovingObject2_e, expectedMovingObject3_e};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_e{expectedMovingObject2_e.id, expectedMovingObject3_e.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_e{expectedMovingObject2_e.id, expectedMovingObject3_e.id};

MovingObjectParameter expectedMovingObject7_f{7, {40.5, -10.0}, {7.0, 0.0}, {-2.8, -1.8}, -M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjects_f{expectedMovingObject7_f};
std::vector<OWL::Id> expectedMovingObjectsVisibleIds_f{expectedMovingObject7_f.id};
std::vector<OWL::Id> expectedMovingObjectsDetectedIds_f{expectedMovingObject7_f.id};

StationaryObjectParameter testStationaryObject12{12, {110.0, 100.0}, 0.5};
StationaryObjectParameter testStationaryObject13{13, {150.0, 54.0}, 0.0};
StationaryObjectParameter testStationaryObject14{14, {130.0, 403.0}, 0.0};
StationaryObjectParameter testStationaryObject15{15, {101.0, 130.0}, 0.0};
StationaryObjectParameter testStationaryObject16{16, {70.0, 134.0}, -0.5};
StationaryObjectParameter testStationaryObject17{17, {0.0, 50.0}, 0.0};
std::vector<StationaryObjectParameter> testStationaryObjects{testStationaryObject12, testStationaryObject13, testStationaryObject14, testStationaryObject15, testStationaryObject16, testStationaryObject17};

StationaryObjectParameter expectedStationaryObject12_a{2, {10.5, 0.0}, 0.5};
StationaryObjectParameter expectedStationaryObject13_a{3, {50.5, -46.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_a{expectedStationaryObject12_a, expectedStationaryObject13_a};
StationaryObjectParameter expectedStationaryObject15_b{5, {1.5, 30.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_b{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b};
StationaryObjectParameter expectedStationaryObject16_c{6, {-29.5, 34.0}, -0.5};
std::vector<StationaryObjectParameter> expectedStationaryObjects_c{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b, expectedStationaryObject16_c};
StationaryObjectParameter expectedStationaryObject17_d{7, {-99.5, -50.0}, 0.0};
std::vector<StationaryObjectParameter> expectedStationaryObjects_d{expectedStationaryObject12_a, expectedStationaryObject13_a, expectedStationaryObject15_b, expectedStationaryObject16_c, expectedStationaryObject17_d};
StationaryObjectParameter expectedStationaryObject12_e{2, {10.0, 0.5}, 0.5 - M_PI * 0.5};
StationaryObjectParameter expectedStationaryObject13_e{3, {50.0, -45.5}, - M_PI * 0.5};
std::vector<StationaryObjectParameter> expectedStationaryObjects_e{expectedStationaryObject12_e, expectedStationaryObject13_e};
StationaryObjectParameter expectedStationaryObject17_f{7, {40.5, -10.0}, -M_PI * 0.5};
std::vector<StationaryObjectParameter> expectedStationaryObjects_f{expectedStationaryObject17_f};

INSTANTIATE_TEST_CASE_P(WithoutObstruction, DetectObjects,
    ::testing::Values(
//                                 openingAngle   lateral   positionX    vehicleYaw                    movingObjects                           expectedMovingObjects
//                         range       | longitudinal|sensorYaw | positionY  |          visualObstruction    |            stationaryObjects              |    expectedStationaryObjects
//                           |         |       |     |   |      |     |      |                  |            |                   |                       |               |
        DetectObjects_Data{0.0  , M_PI * 0.5, 0.0, 0.0,          0.0, 100.0, 100.0, 0.0,       false, testMovingObjects, testStationaryObjects, {},                      {},                          {}, {}}, //zero range => no detected objects
        DetectObjects_Data{300.0, M_PI * 0.5, 0.0, 0.0,         0.0, 100.0, 100.0, 0.0,        false, {}               , {},                    {},                      {},                          {}, {}}, //no other objects beside host
        DetectObjects_Data{300.0, M_PI * 0.5, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_a, expectedStationaryObjects_a, expectedMovingObjectsVisibleIds_a, expectedMovingObjectsDetectedIds_a}, //opening angle small
        DetectObjects_Data{300.0, M_PI      , 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_b, expectedStationaryObjects_b, expectedMovingObjectsVisibleIds_b, expectedMovingObjectsDetectedIds_b}, //half circle
        DetectObjects_Data{300.0, M_PI * 1.5, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_c, expectedStationaryObjects_c, expectedMovingObjectsVisibleIds_c, expectedMovingObjectsDetectedIds_c}, //opening angle big
        DetectObjects_Data{300.0, M_PI * 2.0, 2.0, 3.0,         0.0, 100.0, 100.0, 0.0,        false, testMovingObjects, testStationaryObjects, expectedMovingObjects_d, expectedStationaryObjects_d, expectedMovingObjectsVisibleIds_d, expectedMovingObjectsDetectedIds_d}, //full circle
        DetectObjects_Data{300.0, M_PI * 0.5, 2.0, 3.0, -M_PI * 0.5, 100.0, 100.0, M_PI * 0.5, false, testMovingObjects, testStationaryObjects, expectedMovingObjects_e, expectedStationaryObjects_e, expectedMovingObjectsVisibleIds_e, expectedMovingObjectsDetectedIds_e}, //vehicle is rotated
        DetectObjects_Data{30.5, M_PI * 0.5, 10.0, -10.0,       0.0, -10.0, 10.0, M_PI * 0.5, false, testMovingObjects, testStationaryObjects, expectedMovingObjects_f, expectedStationaryObjects_f, expectedMovingObjectsVisibleIds_f, expectedMovingObjectsDetectedIds_f}) //vehicle is pointing upwards, test correct sensorPosition
);

//Obstruction Tests (RequiredPercentageOfVisibleArea = 40%)

//Test A
//Object 3 is behind Object 2, Object 5 is only partially inside detection field
MovingObjectParameter testMovingObjectObstruction2{2, {50.0, 0.0}, {5.0, 7.0}, {-0.2, 0.3}, 0.0};
MovingObjectParameter testMovingObjectObstruction3{3, {100.0, 0.0}, {6.0, 8.0}, {-0.3, 0.4}, 0.0};// shadowed by 2
MovingObjectParameter testMovingObjectObstruction4{4, {50.0, 10.0}, {7.0, 9.0}, {-0.4, 0.5}, 0.0};
MovingObjectParameter testMovingObjectObstruction5{5, {50.0, 51.0}, {8.0, 10.0}, {-0.4, 0.5}, 0.0};// only 30% inside detection field
std::vector<MovingObjectParameter> testMovingObjectsObstruction_a{testMovingObjectObstruction2, testMovingObjectObstruction3, testMovingObjectObstruction4, testMovingObjectObstruction5};
MovingObjectParameter expectedMovingObjectObstruction2{2, {50.5, 0.0}, {-5.0, 2.0}, {1.8, -2.7}, 0.0};
MovingObjectParameter expectedMovingObjectObstruction4{4, {50.5, 10.0}, {-3.0, 4.0}, {1.6, -2.5}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_a{expectedMovingObjectObstruction2, expectedMovingObjectObstruction4};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_a{testMovingObjectObstruction2.id, testMovingObjectObstruction4.id, testMovingObjectObstruction5.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_a{testMovingObjectObstruction2.id, testMovingObjectObstruction4.id};

//Test B
//Like Test A but with StationaryObjects as Objects 2 and 5
StationaryObjectParameter testStationaryObjectObstruction2{2, {50.0, 0.0}, 0.0};
StationaryObjectParameter testStationaryObjectObstruction5{4, {50.0, 51.0}, 0.0};// only 30% inside detection field
std::vector<MovingObjectParameter> testMovingObjectsObstruction_b{testMovingObjectObstruction3, testMovingObjectObstruction4};
std::vector<StationaryObjectParameter> testStationaryObjectsObstruction_b{testStationaryObjectObstruction2, testStationaryObjectObstruction5};
StationaryObjectParameter expectedStationaryObjectObstruction2{2, {50.5, 0.0}, 0.0};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_b{ expectedMovingObjectObstruction4};
std::vector<StationaryObjectParameter> expectedStationaryObjectsObstruction_b{expectedStationaryObjectObstruction2};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_b{testMovingObjectObstruction4.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_b{testMovingObjectObstruction4.id};

//Test C
//Objects rotated; Object 7 is 50% shadowed by Object 6
MovingObjectParameter testMovingObjectObstruction6{6, {50.0, 2.5}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction7{7, {70.0, 0.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_c{testMovingObjectObstruction6, testMovingObjectObstruction7};
MovingObjectParameter expectedMovingObjectObstruction6{6, {50.5, 2.5}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
MovingObjectParameter expectedMovingObjectObstruction7{7, {70.5, 0.0}, {-4.0, 3.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_c{expectedMovingObjectObstruction6, expectedMovingObjectObstruction7};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_c{testMovingObjectObstruction6.id, testMovingObjectObstruction7.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_c{testMovingObjectObstruction6.id, testMovingObjectObstruction7.id};

//Test D
//Objects rotated and half circle; Object 9 is 70% shadowed by Object 8
MovingObjectParameter testMovingObjectObstruction8{8, {11.0, 7.5}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction9{9, {101.0, 100.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_d{testMovingObjectObstruction8, testMovingObjectObstruction9};
MovingObjectParameter expectedMovingObjectObstruction8{8, {11.5, 7.5}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_d{expectedMovingObjectObstruction8};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_d{testMovingObjectObstruction8.id, testMovingObjectObstruction9.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_d{testMovingObjectObstruction8.id};

//Test E
//Objects 10 and 11 partially shadow Object 12 with less then 40% but together with more than 70%
MovingObjectParameter testMovingObjectObstruction10{10, {97.0, 3.0}, {5.0, 7.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction11{11, {97.0, -3.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
MovingObjectParameter testMovingObjectObstruction12{12, {100.0, 0.0}, {6.0, 8.0}, {-0.2, 0.3}, M_PI * 0.5};
std::vector<MovingObjectParameter> testMovingObjectsObstruction_e{testMovingObjectObstruction10, testMovingObjectObstruction11, testMovingObjectObstruction12};
MovingObjectParameter expectedMovingObjectObstruction10{10, {97.5, 3.0}, {-5.0, 2.0}, {1.8, -2.7}, M_PI * 0.5};
MovingObjectParameter expectedMovingObjectObstruction11{11, {97.5, -3.0}, {-4.0, 3.0}, {1.8, -2.7}, M_PI * 0.5};
std::vector<MovingObjectParameter> expectedMovingObjectsObstruction_e{expectedMovingObjectObstruction10, expectedMovingObjectObstruction11};
std::vector<OWL::Id> expectedVisibleMovingObjectIdsObstruction_e{testMovingObjectObstruction10.id, testMovingObjectObstruction11.id, testMovingObjectObstruction12.id};
std::vector<OWL::Id> expectedDetectedMovingObjectIdsObstruction_e{testMovingObjectObstruction10.id, testMovingObjectObstruction11.id};

INSTANTIATE_TEST_CASE_P(WithObstruction, DetectObjects,
    ::testing::Values(
//                                openingAngle   lateral   positionX vehicleYaw           movingObjects                                      expectedMovingObjects
//                         range      | longitudinal|sensorYaw|position | visualObstruction    |            stationaryObjects                         |                       expectedStationaryObjects
//                           |        |        |    |    |    |    |    |    |                 |                   |                                  |                                    |
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, {}                            , {},                                 {},                                 {}, {}, {}}, //no other objects beside host
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_a, {},                                 expectedMovingObjectsObstruction_a, {}, expectedVisibleMovingObjectIdsObstruction_a, expectedDetectedMovingObjectIdsObstruction_a}, //Test A
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_b, testStationaryObjectsObstruction_b, expectedMovingObjectsObstruction_b, expectedStationaryObjectsObstruction_b, expectedVisibleMovingObjectIdsObstruction_b, expectedDetectedMovingObjectIdsObstruction_b}, //Test B
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_c, {},                                 expectedMovingObjectsObstruction_c, {}, expectedVisibleMovingObjectIdsObstruction_c, expectedDetectedMovingObjectIdsObstruction_c}, //Test C
        DetectObjects_Data{300.0, M_PI * 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_d, {},                                 expectedMovingObjectsObstruction_d, {}, expectedVisibleMovingObjectIdsObstruction_d, expectedDetectedMovingObjectIdsObstruction_d}, //Test D
        DetectObjects_Data{300.0, M_PI * 0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, true, testMovingObjectsObstruction_e, {},                                 expectedMovingObjectsObstruction_e, {}, expectedVisibleMovingObjectIdsObstruction_e, expectedDetectedMovingObjectIdsObstruction_e}) //Test E
);
