/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cmath>

#include "common/helper/importerHelper.h"
#include "vehicleModelsImporter.h"

using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::SizeIs;
using ::testing::Eq;

using namespace Importer;

const std::string validVehicleString{
    "<Vehicle name=\"validCar\" vehicleCategory=\"car\">"
        "<Properties>"
            "<Property name=\"AirDragCoefficient\" value=\"1.0\"/>"
            "<Property name=\"AxleRatio\" value=\"2.0\"/>"
            "<Property name=\"DecelerationFromPowertrainDrag\" value=\"3.0\"/>"
            "<Property name=\"FrictionCoefficient\" value=\"4.0\"/>"
            "<Property name=\"FrontSurface\" value=\"5.0\"/>"
            "<Property name=\"GearRatio1\" value=\"6.0\"/>"
            "<Property name=\"GearRatio2\" value=\"6.1\"/>"
            "<Property name=\"GearRatio3\" value=\"6.2\"/>"
            "<Property name=\"Mass\" value=\"24.0\"/>"
            "<Property name=\"MinimumEngineTorque\" value=\"7.0\"/>"
            "<Property name=\"MaximumEngineTorque\" value=\"8.0\"/>"
            "<Property name=\"MinimumEngineSpeed\" value=\"9.0\"/>"
            "<Property name=\"MaximumEngineSpeed\" value=\"10.0\"/>"
            "<Property name=\"MomentInertiaRoll\" value=\"11.0\"/>"
            "<Property name=\"MomentInertiaPitch\" value=\"12.0\"/>"
            "<Property name=\"MomentInertiaYaw\" value=\"13.0\"/>"
            "<Property name=\"NumberOfGears\" value=\"3\"/>"
            "<Property name=\"SteeringRatio\" value=\"15.0\"/>"
        "</Properties>"
        "<BoundingBox>"
            "<Center x=\"1.6\" y=\"1.7\" z=\"1.8\"/>"
            "<Dimensions width=\"1.9\" length=\"2.0\" height=\"2.1\"/>"
        "</BoundingBox>"
        "<Performance maxSpeed=\"22.0\" maxDeceleration=\"23.0\"/>"
        "<Axles>"
            "<FrontAxle maxSteering=\"1.1\" wheelDiameter=\"26.0\" trackWidth=\"27.0\" positionX=\"28.0\" positionZ=\"29.0\"/>"
            "<RearAxle maxSteering=\"0.0\" wheelDiameter=\"31.0\" trackWidth=\"32.0\" positionX=\"0.0\" positionZ=\"34.0\"/>"
        "</Axles>"
    "</Vehicle>"
};

const std::string parametrizedVehicleString{
    "<Vehicle name=\"validCar\" vehicleCategory=\"car\">"
        "<Properties>"
            "<Property name=\"AirDragCoefficient\" value=\"1.0\"/>"
            "<Property name=\"AxleRatio\" value=\"2.0\"/>"
            "<Property name=\"DecelerationFromPowertrainDrag\" value=\"3.0\"/>"
            "<Property name=\"FrictionCoefficient\" value=\"4.0\"/>"
            "<Property name=\"FrontSurface\" value=\"5.0\"/>"
            "<Property name=\"GearRatio1\" value=\"6.0\"/>"
            "<Property name=\"GearRatio2\" value=\"6.1\"/>"
            "<Property name=\"GearRatio3\" value=\"6.2\"/>"
            "<Property name=\"Mass\" value=\"24.0\"/>"
            "<Property name=\"MinimumEngineTorque\" value=\"7.0\"/>"
            "<Property name=\"MaximumEngineTorque\" value=\"8.0\"/>"
            "<Property name=\"MinimumEngineSpeed\" value=\"9.0\"/>"
            "<Property name=\"MaximumEngineSpeed\" value=\"10.0\"/>"
            "<Property name=\"MomentInertiaRoll\" value=\"11.0\"/>"
            "<Property name=\"MomentInertiaPitch\" value=\"12.0\"/>"
            "<Property name=\"MomentInertiaYaw\" value=\"13.0\"/>"
            "<Property name=\"NumberOfGears\" value=\"3\"/>"
            "<Property name=\"SteeringRatio\" value=\"15.0\"/>"
            "<Property name=\"CustomX\" value=\"1.5\"/>"
            "<Property name=\"CustomLength\" value=\"3.0\"/>"
            "<Property name=\"CustomSpeed\" value=\"30.0\"/>"
            "<Property name=\"CustomFrontTrackWidth\" value=\"1.9\"/>"
        "</Properties>"
        "<ParameterDeclarations>"
            "<ParameterDeclaration name=\"CustomX\" parameterType=\"double\" value=\"1.6\"/>"
            "<ParameterDeclaration name=\"CustomLength\" parameterType=\"double\" value=\"3.0\"/>"
            "<ParameterDeclaration name=\"CustomSpeed\" parameterType=\"double\" value=\"30.0\"/>"
            "<ParameterDeclaration name=\"CustomFrontTrackWidth\" parameterType=\"double\" value=\"1.9\"/>"
        "</ParameterDeclarations>"
        "<BoundingBox>"
            "<Center x=\"$CustomX\" y=\"1.7\" z=\"1.8\"/>"
            "<Dimensions width=\"1.9\" length=\"$CustomLength\" height=\"2.1\"/>"
        "</BoundingBox>"
        "<Performance maxSpeed=\"$CustomSpeed\" maxDeceleration=\"23.0\"/>"
        "<Axles>"
            "<FrontAxle maxSteering=\"1.1\" wheelDiameter=\"26.0\" trackWidth=\"$CustomFrontTrackWidth\" positionX=\"28.0\" positionZ=\"29.0\"/>"
            "<RearAxle maxSteering=\"0.0\" wheelDiameter=\"31.0\" trackWidth=\"32.0\" positionX=\"0.0\" positionZ=\"34.0\"/>"
        "</Axles>"
    "</Vehicle>"
};

const std::string validPedestrianString{
    "<Pedestrian name=\"pedestrian_name\" model=\"pedestrian_model\" pedestrianCategory=\"pedestrian\" mass=\"7.0\">"
        "<BoundingBox>"
            "<Center x=\"1.0\" y=\"2.0\" z=\"3.0\"/>"
            "<Dimensions width=\"4.0\" length=\"5.0\" height=\"6.0\"/>"
        "</BoundingBox>"
        "<Properties/>"
    "</Pedestrian>"
};

const std::string unknownVehicleTypeString{
    "<NotAWellKnownType name=\"invalidCar\" vehicleCategory=\"unknownType\">"
        "<ParameterDeclaration/>"
        "<BoundingBox/>"
        "<Axles>"
            "<Front maxSteering=\"1.1\" wheelDiameter=\"26.0\" trackWidth=\"27.0\" positionX=\"28.0\" positionZ=\"29.0\"/>"
            "<Rear maxSteering=\"0.0\" wheelDiameter=\"31.0\" trackWidth=\"32.0\" positionX=\"0.0\" positionZ=\"34.0\"/>"
        "</Axles>"
    "</NotAWellKnownType>"
};

const openScenario::Parameters EMPTY_PARAMETERS{};

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectGeometry)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.length, DoubleEq(2.0));
    EXPECT_THAT(vehicleModelParameters.width, DoubleEq(1.9));
    EXPECT_THAT(vehicleModelParameters.height, DoubleEq(2.1));
    EXPECT_THAT(vehicleModelParameters.trackwidth, DoubleEq(32.0));
    EXPECT_THAT(vehicleModelParameters.wheelbase, DoubleEq(28.0));
    EXPECT_THAT(vehicleModelParameters.heightCOG, DoubleEq(0.0));
    EXPECT_THAT(vehicleModelParameters.distanceReferencePointToFrontAxle, DoubleEq(28.0));
    EXPECT_THAT(vehicleModelParameters.distanceReferencePointToLeadingEdge, DoubleEq(2.6));     // BB center x + length / 2
    EXPECT_THAT(vehicleModelParameters.staticWheelRadius, DoubleEq(15.5));
}

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectEngineParameters)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.decelerationFromPowertrainDrag, DoubleEq(3.0));
    EXPECT_THAT(vehicleModelParameters.minimumEngineTorque, DoubleEq(7.0));
    EXPECT_THAT(vehicleModelParameters.maximumEngineTorque, DoubleEq(8.0));
    EXPECT_THAT(vehicleModelParameters.minimumEngineSpeed, DoubleEq(9.0));
    EXPECT_THAT(vehicleModelParameters.maximumEngineSpeed, DoubleEq(10.0));
}

TEST(VehicleModelsImporter, GivenValidGeometry_ImportsCorrectDynamics)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.steeringRatio, DoubleEq(15.0));
    EXPECT_THAT(vehicleModelParameters.axleRatio, DoubleEq(2.0));
    EXPECT_THAT(vehicleModelParameters.airDragCoefficient, DoubleEq(1.0));
    EXPECT_THAT(vehicleModelParameters.frictionCoeff, DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.frontSurface, DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.weight, DoubleEq(24.0));
    EXPECT_THAT(vehicleModelParameters.maxVelocity, DoubleEq(22.0));
    EXPECT_THAT(vehicleModelParameters.maximumSteeringWheelAngleAmplitude, DoubleEq(1.1 * 15.0));   // maxSteering front wheel * SteeringRatio
    EXPECT_THAT(vehicleModelParameters.maxCurvature, DoubleEq(std::sin(1.1) / 28.0));   // sin(maxSteering fron wheel) / wheelbase
    EXPECT_THAT(vehicleModelParameters.momentInertiaRoll, DoubleEq(11.0));
    EXPECT_THAT(vehicleModelParameters.momentInertiaPitch, DoubleEq(12.0));
    EXPECT_THAT(vehicleModelParameters.momentInertiaYaw, DoubleEq(13.0));
}

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectGearing)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.numberOfGears, Eq(3));
    EXPECT_THAT(vehicleModelParameters.gearRatios, ElementsAre(0.0, 6.0, 6.1, 6.2));
}

TEST(VehicleModelsImporter, GivenParametrizedVehicle_ImportsCorrectParameters)
{
    QDomElement fakeVehicleRoot = documentRootFromString(parametrizedVehicleString);

    VehicleModelMap vehicleModelMap;
    ParametrizedVehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.length.name, Eq("CustomLength"));
    EXPECT_THAT(vehicleModelParameters.length.defaultValue, DoubleEq(3.0));
    EXPECT_THAT(vehicleModelParameters.maxVelocity.name, Eq("CustomSpeed"));
    EXPECT_THAT(vehicleModelParameters.maxVelocity.defaultValue, DoubleEq(30.0));
    EXPECT_THAT(vehicleModelParameters.frontAxle.trackWidth.name, Eq("CustomFrontTrackWidth"));
    EXPECT_THAT(vehicleModelParameters.frontAxle.trackWidth.defaultValue, DoubleEq(1.9));
}

TEST(VehicleModelsImporter, GivenValidVehicle_SetsCorrectType)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    ParametrizedVehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Car));
}

TEST(VehicleModelsImporter, GivenUnknwonVehicleType_DoesNotImport)
{
    QDomElement fakeVehicleRoot = documentRootFromString(unknownVehicleTypeString);

    VehicleModelMap vehicleModelMap;

    ASSERT_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap), std::out_of_range);
}

TEST(VehicleModelsImporter, GivenValidPedestrian_ImportsCorrectValues)
{
    QDomElement fakePedestrianRoot = documentRootFromString(validPedestrianString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportPedestrianModel(fakePedestrianRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("pedestrian_name").Get(EMPTY_PARAMETERS));

    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(vehicleModelParameters.width, DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.length, DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.height, DoubleEq(6.0));
    EXPECT_THAT(vehicleModelParameters.weight, DoubleEq(7.0));
}
