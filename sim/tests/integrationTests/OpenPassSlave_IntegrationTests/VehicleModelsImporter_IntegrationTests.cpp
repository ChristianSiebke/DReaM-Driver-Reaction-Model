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
    EXPECT_THAT(vehicleModel1.airDragCoefficient, DoubleEq(1.1));
    EXPECT_THAT(vehicleModel1.axleRatio, DoubleEq(2.2));
    EXPECT_THAT(vehicleModel1.decelerationFromPowertrainDrag, DoubleEq(3.3));
    EXPECT_THAT(vehicleModel1.distanceReferencePointToFrontAxle, DoubleEq(2.0));
    EXPECT_THAT(vehicleModel1.distanceReferencePointToLeadingEdge, DoubleEq(3.0));
    EXPECT_THAT(vehicleModel1.frictionCoeff, DoubleEq(4.4));
    EXPECT_THAT(vehicleModel1.frontSurface, DoubleEq(5.5));
    EXPECT_THAT(vehicleModel1.gearRatios, ElementsAre(0.0, 6.6));
    EXPECT_THAT(vehicleModel1.height, DoubleEq(2.2));
    EXPECT_THAT(vehicleModel1.heightCOG, DoubleEq(0.0));                         // unsupported parameter, fixed to 0.0
    EXPECT_THAT(vehicleModel1.length, DoubleEq(4.0));
    EXPECT_THAT(vehicleModel1.maxCurvature, DoubleEq(std::sin(0.707) / 2.0));    // sin(maxSteering fron wheel) / wheelbase
    EXPECT_THAT(vehicleModel1.maxVelocity, DoubleEq(10.0));
    EXPECT_THAT(vehicleModel1.maximumEngineSpeed, DoubleEq(6000.0));
    EXPECT_THAT(vehicleModel1.maximumEngineTorque, DoubleEq(250.0));
    EXPECT_THAT(vehicleModel1.maximumSteeringWheelAngleAmplitude,
                DoubleEq(0.707 * 7.7 / M_PI * 180.0));   // maxSteering front wheel * SteeringRatio
    EXPECT_THAT(vehicleModel1.minimumEngineSpeed, DoubleEq(900.0));
    EXPECT_THAT(vehicleModel1.minimumEngineTorque, DoubleEq(-54.0));
    EXPECT_THAT(vehicleModel1.momentInertiaPitch, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel1.momentInertiaRoll, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel1.momentInertiaYaw, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel1.numberOfGears, 1);
    EXPECT_THAT(vehicleModel1.staticWheelRadius, DoubleEq(0.25));
    EXPECT_THAT(vehicleModel1.steeringRatio, DoubleEq(7.7));
    EXPECT_THAT(vehicleModel1.trackwidth, DoubleEq(1.0));
    EXPECT_THAT(vehicleModel1.weight, DoubleEq(1000.0));
    EXPECT_THAT(vehicleModel1.wheelbase, DoubleEq(2.0));
    EXPECT_THAT(vehicleModel1.width, DoubleEq(1.0));

    auto vehicleModel2 = vehicleModels.GetVehicleModel("car_two");
    EXPECT_THAT(vehicleModel2.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(vehicleModel2.airDragCoefficient, DoubleEq(2.2));
    EXPECT_THAT(vehicleModel2.axleRatio, DoubleEq(3.3));
    EXPECT_THAT(vehicleModel2.decelerationFromPowertrainDrag, DoubleEq(4.4));
    EXPECT_THAT(vehicleModel2.distanceReferencePointToFrontAxle, DoubleEq(2.5));
    EXPECT_THAT(vehicleModel2.distanceReferencePointToLeadingEdge, DoubleEq(3.5));
    EXPECT_THAT(vehicleModel2.frictionCoeff, DoubleEq(5.5));
    EXPECT_THAT(vehicleModel2.frontSurface, DoubleEq(6.6));
    EXPECT_THAT(vehicleModel2.gearRatios, ElementsAre(0.0, 7.7, 8.8));
    EXPECT_THAT(vehicleModel2.height, DoubleEq(4.4));
    EXPECT_THAT(vehicleModel2.heightCOG, DoubleEq(0.0));                       // unsupported parameter, fixed to 0.0
    EXPECT_THAT(vehicleModel2.length, DoubleEq(3.0));
    EXPECT_THAT(vehicleModel2.maxCurvature, DoubleEq(std::sin(0.5) / 2.5));    // sin(maxSteering fron wheel) / wheelbase
    EXPECT_THAT(vehicleModel2.maxVelocity, DoubleEq(11.0));
    EXPECT_THAT(vehicleModel2.maximumEngineSpeed, DoubleEq(6000.0));
    EXPECT_THAT(vehicleModel2.maximumEngineTorque, DoubleEq(250.0));
    EXPECT_THAT(vehicleModel2.maximumSteeringWheelAngleAmplitude,
                DoubleEq(0.5 * 9.9 / M_PI * 180.0));   // maxSteering front wheel * SteeringRatio
    EXPECT_THAT(vehicleModel2.minimumEngineSpeed, DoubleEq(900.0));
    EXPECT_THAT(vehicleModel2.minimumEngineTorque, DoubleEq(-54.0));
    EXPECT_THAT(vehicleModel2.momentInertiaPitch, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel2.momentInertiaRoll, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel2.momentInertiaYaw, DoubleEq(0.0));
    EXPECT_THAT(vehicleModel2.numberOfGears, 2);
    EXPECT_THAT(vehicleModel2.staticWheelRadius, DoubleEq(0.2));
    EXPECT_THAT(vehicleModel2.steeringRatio, DoubleEq(9.9));
    EXPECT_THAT(vehicleModel2.trackwidth, DoubleEq(1.1));
    EXPECT_THAT(vehicleModel2.weight, DoubleEq(999.9));
    EXPECT_THAT(vehicleModel2.wheelbase, DoubleEq(2.5));
    EXPECT_THAT(vehicleModel2.width, DoubleEq(1.5));

    auto pedestrianModel1 = vehicleModels.GetVehicleModel("pedestrian_one");
    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(pedestrianModel1.length, DoubleEq(5.5));
    EXPECT_THAT(pedestrianModel1.width, DoubleEq(4.4));
    EXPECT_THAT(pedestrianModel1.height, DoubleEq(6.6));
    EXPECT_THAT(pedestrianModel1.weight, DoubleEq(100.0));

    auto pedestrianModel2 = vehicleModels.GetVehicleModel("pedestrian_two");
    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(pedestrianModel1.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(pedestrianModel2.length, DoubleEq(2.2));
    EXPECT_THAT(pedestrianModel2.width, DoubleEq(3.3));
    EXPECT_THAT(pedestrianModel2.height, DoubleEq(1.1));
    EXPECT_THAT(pedestrianModel2.weight, DoubleEq(111.1));
}

