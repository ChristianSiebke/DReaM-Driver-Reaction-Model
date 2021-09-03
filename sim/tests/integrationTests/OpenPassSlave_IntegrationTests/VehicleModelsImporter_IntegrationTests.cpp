/*********************************************************************
* Copyright (c) 2017, 2018, 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <boost/filesystem.hpp>

#include "importer/vehicleModels.h"
#include "importer/vehicleModelsImporter.h"

using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Eq;

TEST(VehicleModelImporter, GivenVehicleAndPedestrianCatalogs_ImportsAllModels)
{
    boost::filesystem::path catalogsDirectory = boost::filesystem::initial_path() / "Resources" / "ImporterTest";

    Configuration::VehicleModels vehicleModels;

    ASSERT_TRUE(Importer::VehicleModelsImporter::Import(
                    catalogsDirectory.string() + "/VehicleModelsCatalog.xosc",
                    catalogsDirectory.string() + "/PedestrianModelsCatalog.xosc",
                    vehicleModels));

    auto vehicleModel1 = vehicleModels.GetVehicleModel("car_one");
    EXPECT_THAT(vehicleModel1.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(vehicleModel1.boundingBoxCenter.x, DoubleEq(1.0));
    EXPECT_THAT(vehicleModel1.boundingBoxCenter.y, DoubleEq(0));
    EXPECT_THAT(vehicleModel1.boundingBoxCenter.z, DoubleEq(1.1));
    EXPECT_THAT(vehicleModel1.boundingBoxDimensions.width, DoubleEq(1.0));
    EXPECT_THAT(vehicleModel1.boundingBoxDimensions.length, DoubleEq(4.0));
    EXPECT_THAT(vehicleModel1.boundingBoxDimensions.height, DoubleEq(2.2));
    EXPECT_THAT(vehicleModel1.performance.maxSpeed, DoubleEq(10.0));
    EXPECT_THAT(vehicleModel1.performance.maxAcceleration, DoubleEq(10.0));
    EXPECT_THAT(vehicleModel1.performance.maxDeceleration, DoubleEq(9.8));
    EXPECT_THAT(vehicleModel1.frontAxle.maxSteering, DoubleEq(0.707));
    EXPECT_THAT(vehicleModel1.frontAxle.wheelDiameter, DoubleEq(0.5));
    EXPECT_THAT(vehicleModel1.frontAxle.trackWidth, DoubleEq(1.0));
    EXPECT_THAT(vehicleModel1.frontAxle.positionX, DoubleEq(2.0));
    EXPECT_THAT(vehicleModel1.frontAxle.positionZ, DoubleEq(0.25));
    EXPECT_THAT(vehicleModel1.rearAxle.maxSteering, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel1.rearAxle.wheelDiameter, DoubleEq(0.5));
    EXPECT_THAT(vehicleModel1.rearAxle.trackWidth, DoubleEq(1.0));
    EXPECT_THAT(vehicleModel1.rearAxle.positionX, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel1.rearAxle.positionZ, DoubleEq(0.25));
    EXPECT_THAT(vehicleModel1.properties, ElementsAre(
                    std::make_pair("AirDragCoefficient", 1.1),
                    std::make_pair("AxleRatio", 2.2),
                    std::make_pair("DecelerationFromPowertrainDrag", 3.3),
                    std::make_pair("FrictionCoefficient", 4.4),
                    std::make_pair("FrontSurface", 5.5),
                    std::make_pair("GearRatio1", 6.6),
                    std::make_pair("Mass", 1000.0),
                    std::make_pair("MaximumEngineSpeed", 6000.),
                    std::make_pair("MaximumEngineTorque", 250.),
                    std::make_pair("MinimumEngineSpeed", 900.),
                    std::make_pair("MinimumEngineTorque", -54.),
                    std::make_pair("MomentInertiaPitch", 0.0),
                    std::make_pair("MomentInertiaRoll", 0.0),
                    std::make_pair("MomentInertiaYaw", 0.0),
                    std::make_pair("NumberOfGears", 1.),
                    std::make_pair("SteeringRatio", 7.7)));

    auto vehicleModel2 = vehicleModels.GetVehicleModel("car_two");
    EXPECT_THAT(vehicleModel2.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(vehicleModel2.boundingBoxCenter.x, DoubleEq(2.0));
    EXPECT_THAT(vehicleModel2.boundingBoxCenter.y, DoubleEq(0));
    EXPECT_THAT(vehicleModel2.boundingBoxCenter.z, DoubleEq(2.2));
    EXPECT_THAT(vehicleModel2.boundingBoxDimensions.width, DoubleEq(1.5));
    EXPECT_THAT(vehicleModel2.boundingBoxDimensions.length, DoubleEq(3.0));
    EXPECT_THAT(vehicleModel2.boundingBoxDimensions.height, DoubleEq(4.4));
    EXPECT_THAT(vehicleModel2.performance.maxSpeed, DoubleEq(11.0));
    EXPECT_THAT(vehicleModel2.performance.maxAcceleration, DoubleEq(10.0));
    EXPECT_THAT(vehicleModel2.performance.maxDeceleration, DoubleEq(4.5));
    EXPECT_THAT(vehicleModel2.frontAxle.maxSteering, DoubleEq(0.5));
    EXPECT_THAT(vehicleModel2.frontAxle.wheelDiameter, DoubleEq(0.4));
    EXPECT_THAT(vehicleModel2.frontAxle.trackWidth, DoubleEq(1.1));
    EXPECT_THAT(vehicleModel2.frontAxle.positionX, DoubleEq(2.5));
    EXPECT_THAT(vehicleModel2.frontAxle.positionZ, DoubleEq(0.2));
    EXPECT_THAT(vehicleModel2.rearAxle.maxSteering, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel2.rearAxle.wheelDiameter, DoubleEq(0.4));
    EXPECT_THAT(vehicleModel2.rearAxle.trackWidth, DoubleEq(1.1));
    EXPECT_THAT(vehicleModel2.rearAxle.positionX, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel2.rearAxle.positionZ, DoubleEq(0.2));
    EXPECT_THAT(vehicleModel2.properties, ElementsAre(
                    std::make_pair("AirDragCoefficient", 2.2),
                    std::make_pair("AxleRatio", 3.3),
                    std::make_pair("DecelerationFromPowertrainDrag", 4.4),
                    std::make_pair("FrictionCoefficient", 5.5),
                    std::make_pair("FrontSurface", 6.6),
                    std::make_pair("GearRatio1", 7.7),
                    std::make_pair("GearRatio2", 8.8),
                    std::make_pair("Mass", 999.9),
                    std::make_pair("MaximumEngineSpeed", 6000.),
                    std::make_pair("MaximumEngineTorque", 250.),
                    std::make_pair("MinimumEngineSpeed", 900.),
                    std::make_pair("MinimumEngineTorque", -54.),
                    std::make_pair("MomentInertiaPitch", 0.0),
                    std::make_pair("MomentInertiaRoll", 0.0),
                    std::make_pair("MomentInertiaYaw", 0.0),
                    std::make_pair("NumberOfGears", 2.),
                    std::make_pair("SteeringRatio", 9.9)));

    auto pedestrianModel1 = vehicleModels.GetVehicleModel("pedestrian_one");
    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(pedestrianModel1.boundingBoxDimensions.length, DoubleEq(5.5));
    EXPECT_THAT(pedestrianModel1.boundingBoxDimensions.width, DoubleEq(4.4));
    EXPECT_THAT(pedestrianModel1.boundingBoxDimensions.height, DoubleEq(6.6));

    auto pedestrianModel2 = vehicleModels.GetVehicleModel("pedestrian_two");
    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(pedestrianModel2.boundingBoxDimensions.length, DoubleEq(2.2));
    EXPECT_THAT(pedestrianModel2.boundingBoxDimensions.width, DoubleEq(3.3));
    EXPECT_THAT(pedestrianModel2.boundingBoxDimensions.height, DoubleEq(1.1));
}

