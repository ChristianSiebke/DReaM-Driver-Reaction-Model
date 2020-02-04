/*********************************************************************
* Copyright (c) 2019 in-tech GmbH on behalf of BMW
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/helper/importerHelper.h"

#include "profilesImporter.h"

using ::testing::Eq;
using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;

using namespace Importer;

TEST(ProfilesImporter_UnitTests, ImportAllVehicleComponentsOfVehicleProfileSuccessful)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<root>"
                    "<Components>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.5\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                    "</Components>"
                "</root>"
                );

    VehicleProfile resultProfiles;

    ASSERT_TRUE(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(fakeDocumentRoot, resultProfiles));

    ASSERT_EQ(resultProfiles.vehicleComponents.size(), (size_t )1);

    VehicleComponent resultVehicleComponent = resultProfiles.vehicleComponents.front();

    ASSERT_EQ (resultVehicleComponent.componentProfiles.size(), (size_t) 2);
    ASSERT_EQ (resultVehicleComponent.sensorLinks.size(), (size_t) 3);
}

TEST(ProfilesImporter_UnitTests, ImportAllVehicleComponentsOfVehicleProfileUnsuccessful)
{
    QDomElement fakeDocumentRootMissingComponentsTag = documentRootFromString(
                "<root>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.3\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                "</root>"
                );

    QDomElement fakeDocumentRootProbabilitySumAboveOne = documentRootFromString(
                "<root>"
                    "<Components>"
                        "<Component Type=\"ComponentA\">"
                            "<Profiles>"
                                "<Profile Name=\"No warning\" Probability=\"0.5\"/>"
                                "<Profile Name=\"Standard\" Probability=\"0.8\"/>"
                            "</Profiles>"
                            "<SensorLinks>"
                                "<SensorLink SensorId=\"0\" InputId=\"Camera\"/>"
                                "<SensorLink SensorId=\"1\" InputId=\"DrivingCorridor\"/>"
                                "<SensorLink SensorId=\"2\" InputId=\"Camera\"/>"
                            "</SensorLinks>"
                        "</Component>"
                    "</Components>"
                "</root>"
                );

    VehicleProfile resultProfiles;

    ASSERT_FALSE(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(fakeDocumentRootMissingComponentsTag, resultProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportAllVehicleComponentsOfVehicleProfile(fakeDocumentRootProbabilitySumAboveOne, resultProfiles));
}

TEST(ProfilesImporter_UnitTests, ImportAllSensorsOfVehicleProfileSuccessful)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                        "<Sensor Id=\"1\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"0.0\" Lateral=\"0.0\" Height=\"0.5\" Pitch=\"0.0\" Yaw=\"0.0\" Roll=\"0.0\"/>"
                            "<Profile Type=\"DrivingCorridor\" Name=\"Camera\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );

    VehicleProfile resultProfiles;

    ASSERT_TRUE(ProfilesImporter::ImportAllSensorsOfVehicleProfile(fakeDocumentRoot, resultProfiles));

    std::list<SensorParameter> resultSensors = resultProfiles.sensors;

    ASSERT_EQ(resultSensors.size(), (size_t ) 2);

    SensorParameter resultSensorParameter = resultSensors.front();
    SensorPosition resultSensorPosition = resultSensorParameter.sensorPosition;
    SensorProfile resultSensorProfile = resultSensorParameter.sensorProfile;

    ASSERT_EQ (resultSensorPosition.name, "Somewhere");
    ASSERT_EQ (resultSensorPosition.longitudinal, 1.0);
    ASSERT_EQ (resultSensorPosition.lateral, 2.0);
    ASSERT_EQ (resultSensorPosition.height, 3.0);
    ASSERT_EQ (resultSensorPosition.pitch, 4.0);
    ASSERT_EQ (resultSensorPosition.yaw, 5.0);
    ASSERT_EQ (resultSensorPosition.roll, 6.0);

    ASSERT_EQ (resultSensorProfile.type, "Geometric2D");
    ASSERT_EQ (resultSensorProfile.name, "Camera");
}

TEST(ProfilesImporter_UnitTests, ImportAllSensorsOfVehicleProfileUnsuccessful)
{
    QDomElement fakeDocumentRootSensorsTagMissing = documentRootFromString(
                "<root>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                        "<Sensor Id=\"1\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"0.0\" Lateral=\"0.0\" Height=\"0.5\" Pitch=\"0.0\" Yaw=\"0.0\" Roll=\"0.0\"/>"
                            "<Profile Type=\"DrivingCorridor\" Name=\"Camera\"/>"
                        "</Sensor>"
                "</root>"
                );

    QDomElement fakeDocumentRootPositionTagMissing = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Profile Type=\"Geometric2D\" Name=\"Camera\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );

    QDomElement fakeDocumentRootProfileTagMissing = documentRootFromString(
                "<root>"
                    "<Sensors>"
                        "<Sensor Id=\"0\">"
                            "<Position Name=\"Somewhere\" Longitudinal=\"1.0\" Lateral=\"2.0\" Height=\"3.0\" Pitch=\"4.0\" Yaw=\"5.0\" Roll=\"6.0\"/>"
                        "</Sensor>"
                    "</Sensors>"
                "</root>"
                );

    VehicleProfile resultProfiles;

    ASSERT_FALSE(ProfilesImporter::ImportAllSensorsOfVehicleProfile(fakeDocumentRootSensorsTagMissing, resultProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportAllSensorsOfVehicleProfile(fakeDocumentRootPositionTagMissing, resultProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportAllSensorsOfVehicleProfile(fakeDocumentRootProfileTagMissing, resultProfiles));
}

TEST(ProfilesImporter_UnitTests, ImportAllSensorProfilesOfSensorProfilesSuccessful)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<root>"
                      "<SensorProfile Name=\"Narrow\" Type=\"Geometric\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                            "<String Key=\"WordA\" Value=\"A\"/>"
                        "</SensorProfile>"
                        "<SensorProfile Name=\"Wide\" Type=\"Lane\">"
                            "<Int Key=\"Number1\" Value=\"2\"/>"
                            "<String Key=\"WordA\" Value=\"B\"/>"
                        "</SensorProfile>"
                "</root>"
                );

    std::list<SensorProfile> resultSensorProfiles;

    ASSERT_TRUE(ProfilesImporter::ImportSensorProfiles(fakeDocumentRoot, resultSensorProfiles));
    ASSERT_EQ(resultSensorProfiles.size(), (size_t ) 2);


    SensorProfile& firstProfile = resultSensorProfiles.front();
    ASSERT_EQ(firstProfile.name, "Narrow");
    ASSERT_EQ(firstProfile.type, "Geometric");
    ASSERT_EQ(firstProfile.parameters->GetParametersInt().at("Number1"), 1);
    ASSERT_EQ(firstProfile.parameters->GetParametersString().at("WordA"), "A");
}


TEST(ProfilesImporter_UnitTests, ImportAllSensorProfilesOfSensorProfilesUnsuccessful)
{
    QDomElement fakeDocumentRootNameTagMissing = documentRootFromString(
                "<root>"
                      "<SensorProfile Type=\"Geometric\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                        "</SensorProfile>"
                "</root>"
                );

    QDomElement fakeDocumentRootTypeTagMissing = documentRootFromString(
                "<root>"
                       "<SensorProfile Name=\"Narrow\">"
                            "<Int Key=\"Number1\" Value=\"1\"/>"
                        "</SensorProfile>"
                "</root>"
                );

    std::list<SensorProfile> resultSensorProfiles;

    ASSERT_FALSE(ProfilesImporter::ImportSensorProfiles(fakeDocumentRootNameTagMissing, resultSensorProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportSensorProfiles(fakeDocumentRootTypeTagMissing, resultSensorProfiles));
}


TEST(ProfilesImporter_UnitTests, ImportAgentProfilesUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingAtLeastOneElement = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<DriverProfiles>"
                            "<DriverProfile Name = \"Regular\" Probability = \"0.5\"/>"
                            "<DriverProfile Name = \"Trajectory\" Probability = \"0.5\"/>"
                        "</DriverProfiles>"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );

    QDomElement fakeDocumentRootWrongProbabilities = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<DriverProfiles>"
                            "<DriverProfile Name = \"Regular\" Probability = \"0.5\"/>"
                            "<DriverProfile Name = \"Trajectory\" Probability = \"0.5\"/>"
                        "</DriverProfiles>"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                            "<ADASProfile Name = \"AEB\" Probability = \"1.0\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                            "<VehicleModel Name = \"Batmobile\" Probability = \"1.5\"/>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );

    QDomElement fakeDocumentRootMissingTag = documentRootFromString(
                "<root>"
                    "<AgentProfile Name = \"Superhero\">"
                        "<ADASProfiles>"
                            "<ADASProfile Name = \"ComponentA\" Probability = \"0.5\"/>"
                            "<ADASProfile Name = \"AEB\" Probability = \"1.0\"/>"
                        "</ADASProfiles>"
                        "<VehicleModels>"
                            "<VehicleModel Name = \"Batmobile\" Probability = \"1.0\"/>"
                        "</VehicleModels>"
                    "</AgentProfile>"
                "</root>"
                );

    std::unordered_map<std::string, AgentProfile> agentProfiles {};

    ASSERT_FALSE(ProfilesImporter::ImportAgentProfiles(fakeDocumentRootMissingAtLeastOneElement, agentProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportAgentProfiles(fakeDocumentRootWrongProbabilities, agentProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportAgentProfiles(fakeDocumentRootMissingTag, agentProfiles));
}

TEST(ProfilesImporter_UnitTests, ImportDriverProfilesSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"TrajectoryFollower\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );

    DriverProfiles driverProfiles;

    ASSERT_TRUE(ProfilesImporter::ImportDriverProfiles(fakeDocumentRoot, driverProfiles));

    ASSERT_EQ(driverProfiles.size(), (size_t) 2);

    auto resultParameter1 = driverProfiles.at("Regular");
    auto resultParameter2 = driverProfiles.at("TrajectoryFollower");

    ASSERT_EQ(resultParameter1->GetParametersString().size(), (size_t) 1);
    ASSERT_EQ(resultParameter2->GetParametersString().size(), (size_t) 2);
}

TEST(ProfilesImporter_UnitTests, ImportDriverProfilesUnsuccessfully)
{
    QDomElement fakeDocumentRootWithoutType = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"TrajectoryFollower\">"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );

    QDomElement fakeDocumentRootWithDuplicateNames = documentRootFromString(
                "<root>"
                    "<DriverProfile Name = \"Regular\">"
                        "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                    "</DriverProfile>"
                    "<DriverProfile Name = \"Regular\">"
                    "<String Key=\"Type\" Value=\"MyDriverModule\"/>"
                        "<String Key=\"TrajectoryFile\" Value=\"Trajectory.csv\"/>"
                    "</DriverProfile>"
                "</root>"
                );

    DriverProfiles driverProfiles;

    ASSERT_FALSE(ProfilesImporter::ImportDriverProfiles(fakeDocumentRootWithoutType, driverProfiles));
    ASSERT_FALSE(ProfilesImporter::ImportDriverProfiles(fakeDocumentRootWithDuplicateNames, driverProfiles));

}

TEST(ProfilesImporter_UnitTests, ImportAllVehicleComponentProfilesSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                "<root>"
                    "<VehicleComponentProfile Type=\"ComponentA\" Name = \"ComponentAProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentA\" Name = \"ComponentAProfileTwo\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentB\" Name = \"ComponentBProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentB\" Name = \"ComponentBProfileTwo\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                    "<VehicleComponentProfile Type=\"ComponentC\" Name = \"ComponentCProfileOne\">"
                        "<Double Key=\"FakeDoubleOne\" Value=\"0.0\"/>"
                    "</VehicleComponentProfile>"
                "</root>"
                );

    std::unordered_map<std::string, VehicleComponentProfiles> fakeVehicleComponentProfiles;

    ASSERT_TRUE(ProfilesImporter::ImportAllVehicleComponentProfiles(fakeDocumentRoot, fakeVehicleComponentProfiles));

    ASSERT_EQ(fakeVehicleComponentProfiles.at("ComponentA").size(), static_cast<size_t>(2));
    ASSERT_EQ(fakeVehicleComponentProfiles.at("ComponentB").size(), static_cast<size_t>(2));
    ASSERT_EQ(fakeVehicleComponentProfiles.at("ComponentC").size(), static_cast<size_t>(1));
}
