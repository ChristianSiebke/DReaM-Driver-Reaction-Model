#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cmath>

#include "common/helper/importerHelper.h"
#include "vehicleModelsImporter.h"

using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Eq;

using namespace Importer;

const std::string validVehicleString{
    "<Vehicle name=\"validCar\" category=\"car\">"
        "<ParameterDeclaration>"
            "<Parameter name=\"AirDragCoefficient\" type=\"double\" value=\"1.0\"/>"
            "<Parameter name=\"AxleRatio\" type=\"double\" value=\"2.0\"/>"
            "<Parameter name=\"DecelerationFromPowertrainDrag\" type=\"double\" value=\"3.0\"/>"
            "<Parameter name=\"FrictionCoefficient\" type=\"double\" value=\"4.0\"/>"
            "<Parameter name=\"FrontSurface\" type=\"double\" value=\"5.0\"/>"
            "<Parameter name=\"GearRatio1\" type=\"double\" value=\"6.0\"/>"
            "<Parameter name=\"GearRatio2\" type=\"double\" value=\"6.1\"/>"
            "<Parameter name=\"GearRatio3\" type=\"double\" value=\"6.2\"/>"
            "<Parameter name=\"MinimumEngineTorque\" type=\"double\" value=\"7.0\"/>"
            "<Parameter name=\"MaximumEngineTorque\" type=\"double\" value=\"8.0\"/>"
            "<Parameter name=\"MinimumEngineSpeed\" type=\"double\" value=\"9.0\"/>"
            "<Parameter name=\"MaximumEngineSpeed\" type=\"double\" value=\"10.0\"/>"
            "<Parameter name=\"MomentInertiaRoll\" type=\"double\" value=\"11.0\"/>"
            "<Parameter name=\"MomentInertiaPitch\" type=\"double\" value=\"12.0\"/>"
            "<Parameter name=\"MomentInertiaYaw\" type=\"double\" value=\"13.0\"/>"
            "<Parameter name=\"NumberOfGears\" type=\"integer\" value=\"3\"/>"
            "<Parameter name=\"SteeringRatio\" type=\"double\" value=\"15.0\"/>"
        "</ParameterDeclaration>"
        "<BoundingBox>"
            "<Center x=\"1.6\" y=\"1.7\" z=\"1.8\"/>"
            "<Dimension width=\"1.9\" length=\"2.0\" height=\"2.1\"/>"
        "</BoundingBox>"
        "<Performance maxSpeed=\"22.0\" maxDeceleration=\"23.0\" mass=\"24.0\"/>"
        "<Axles>"
            "<Front maxSteering=\"1.1\" wheelDiameter=\"26.0\" trackWidth=\"27.0\" positionX=\"28.0\" positionZ=\"29.0\"/>"
            "<Rear maxSteering=\"0.0\" wheelDiameter=\"31.0\" trackWidth=\"32.0\" positionX=\"0.0\" positionZ=\"34.0\"/>"
        "</Axles>"
    "</Vehicle>"
};

const std::string validPedestrianString{
    "<Pedestrian name=\"pedestrian_name\" model=\"pedestrian_model\" category=\"pedestrian\" mass=\"7.0\">"
        "<ParameterDeclaration/>"
        "<BoundingBox>"
            "<Center x=\"1.0\" y=\"2.0\" z=\"3.0\"/>"
            "<Dimension width=\"4.0\" length=\"5.0\" height=\"6.0\"/>"
        "</BoundingBox>"
        "<Properties/>"
    "</Pedestrian>"
};

const std::string unknownVehicleTypeString{
    "<NotAWellKnownType name=\"invalidCar\" category=\"unknownType\">"
        "<ParameterDeclaration/>"
        "<BoundingBox/>"
        "<Axles>"
            "<Front maxSteering=\"1.1\" wheelDiameter=\"26.0\" trackWidth=\"27.0\" positionX=\"28.0\" positionZ=\"29.0\"/>"
            "<Rear maxSteering=\"0.0\" wheelDiameter=\"31.0\" trackWidth=\"32.0\" positionX=\"0.0\" positionZ=\"34.0\"/>"
        "</Axles>"
    "</NotAWellKnownType>"
};

TEST(VehicleModelsImporter, GivenValidVehicle_ImportsCorrectGeometry)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

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
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

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
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.steeringRatio, DoubleEq(15.0));
    EXPECT_THAT(vehicleModelParameters.axleRatio, DoubleEq(2.0));
    EXPECT_THAT(vehicleModelParameters.airDragCoefficient, DoubleEq(1.0));
    EXPECT_THAT(vehicleModelParameters.frictionCoeff, DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.frontSurface, DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.weight, DoubleEq(24.0));
    EXPECT_THAT(vehicleModelParameters.maxVelocity, DoubleEq(22.0));
    EXPECT_THAT(vehicleModelParameters.maximumSteeringWheelAngleAmplitude, DoubleEq(1.1 * 15.0 / M_PI * 180.0));   // maxSteering front wheel * SteeringRatio
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
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.numberOfGears, Eq(3));
    EXPECT_THAT(vehicleModelParameters.gearRatios, ElementsAre(0.0, 6.0, 6.1, 6.2));
}

TEST(VehicleModelsImporter, GivenValidVehicle_SetsCorrectType)
{
    QDomElement fakeVehicleRoot = documentRootFromString(validVehicleString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("validCar"));

    EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Car));
}

TEST(VehicleModelsImporter, GivenUnknwonVehicleType_DoesNotImport)
{
    QDomElement fakeVehicleRoot = documentRootFromString(unknownVehicleTypeString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_THROW(VehicleModelsImporter::ImportVehicleModel(fakeVehicleRoot, vehicleModelMap), std::runtime_error);
    ASSERT_THROW(vehicleModelMap.at("invalidCar"), std::out_of_range);
}

TEST(VehicleModelsImporter, GivenValidPedestrian_ImportsCorrectValues)
{
    QDomElement fakePedestrianRoot = documentRootFromString(validPedestrianString);

    VehicleModelMap vehicleModelMap;
    VehicleModelParameters vehicleModelParameters;

    ASSERT_NO_THROW(VehicleModelsImporter::ImportPedestrianModel(fakePedestrianRoot, vehicleModelMap));
    ASSERT_NO_THROW(vehicleModelParameters = vehicleModelMap.at("pedestrian_name"));

    // workaround for ground truth not being able to handle pedestrians
    //EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Pedestrian));
    EXPECT_THAT(vehicleModelParameters.vehicleType, Eq(AgentVehicleType::Car));
    EXPECT_THAT(vehicleModelParameters.width, DoubleEq(4.0));
    EXPECT_THAT(vehicleModelParameters.length, DoubleEq(5.0));
    EXPECT_THAT(vehicleModelParameters.height, DoubleEq(6.0));
    EXPECT_THAT(vehicleModelParameters.weight, DoubleEq(7.0));
}
