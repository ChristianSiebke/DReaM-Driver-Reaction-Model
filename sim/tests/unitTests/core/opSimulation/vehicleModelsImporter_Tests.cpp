/********************************************************************************
 * Copyright (c) 2020 in-tech GmbH
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
        "<Performance maxSpeed=\"22.0\" maxAcceleration=\"10.0\" maxDeceleration=\"23.0\"/>"
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
        "<Performance maxSpeed=\"$CustomSpeed\" maxAcceleration=\"10.0\" maxDeceleration=\"23.0\"/>"
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

    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.length, DoubleEq(2.0));
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.width, DoubleEq(1.9));
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.height, DoubleEq(2.1));
    EXPECT_THAT(vehicleModelParameters.boundingBoxCenter.x, DoubleEq(1.6));
    EXPECT_THAT(vehicleModelParameters.boundingBoxCenter.y, DoubleEq(1.7));
    EXPECT_THAT(vehicleModelParameters.boundingBoxCenter.z, DoubleEq(1.8));
    EXPECT_THAT(vehicleModelParameters.performance.maxSpeed, DoubleEq(22.0));
    EXPECT_THAT(vehicleModelParameters.performance.maxDeceleration, DoubleEq(23.0));
    EXPECT_THAT(vehicleModelParameters.frontAxle.maxSteering, DoubleEq(1.1));
    EXPECT_THAT(vehicleModelParameters.frontAxle.wheelDiameter, DoubleEq(26.0));
    EXPECT_THAT(vehicleModelParameters.frontAxle.trackWidth, DoubleEq(27.));
    EXPECT_THAT(vehicleModelParameters.frontAxle.positionX, DoubleEq(28.));
    EXPECT_THAT(vehicleModelParameters.frontAxle.positionZ, DoubleEq(29.));
    EXPECT_THAT(vehicleModelParameters.rearAxle.maxSteering, DoubleEq(0.0));
    EXPECT_THAT(vehicleModelParameters.rearAxle.wheelDiameter, DoubleEq(31.0));
    EXPECT_THAT(vehicleModelParameters.rearAxle.trackWidth, DoubleEq(32.));
    EXPECT_THAT(vehicleModelParameters.rearAxle.positionX, DoubleEq(0.));
    EXPECT_THAT(vehicleModelParameters.rearAxle.positionZ, DoubleEq(34.));
}

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectEngineParameters)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.properties.at("DecelerationFromPowertrainDrag"), DoubleEq(3.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MinimumEngineTorque"), DoubleEq(7.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MaximumEngineTorque"), DoubleEq(8.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MinimumEngineSpeed"), DoubleEq(9.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MaximumEngineSpeed"), DoubleEq(10.0));
}

TEST(VehicleModelsImporter, GivenValidGeometry_ImportsCorrectDynamics)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.properties.at("SteeringRatio"), DoubleEq(15.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("AxleRatio"), DoubleEq(2.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("AirDragCoefficient"), DoubleEq(1.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("FrictionCoefficient"), DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("FrontSurface"), DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("Mass"), DoubleEq(24.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MomentInertiaRoll"), DoubleEq(11.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MomentInertiaPitch"), DoubleEq(12.0));
    EXPECT_THAT(vehicleModelParameters.properties.at("MomentInertiaYaw"), DoubleEq(13.0));
}

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectGearing)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar").Get(EMPTY_PARAMETERS));

    EXPECT_THAT(vehicleModelParameters.properties.at("NumberOfGears"), DoubleEq(3));
    EXPECT_THAT(vehicleModelParameters.properties.at("GearRatio1"), DoubleEq(6));
    EXPECT_THAT(vehicleModelParameters.properties.at("GearRatio2"), DoubleEq(6.1));
    EXPECT_THAT(vehicleModelParameters.properties.at("GearRatio3"), DoubleEq(6.2));
}

TEST(VehicleModelsImporter, GivenParametrizedVehicle_ImportsCorrectParameters)
{
    QDomElement fakeVehicleRoot = documentRootFromString(parametrizedVehicleString);

    VehicleModelMap vehicleModelMap;
    ParametrizedVehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.length.name, Eq("CustomLength"));
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.length.defaultValue, DoubleEq(3.0));
    EXPECT_THAT(vehicleModelParameters.performance.maxSpeed.name, Eq("CustomSpeed"));
    EXPECT_THAT(vehicleModelParameters.performance.maxSpeed.defaultValue, DoubleEq(30.0));
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
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.width, DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.length, DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.boundingBoxDimensions.height, DoubleEq(6.0));
}
