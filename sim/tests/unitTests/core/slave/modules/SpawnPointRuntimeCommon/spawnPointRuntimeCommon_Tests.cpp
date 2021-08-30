/*********************************************************************
* Copyright (c) 2019, 2021 in-tech
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SpawnPointRuntimeCommon.h"
#include "SpawnPointRuntimeCommonParameterExtractor.h"
#include "common/WorldAnalyzer.h"

#include "fakeParameter.h"
#include "fakeWorld.h"
#include "fakeCallback.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::UnorderedElementsAre;

namespace SpawnPointRuntimeCommonDefinitions {
std::ostream& operator<<(std::ostream& os, const SpawnPosition& position)
{
    os << "road: " << position.roadId
       << " lane: " << position.laneId
       << " s: " << position.sPosition;

    return os;
}
}

TEST(SpawnPointRuntimeCommonParameterExtractor, ExtractSpawnPointParameters)
{
    FakeParameter parameter;
    auto spawnPoint1 = std::make_shared<FakeParameter>();
    auto spawnPoint2 = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoint {{spawnPoint1, spawnPoint2}};

    std::map<std::string, const openpass::parameter::StochasticDistribution> stochasticParameters;
    ON_CALL(parameter, GetParametersStochastic()).WillByDefault(ReturnRef(stochasticParameters));
    std::map<std::string, double> doubleParameters;
    ON_CALL(parameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    const std::string invalidRoadId = "InvalidRoad";
    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {"RoadA", invalidRoadId}}};
    ON_CALL(*spawnPoint1, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{{"Lanes", {1,2,3}}};
    ON_CALL(*spawnPoint1, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{{"SCoordinate", 10.0}};
    ON_CALL(*spawnPoint1, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));
    std::map<std::string, const std::vector<std::string>> strings2{{"Roads", {"RoadB"}}};
    ON_CALL(*spawnPoint2, GetParametersStringVector()).WillByDefault(ReturnRef(strings2));
    std::map<std::string, const std::vector<int>> intVectors2{{"Lanes", {-1,-2}}};
    ON_CALL(*spawnPoint2, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors2));
    std::map<std::string, double> doubles2{{"SCoordinate", 11.0}};
    ON_CALL(*spawnPoint2, GetParametersDouble()).WillByDefault(ReturnRef(doubles2));

    auto trafficGroup1 = std::make_shared<FakeParameter>();
    auto trafficGroup2 = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists trafficGroups {{trafficGroup1, trafficGroup2}};

    openpass::parameter::NormalDistribution testNormalDistribution1 = {1.,2.,3.,4.};
    openpass::parameter::NormalDistribution testNormalDistribution2 = {2.,3.,4.,5.};
    openpass::parameter::NormalDistribution testNormalDistribution3 = {10.,20.,30.,40.};
    openpass::parameter::NormalDistribution testNormalDistribution4 = {20.,30.,40.,50.};

    std::map<std::string, double> emptyDoubles{};
    ON_CALL(*trafficGroup1, GetParametersDouble()).WillByDefault(ReturnRef(emptyDoubles));
    std::map<std::string, const openpass::parameter::StochasticDistribution> normalDistribution1{{"Velocity",testNormalDistribution1},
                                                                                                 {"TGap", testNormalDistribution2}};
    ON_CALL(*trafficGroup1, GetParametersStochastic()).WillByDefault(ReturnRef(normalDistribution1));
    std::map<std::string, const std::vector<double>> doubleVector1{{"Homogeneity", {0.1,0.2}}};
    ON_CALL(*trafficGroup1, GetParametersDoubleVector()).WillByDefault(ReturnRef(doubleVector1));
    std::map<std::string, bool> bools1{{"RightLaneOnly", true}};
    ON_CALL(*trafficGroup1, GetParametersBool()).WillByDefault(ReturnRef(bools1));

    auto agentProfile1 = std::make_shared<FakeParameter>();
    auto agentProfile2 = std::make_shared<FakeParameter>();
    std::map<std::string, double> weight{{"Weight", 2.}};
    ON_CALL(*agentProfile1, GetParametersDouble()).WillByDefault(ReturnRef(weight));
    std::map<std::string, const std::string> name1{{"Name", "Profile1"}};
    ON_CALL(*agentProfile1, GetParametersString()).WillByDefault(ReturnRef(name1));
    ON_CALL(*agentProfile2, GetParametersDouble()).WillByDefault(ReturnRef(emptyDoubles));
    std::map<std::string, const std::string> name2{{"Name", "Profile2"}};
    ON_CALL(*agentProfile2, GetParametersString()).WillByDefault(ReturnRef(name2));

    ParameterInterface::ParameterLists agentProfiles1 {{agentProfile1, agentProfile2}};
    std::map<std::string, ParameterInterface::ParameterLists> agentProfilesList1{{"AgentProfiles", agentProfiles1}};
    ON_CALL(*trafficGroup1, GetParameterLists()).WillByDefault(ReturnRef(agentProfilesList1));

    ON_CALL(*trafficGroup2, GetParametersDouble()).WillByDefault(ReturnRef(weight));
    std::map<std::string, const openpass::parameter::StochasticDistribution> normalDistribution2{{"Velocity",testNormalDistribution3},
                                                                                                 {"TGap", testNormalDistribution4}};
    ON_CALL(*trafficGroup2, GetParametersStochastic()).WillByDefault(ReturnRef(normalDistribution2));
    std::map<std::string, const std::vector<double>> emptyDoubleVector{};
    ON_CALL(*trafficGroup2, GetParametersDoubleVector()).WillByDefault(ReturnRef(emptyDoubleVector));
    std::map<std::string, bool> emptyBools{};
    ON_CALL(*trafficGroup2, GetParametersBool()).WillByDefault(ReturnRef(emptyBools));

    auto agentProfile3 = std::make_shared<FakeParameter>();
    ON_CALL(*agentProfile3, GetParametersDouble()).WillByDefault(ReturnRef(weight));
    std::map<std::string, const std::string> name3{{"Name", "Profile3"}};
    ON_CALL(*agentProfile3, GetParametersString()).WillByDefault(ReturnRef(name3));

    ParameterInterface::ParameterLists agentProfiles2 {{agentProfile3}};
    std::map<std::string, ParameterInterface::ParameterLists> agentProfilesList2{{"AgentProfiles", agentProfiles2}};
    ON_CALL(*trafficGroup2, GetParameterLists()).WillByDefault(ReturnRef(agentProfilesList2));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnPoints", spawnPoint}, {"TrafficGroups", trafficGroups}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(invalidRoadId, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, IsLaneTypeValid(_, _, _, _)).WillByDefault(Return(true));

    WorldAnalyzer worldAnalyzer{&fakeWorld};
    FakeCallback callbacks;

    const LaneTypes validLaneTypes = {};

    const auto result = SpawnPointRuntimeCommonParameterExtractor::ExtractSpawnPointParameters(parameter, worldAnalyzer, validLaneTypes, &callbacks);

    const auto spawnPositions = result.spawnPositions;
    ASSERT_THAT(result.spawnPositions, UnorderedElementsAre(SpawnPosition{"RoadA", 1, 10.},
                                                            SpawnPosition{"RoadA", 2, 10.},
                                                            SpawnPosition{"RoadA", 3, 10.},
                                                            SpawnPosition{"RoadB", -1, 11.},
                                                            SpawnPosition{"RoadB", -2, 11.}));

    SpawningAgentProfile spawningAgentProfile1 = {"Profile1", openpass::parameter::NormalDistribution{1.,2.,3.,4.}, {0.1,0.2}, openpass::parameter::NormalDistribution{2.,3.,4.,5.}};
    SpawningAgentProfile spawningAgentProfile2 = {"Profile2", openpass::parameter::NormalDistribution{1.,2.,3.,4.}, {0.1,0.2}, openpass::parameter::NormalDistribution{2.,3.,4.,5.}};
    SpawningAgentProfile spawningAgentProfile3 = {"Profile3", openpass::parameter::NormalDistribution{10.,20.,30.,40.}, {1}, openpass::parameter::NormalDistribution{20.,30.,40.,50.}};

    ASSERT_THAT(result.agentProfileLaneMaps.rightLanes, UnorderedElementsAre(
                    std::make_pair(spawningAgentProfile1, 2),
                    std::make_pair(spawningAgentProfile2, 1),
                    std::make_pair(spawningAgentProfile3, 4)));

    ASSERT_THAT(result.agentProfileLaneMaps.leftLanes, UnorderedElementsAre(
                    std::make_pair(spawningAgentProfile3, 4)));
}

TEST(SpawnPointRuntimeCommonParameterExtractor, ExtractSpawnPoints)
{
    FakeParameter parameter;
    auto spawnPoint1 = std::make_shared<FakeParameter>();
    auto spawnPoint2 = std::make_shared<FakeParameter>();
    ParameterInterface::ParameterLists spawnPoint {{spawnPoint1, spawnPoint2}};

    std::map<std::string, const openpass::parameter::StochasticDistribution> stochasticParameters;
    ON_CALL(parameter, GetParametersStochastic()).WillByDefault(ReturnRef(stochasticParameters));
    std::map<std::string, double> doubleParameters;
    ON_CALL(parameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    const std::string roadA = "RoadA";
    const std::string roadB = "RoadB";
    const std::string invalidRoadId = "InvalidRoad";
    std::map<std::string, const std::vector<std::string>> strings1{{"Roads", {roadA, invalidRoadId}}};
    ON_CALL(*spawnPoint1, GetParametersStringVector()).WillByDefault(ReturnRef(strings1));
    std::map<std::string, const std::vector<int>> intVectors1{{"Lanes", {1,2,3}}};
    ON_CALL(*spawnPoint1, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors1));
    std::map<std::string, double> doubles1{{"SCoordinate", 10.0}};
    ON_CALL(*spawnPoint1, GetParametersDouble()).WillByDefault(ReturnRef(doubles1));

    std::map<std::string, const std::vector<std::string>> strings2{{"Roads", {"RoadB"}}};
    ON_CALL(*spawnPoint2, GetParametersStringVector()).WillByDefault(ReturnRef(strings2));
    std::map<std::string, const std::vector<int>> intVectors2{{"Lanes", {-1,-2}}};
    ON_CALL(*spawnPoint2, GetParametersIntVector()).WillByDefault(ReturnRef(intVectors2));
    std::map<std::string, double> doubles2{{"SCoordinate", 11.0}};
    ON_CALL(*spawnPoint2, GetParametersDouble()).WillByDefault(ReturnRef(doubles2));

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnPoints", spawnPoint}};
    ON_CALL(parameter, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));

    const LaneTypes validLaneTypes = {LaneType::Driving};

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(_, _)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsDirectionalRoadExisting(invalidRoadId, _)).WillByDefault(Return(false));
    ON_CALL(fakeWorld, IsLaneTypeValid(roadA, _, _, validLaneTypes)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsLaneTypeValid(roadB, _, _, validLaneTypes)).WillByDefault(Return(true));
    ON_CALL(fakeWorld, IsLaneTypeValid(roadB, -2, _, validLaneTypes)).WillByDefault(Return(false));

    WorldAnalyzer worldAnalyzer{&fakeWorld};
    FakeCallback callbacks;

    const auto result = SpawnPointRuntimeCommonParameterExtractor::ExtractSpawnPoints(parameter, worldAnalyzer, validLaneTypes, &callbacks);

    ASSERT_THAT(result, UnorderedElementsAre(SpawnPosition{roadA, 1, 10.},
                                             SpawnPosition{roadA, 2, 10.},
                                             SpawnPosition{roadA, 3, 10.},
                                             SpawnPosition{roadB , -1, 11.}));

    SpawningAgentProfile spawningAgentProfile1 = {"Profile1", openpass::parameter::NormalDistribution{1.,2.,3.,4.}, {0.1,0.2}, openpass::parameter::NormalDistribution{2.,3.,4.,5.}};
    SpawningAgentProfile spawningAgentProfile2 = {"Profile2", openpass::parameter::NormalDistribution{1.,2.,3.,4.}, {0.1,0.2}, openpass::parameter::NormalDistribution{2.,3.,4.,5.}};
    SpawningAgentProfile spawningAgentProfile3 = {"Profile3", openpass::parameter::NormalDistribution{10.,20.,30.,40.}, {1}, openpass::parameter::NormalDistribution{20.,30.,40.,50.}};
}

TEST(SpawnPointRuntimeCommonParameterExtractor, ExtractMinimumSeparationBuffer_FixedValue)
{
    FakeParameter parameter;
    std::map<std::string, const openpass::parameter::StochasticDistribution> stochasticParameters;
    ON_CALL(parameter, GetParametersStochastic()).WillByDefault(ReturnRef(stochasticParameters));
    std::map<std::string, double> doubleParameters {{"MinimumSeparationBuffer", 12.3}};
    ON_CALL(parameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    const LaneTypes validLaneTypes = {};

    const auto result = SpawnPointDefinitions::ExtractMinimumSpawnBuffer(parameter);
    ASSERT_THAT(std::holds_alternative<double>(result), Eq(true));
    ASSERT_THAT(std::get<double>(result), Eq(12.3));
}

TEST(SpawnPointRuntimeCommonParameterExtractor, ExtractMinimumSeparationBuffer_StochasticValue)
{
    FakeParameter parameter;
    std::map<std::string, const openpass::parameter::StochasticDistribution> stochasticParameters{{"MinimumSeparationBuffer", openpass::parameter::UniformDistribution(0,10)}};
    ON_CALL(parameter, GetParametersStochastic()).WillByDefault(ReturnRef(stochasticParameters));
    std::map<std::string, double> doubleParameters;
    ON_CALL(parameter, GetParametersDouble()).WillByDefault(ReturnRef(doubleParameters));

    const LaneTypes validLaneTypes = {};

    const auto result = SpawnPointDefinitions::ExtractMinimumSpawnBuffer(parameter);
    ASSERT_THAT(std::holds_alternative<openpass::parameter::StochasticDistribution>(result), Eq(true));
    const auto& distribution = std::get<openpass::parameter::StochasticDistribution>(result);
    ASSERT_THAT(std::holds_alternative<openpass::parameter::UniformDistribution>(distribution), Eq(true));
}
