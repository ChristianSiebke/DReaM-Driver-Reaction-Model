/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech GmbH on behalf of BMW
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
#include "slaveConfigImporter.h"

using ::testing::SizeIs;
using ::testing::UnorderedElementsAre;
using ::testing::EndsWith;

using namespace Importer;

TEST(SlaveConfigImporter_UnitTests, ImportExperimentConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<ExperimentID>1337</ExperimentID>"
                                       "<NumberOfInvocations>5</NumberOfInvocations>"
                                       "<RandomSeed>12345</RandomSeed>"
                                       "<LoggingGroups>"
                                       "<LoggingGroup>Group1</LoggingGroup>"
                                       "<LoggingGroup>Group2</LoggingGroup>"
                                       "</LoggingGroups>"
                                       "</root>"
                                   );


    ExperimentConfig experimentConfig;

    bool importResult = SlaveConfigImporter::ImportExperimentConfig(fakeDocumentRoot, experimentConfig);

    ASSERT_THAT(importResult,                         true);
    EXPECT_THAT(experimentConfig.experimentId,        1337);
    EXPECT_THAT(experimentConfig.numberOfInvocations, 5);
    EXPECT_THAT(experimentConfig.randomSeed,          12345);
    EXPECT_THAT(experimentConfig.loggingGroups,       UnorderedElementsAre("Group1", "Group2"));
}

TEST(SlaveConfigImporter_UnitTests, ImportExperimentConfigUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingLoggingTag = documentRootFromString(
                "<root>"
                "<ExperimentID>1337</ExperimentID>"
                "<NumberOfInvocations>5</NumberOfInvocations>"
                "<RandomSeed>12345</RandomSeed>"
                "</root>"
            );

    QDomElement fakeDocumentRootWrongDataType = documentRootFromString(
                "<root>"
                "<ExperimentID>1337</ExperimentID>"
                "<NumberOfInvocations>abc</NumberOfInvocations>"
                "<RandomSeed>12345</RandomSeed>"
                "<LoggingGroups/>"
                "</root>"
            );

    QDomElement fakeDocumentRootWrongTagName = documentRootFromString(
                "<root>"
                "<Experimentid>1337</Experimentid>"
                "<NumberOfInvocations>5</NumberOfInvocations>"
                "<RandomSeed>12345</RandomSeed>"
                "<LoggingGroups/>"
                "</root>"
            );

    ExperimentConfig experimentConfig;

    ASSERT_FALSE(SlaveConfigImporter::ImportExperimentConfig(fakeDocumentRootMissingLoggingTag, experimentConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportExperimentConfig(fakeDocumentRootWrongDataType, experimentConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportExperimentConfig(fakeDocumentRootWrongTagName, experimentConfig));
}

TEST(SlaveConfigImporter_UnitTests, ImportValidLoggingGroups_Succeeds)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<LoggingGroups>"
                                       "<LoggingGroup>Group01</LoggingGroup>"
                                       "<LoggingGroup>Group02</LoggingGroup>"
                                       "<LoggingGroup>AnotherGroup</LoggingGroup>"
                                       "</LoggingGroups>"
                                       "</root>"
                                   );


    std::vector<std::string> loggingGroups;

    bool importResult = SlaveConfigImporter::ImportLoggingGroups(fakeDocumentRoot, loggingGroups);

    ASSERT_THAT(importResult, true);
    EXPECT_THAT(loggingGroups, UnorderedElementsAre("Group01", "Group02", "AnotherGroup"));
}

TEST(SlaveConfigImporter_UnitTests, ImportEmptyLoggingGroupString_Fails)
{
    QDomElement fakeDocumentRootEmptyString = documentRootFromString(
                "<root>"
                "<LoggingGroups>"
                "<LoggingGroup></LoggingGroup>"
                "</LoggingGroups>"
                "</root>"
            );

    std::vector<std::string> loggingGroups;

    bool importResult = SlaveConfigImporter::ImportLoggingGroups(fakeDocumentRootEmptyString, loggingGroups);

    ASSERT_THAT(importResult, false);
    EXPECT_THAT(loggingGroups, SizeIs(0));
}


TEST(SlaveConfigImporter_UnitTests, ImportMissingLibraries_LoadsDefaults)
{
    QDomElement fakeDocumentRoot = documentRootFromString("<root/>");

    auto libraries = SlaveConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "EventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["WorldLibrary"], "World");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
    EXPECT_THAT(libraries["SpawnPointLibrary"], "SpawnPoint_OSI");
    EXPECT_THAT(libraries["ObservationLibrary"], "Observation");
}

TEST(SlaveConfigImporter_UnitTests, ImportCompleteLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "  <Libraries>"
                                       "    <EventDetectorLibrary>TestEventDetector</EventDetectorLibrary>"
                                       "    <ManipulatorLibrary>TestManipulator</ManipulatorLibrary>"
                                       "    <WorldLibrary>TestWorld</WorldLibrary>"
                                       "    <StochasticsLibrary>TestStochastics</StochasticsLibrary>"
                                       "    <SpawnPointLibrary>TestSpawnPoint</SpawnPointLibrary>"
                                       "    <ObservationLibrary>TestObservation</ObservationLibrary>"
                                       "  </Libraries>"
                                       "</root>"
                                   );

    auto libraries = SlaveConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "TestEventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "TestManipulator");
    EXPECT_THAT(libraries["WorldLibrary"], "TestWorld");
    EXPECT_THAT(libraries["StochasticsLibrary"], "TestStochastics");
    EXPECT_THAT(libraries["SpawnPointLibrary"], "TestSpawnPoint");
    EXPECT_THAT(libraries["ObservationLibrary"], "TestObservation");
}

TEST(SlaveConfigImporter_UnitTests, ImportPartialLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "  <Libraries>"
                                       "    <WorldLibrary>TestWorld</WorldLibrary>"
                                       "    <ObservationLibrary>TestObservation</ObservationLibrary>"
                                       "  </Libraries>"
                                       "</root>"
                                   );

    auto libraries = SlaveConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["WorldLibrary"], "TestWorld");
    EXPECT_THAT(libraries["ObservationLibrary"], "TestObservation");
    EXPECT_THAT(libraries["EventDetectorLibrary"], "EventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
    EXPECT_THAT(libraries["SpawnPointLibrary"], "SpawnPoint_OSI");
}

TEST(SlaveConfigImporter_UnitTests, ImportEmptyLibraryList_ParsesSpecifiedValues)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "</root>"
                                   );

    auto libraries = SlaveConfigImporter::ImportLibraries(fakeDocumentRoot);

    EXPECT_THAT(libraries["EventDetectorLibrary"], "EventDetector");
    EXPECT_THAT(libraries["ManipulatorLibrary"], "Manipulator");
    EXPECT_THAT(libraries["ObservationLibrary"], "Observation");
    EXPECT_THAT(libraries["SpawnPointLibrary"], "SpawnPoint_OSI");
    EXPECT_THAT(libraries["StochasticsLibrary"], "Stochastics");
    EXPECT_THAT(libraries["WorldLibrary"], "World");
}

TEST(SlaveConfigImporter_UnitTests, ImportScenarioConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<OpenScenarioFile>scenarioFile.xml</OpenScenarioFile>"
                                       "</root>"
                                   );


    ScenarioConfig scenarioConfig;

    ASSERT_THAT(SlaveConfigImporter::ImportScenarioConfig(fakeDocumentRoot, "configs", scenarioConfig), true);
    ASSERT_THAT(scenarioConfig.scenarioPath, EndsWith("configs/scenarioFile.xml"));
}

TEST(SlaveConfigImporter_UnitTests, ImportScenarioConfigUnsuccessfullyMissingTag)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<OpenScenarioF>scenarioFile.xml</OpenScenarioF>"
                                       "</root>"
                                   );


    ScenarioConfig scenarioConfig;

    ASSERT_THAT(SlaveConfigImporter::ImportScenarioConfig(fakeDocumentRoot, "configs", scenarioConfig), false);
}

TEST(SlaveConfigImporter_UnitTests, ImportEnvironmentConfigSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<TimeOfDays>"
                                       "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                                       "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                                       "</TimeOfDays>"
                                       "<VisibilityDistances>"
                                       "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                                       "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                                       "</VisibilityDistances>"
                                       "<Frictions>"
                                       "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                                       "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                                       "</Frictions>"
                                       "<Weathers>"
                                       "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                                       "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                                       "</Weathers>"
                                       "</root>"
                                   );


    EnvironmentConfig environmentConfig;

    ASSERT_TRUE(SlaveConfigImporter::ImportEnvironmentConfig(fakeDocumentRoot, environmentConfig));

    ASSERT_EQ(environmentConfig.timeOfDays.at("12"), 0.5);
    ASSERT_EQ(environmentConfig.timeOfDays.at("18"), 0.5);
    ASSERT_EQ(environmentConfig.visibilityDistances.at(100), 0.5);
    ASSERT_EQ(environmentConfig.visibilityDistances.at(200), 0.5);
    ASSERT_EQ(environmentConfig.frictions.at(0.3), 0.5);
    ASSERT_EQ(environmentConfig.frictions.at(0.7), 0.5);
    ASSERT_EQ(environmentConfig.weathers.at("Rainy"), 0.5);
    ASSERT_EQ(environmentConfig.weathers.at("Snowy"), 0.5);
}

TEST(SlaveConfigImporter_UnitTests, ImportEnvironmentConfigUnsuccessfully)
{
    QDomElement fakeDocumentRootWrongProbabilities = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.6\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "<Weathers>"
                "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                "</Weathers>"
                "</root>"
            );

    QDomElement fakeDocumentRootMissingAtLeastOneElement = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "<Weathers>"
                "<Weather Value=\"Rainy\" Probability=\"0.5\"/>"
                "<Weather Value=\"Snowy\" Probability=\"0.5\"/>"
                "</Weathers>"
                "</root>"
            );

    QDomElement fakeDocumentRootMissingTag = documentRootFromString(
                "<root>"
                "<TimeOfDays>"
                "<TimeOfDay Value=\"12\" Probability=\"0.5\"/>"
                "<TimeOfDay Value=\"18\" Probability=\"0.5\"/>"
                "</TimeOfDays>"
                "<VisibilityDistances>"
                "<VisibilityDistance Value=\"100\" Probability=\"0.5\"/>"
                "<VisibilityDistance Value=\"200\" Probability=\"0.5\"/>"
                "</VisibilityDistances>"
                "<Frictions>"
                "<Friction Value=\"0.3\" Probability=\"0.5\"/>"
                "<Friction Value=\"0.7\" Probability=\"0.5\"/>"
                "</Frictions>"
                "</root>"
            );

    EnvironmentConfig environmentConfig;

    ASSERT_FALSE(SlaveConfigImporter::ImportEnvironmentConfig(fakeDocumentRootWrongProbabilities,
                 environmentConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportEnvironmentConfig(fakeDocumentRootMissingAtLeastOneElement,
                 environmentConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportEnvironmentConfig(fakeDocumentRootMissingTag, environmentConfig));
}

TEST(SlaveConfigImporter_UnitTests, ImportTrafficParameterSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<TrafficVolumes>"
                                       "<TrafficVolume Value = \"900\" Probability = \"0.5\"/>"
                                       "<TrafficVolume Value = \"1500\" Probability = \"0.5\"/>"
                                       "</TrafficVolumes>"
                                       "<PlatoonRates>"
                                       "<PlatoonRate Value = \"0.3\" Probability = \"1.0\"/>"
                                       "</PlatoonRates>"
                                       "<Velocities>"
                                       "<Velocity Value = \"85.0\" Probability = \"0.2\"/>"
                                       "<Velocity Value = \"3.0\" Probability = \"0.8\"/>"
                                       "</Velocities>"
                                       "<Homogenities>"
                                       "<Homogenity Value = \"0.2\" Probability = \"1.0\"/>"
                                       "<Homogenity Value = \"0.4\" Probability = \"0.0\"/>"
                                       "</Homogenities>"
                                       "</root>"
                                   );

    TrafficConfig trafficConfig;

    ASSERT_TRUE(SlaveConfigImporter::ImportTrafficParameter(fakeDocumentRoot, trafficConfig));

    ASSERT_EQ(trafficConfig.trafficVolumes.at(900.0), 0.5);
    ASSERT_EQ(trafficConfig.trafficVolumes.at(1500.0), 0.5);
    ASSERT_EQ(trafficConfig.platoonRates.at(0.3), 1.0);
    ASSERT_EQ(trafficConfig.velocities.at(85.0), 0.2);
    ASSERT_EQ(trafficConfig.velocities.at(3.0), 0.8);
    ASSERT_EQ(trafficConfig.homogenities.at(0.2), 1.0);
    ASSERT_EQ(trafficConfig.homogenities.at(0.4), 0.0);
}

TEST(SlaveConfigImporter_UnitTests, ImportTrafficParameterUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingTag = documentRootFromString(
                "<root>"
                "<TrafficVolumes>"
                "<TrafficVolume Value = \"900\" Probability = \"0.5\"/>"
                "<TrafficVolume Value = \"1500\" Probability = \"0.5\"/>"
                "</TrafficVolumes>"
                "<Velocities>"
                "<Velocity Value = \"85.0\" Probability = \"0.0\"/>"
                "<Velocity Value = \"3.0\" Probability = \"0.8\"/>"
                "</Velocities>"
                "<Homogenities>"
                "<Homogenity Value = \"0.2\" Probability = \"1.0\"/>"
                "<Homogenity Value = \"0.4\" Probability = \"0.0\"/>"
                "</Homogenities>"
                "</root>"
            );

    QDomElement fakeDocumentRootAtLeastOneEntryMissing = documentRootFromString(
                "<root>"
                "<TrafficVolumes>"
                "<TrafficVolume Value = \"900\" Probability = \"0.5\"/>"
                "<TrafficVolume Value = \"1500\" Probability = \"0.5\"/>"
                "</TrafficVolumes>"
                "<PlatoonRates>"
                "<PlatoonRate Value = \"0.3\" Probability = \"1.0\"/>"
                "</PlatoonRates>"
                "<Velocities>"
                "</Velocities>"
                "<Homogenities>"
                "<Homogenity Value = \"0.2\" Probability = \"1.0\"/>"
                "<Homogenity Value = \"0.4\" Probability = \"0.0\"/>"
                "</Homogenities>"
                "</root>"
            );

    TrafficConfig trafficConfig;

    ASSERT_FALSE(SlaveConfigImporter::ImportTrafficParameter(fakeDocumentRootMissingTag, trafficConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportTrafficParameter(fakeDocumentRootAtLeastOneEntryMissing, trafficConfig));
}

TEST(SlaveConfigImporter_UnitTests, ImportLaneParameterSuccessfully)
{
    QDomElement fakeDocumentRoot = documentRootFromString(
                                       "<root>"
                                       "<RegularLane>"
                                       "<AgentProfile Name = \"Grandpa\" Probability = \"0.5\"/>"
                                       "<AgentProfile Name = \"Grandma\" Probability = \"0.5\"/>"
                                       "</RegularLane>"
                                       "<RightMostLane>"
                                       "<AgentProfile Name = \"Truck\" Probability = \"1.0\"/>"
                                       "</RightMostLane>"
                                       "</root>"
                                   );

    TrafficConfig trafficConfig;

    ASSERT_TRUE(SlaveConfigImporter::ImportLaneParameter(fakeDocumentRoot, trafficConfig));

    ASSERT_EQ(trafficConfig.regularLaneAgents.at("Grandpa"), 0.5);
    ASSERT_EQ(trafficConfig.regularLaneAgents.at("Grandma"), 0.5);
    ASSERT_EQ(trafficConfig.rightMostLaneAgents.at("Truck"), 1.0);
}

TEST(SlaveConfigImporter_UnitTests, ImportLaneParameterUnsuccessfully)
{
    QDomElement fakeDocumentRootMissingTag = documentRootFromString(
                "<root>"
                "<RightMostLane>"
                "<AgentProfile Name = \"Truck\" Probability = \"1.0\"/>"
                "</RightMostLane>"
                "</root>"
            );

    QDomElement fakeDocumentRootAtLeastOneEntryMissing = documentRootFromString(
                "<root>"
                "<RegularLane>"
                "<AgentProfile Name = \"Grandpa\" Probability = \"0.5\"/>"
                "<AgentProfile Name = \"Grandma\" Probability = \"0.5\"/>"
                "</RegularLane>"
                "<RightMostLane>"
                "</RightMostLane>"
                "</root>"
            );

    TrafficConfig trafficConfig;

    ASSERT_FALSE(SlaveConfigImporter::ImportLaneParameter(fakeDocumentRootMissingTag, trafficConfig));
    ASSERT_FALSE(SlaveConfigImporter::ImportLaneParameter(fakeDocumentRootAtLeastOneEntryMissing, trafficConfig));
}
