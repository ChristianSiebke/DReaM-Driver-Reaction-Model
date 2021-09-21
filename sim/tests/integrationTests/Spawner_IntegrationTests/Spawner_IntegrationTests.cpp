/*********************************************************************
* Copyright (c) 2020, 2021 in-tech GmbH
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "SpawnerPreRunCommon.h"
#include "fakeAgent.h"
#include "fakeAgentBlueprint.h"
#include "fakeAgentBlueprintProvider.h"
#include "fakeAgentFactory.h"
#include "fakeCallback.h"
#include "fakeParameter.h"
#include "fakeStochastics.h"
#include "fakeStream.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::Le;
using ::testing::Ge;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::DoubleEq;
using ::testing::ByMove;

constexpr char ROADID[] = "MyRoad";

class SpawnerPreRun_IntegrationTests : public testing::Test
{
public:
    SpawnerPreRun_IntegrationTests()
    {
        dependencies.parameters = &parameters;
        ON_CALL(parameters, GetParameterLists()).WillByDefault(ReturnRef(parameterLists));
        ON_CALL(parameters, GetParametersStochastic()).WillByDefault(ReturnRef(parametersStochastic));
        ON_CALL(parameters, GetParametersDouble()).WillByDefault(ReturnRef(parametersDouble));
        ON_CALL(*trafficGroup1, GetParametersDouble()).WillByDefault(ReturnRef(trafficGroup1Double));
        ON_CALL(*trafficGroup1, GetParametersStochastic()).WillByDefault(ReturnRef(trafficGroup1Stochastic));
        ON_CALL(*trafficGroup1, GetParametersBool()).WillByDefault(ReturnRef(trafficGroup1Bool));
        ON_CALL(*trafficGroup1, GetParametersDoubleVector()).WillByDefault(ReturnRef(trafficGroup1DoubleVector));
        ON_CALL(*trafficGroup1, GetParameterLists()).WillByDefault(ReturnRef(trafficGroup1Lists));
        ON_CALL(*trafficGroup2, GetParametersDouble()).WillByDefault(ReturnRef(trafficGroup2Double));
        ON_CALL(*trafficGroup2, GetParametersStochastic()).WillByDefault(ReturnRef(trafficGroup2Stochastic));
        ON_CALL(*trafficGroup2, GetParametersBool()).WillByDefault(ReturnRef(trafficGroup2Bool));
        ON_CALL(*trafficGroup2, GetParametersDoubleVector()).WillByDefault(ReturnRef(trafficGroup2DoubleVector));
        ON_CALL(*trafficGroup2, GetParameterLists()).WillByDefault(ReturnRef(trafficGroup2Lists));
        ON_CALL(*agentProfile1a, GetParametersDouble()).WillByDefault(ReturnRef(agentProfile1aDouble));
        ON_CALL(*agentProfile1b, GetParametersDouble()).WillByDefault(ReturnRef(agentProfile1bDouble));
        ON_CALL(*agentProfile2a, GetParametersDouble()).WillByDefault(ReturnRef(agentProfile2aDouble));
        ON_CALL(*agentProfile2b, GetParametersDouble()).WillByDefault(ReturnRef(agentProfile2bDouble));
        ON_CALL(*agentProfile1a, GetParametersString()).WillByDefault(ReturnRef(agentProfile1aString));
        ON_CALL(*agentProfile1b, GetParametersString()).WillByDefault(ReturnRef(agentProfile1bString));
        ON_CALL(*agentProfile2a, GetParametersString()).WillByDefault(ReturnRef(agentProfile2aString));
        ON_CALL(*agentProfile2b, GetParametersString()).WillByDefault(ReturnRef(agentProfile2bString));
        ON_CALL(stochastics, GetUniformDistributed(10,20)).WillByDefault(Return(15));
        ON_CALL(stochastics, GetUniformDistributed(20,30)).WillByDefault(Return(25));
        ON_CALL(stochastics, GetUniformDistributed(1,3)).WillByDefault(Return(2));
        ON_CALL(stochastics, GetUniformDistributed(2,4)).WillByDefault(Return(3));
        ON_CALL(stochastics, GetUniformDistributed(0,DoubleEq(6))).WillByDefault(Return(1));
        ON_CALL(stochastics, GetUniformDistributed(0,DoubleEq(41))).WillByDefault(Return(7));
        ON_CALL(agentFactory, AddAgent(_)).WillByDefault([&](AgentBlueprintInterface* bluePrint)
        {
            agents.push_back(new core::Agent(&world, *bluePrint));
            auto laneId = -1 + static_cast<int>(bluePrint->GetSpawnParameter().positionY / 3.0);
            auto agent = new FakeAgent;
            agentsOnLane[laneId].insert(agentsOnLane[laneId].begin(), agent);
            ON_CALL(*agent, GetDistanceToStartOfRoad(_,_)).WillByDefault(Return(bluePrint->GetSpawnParameter().positionX));
            ON_CALL(*agent, GetLength()).WillByDefault(Return(5.0));
            ON_CALL(*agent, GetDistanceReferencePointToLeadingEdge()).WillByDefault(Return(3.0));
            ON_CALL(*agent, GetVelocity()).WillByDefault(Return(bluePrint->GetSpawnParameter().velocity));
            ON_CALL(*agent, GetAgentTypeName()).WillByDefault(Return(bluePrint->GetAgentProfileName()));
            return agents.back();
        });

        RoadGraph roadGraph {};
        RoadGraphVertex vertex {};
        RouteElement routeElement{ROADID, true};
        add_vertex(routeElement, roadGraph);

        ON_CALL(world, IsDirectionalRoadExisting(ROADID, true))
                .WillByDefault(Return(true));
        ON_CALL(world, IsDirectionalRoadExisting(ROADID, false))
                .WillByDefault(Return(false));
        ON_CALL(world, IsLaneTypeValid(_, _, _, _))
                .WillByDefault(Return(true));
        ON_CALL(world, GetRoadGraph(_, _))
                .WillByDefault(Return(std::pair<RoadGraph, RoadGraphVertex>{roadGraph, vertex}));
        ON_CALL(world, LaneCoord2WorldCoord(_,_,_,_)).WillByDefault(
                    [&](double distanceOnLane, double offset, std::string , int laneId)
        {return Position{distanceOnLane, offset + 3 * laneId + 1.5, 0, 0};});

        FakeRoadStream* roadStream = new FakeRoadStream;
        ON_CALL(*roadStream, GetStreamPosition(_)).WillByDefault(
                    [](const GlobalRoadPosition& roadPosition){return StreamPosition{roadPosition.roadPosition.s, 0};});
        FakeLaneStream* laneStream1 = new FakeLaneStream;
        ON_CALL(*laneStream1, GetAgentsInRange(_,_)).WillByDefault(
                    [&](StreamPosition start, StreamPosition end)
        {
            AgentInterfaces agentsInRange;
            for (const auto agent : agentsOnLane[-1])
            {
                auto agentS = agent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, "");
                if (start.s <= agentS && agentS <= end.s)
                {
                    agentsInRange.push_back(agent);
                }
            }
            return agentsInRange;
        });
        ON_CALL(*laneStream1, GetStreamPosition(_)).WillByDefault(
                    [](const GlobalRoadPosition& roadPosition){return StreamPosition{roadPosition.roadPosition.s, 0};});
        ON_CALL(*laneStream1, GetRoadPosition(_)).WillByDefault(
                    [](const StreamPosition& streamPosition){return GlobalRoadPosition{ROADID, -1, streamPosition.s, streamPosition.t, 0};});
        std::vector<std::pair<double, LaneType>> laneTypes{{0, LaneType::Driving}};
        ON_CALL(*laneStream1, GetLaneTypes()).WillByDefault(Return(laneTypes));
        ON_CALL(*laneStream1, GetStreamPosition(_, MeasurementPoint::Rear)).WillByDefault(
                    [](const WorldObjectInterface* object, const MeasurementPoint&)
        {return StreamPosition{object->GetDistanceToStartOfRoad(MeasurementPoint::Rear, ROADID),0};});
        FakeLaneStream* laneStream2 = new FakeLaneStream;
        ON_CALL(*laneStream2, GetAgentsInRange(_,_)).WillByDefault(
                    [&](StreamPosition start, StreamPosition end)
        {
            AgentInterfaces agentsInRange;
            for (const auto agent : agentsOnLane[-2])
            {
                auto agentS = agent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, "");
                if (start.s <= agentS && agentS <= end.s)
                {
                    agentsInRange.push_back(agent);
                }
            }
            return agentsInRange;
        });
        ON_CALL(*laneStream2, GetStreamPosition(_)).WillByDefault(
                    [](const GlobalRoadPosition& roadPosition){return StreamPosition{roadPosition.roadPosition.s, 0};});
        ON_CALL(*laneStream2, GetRoadPosition(_)).WillByDefault(
                    [](const StreamPosition& streamPosition){return GlobalRoadPosition{ROADID, -2, streamPosition.s, streamPosition.t, 0};});
        ON_CALL(*laneStream2, GetLaneTypes()).WillByDefault(Return(laneTypes));
        ON_CALL(*laneStream2, GetStreamPosition(_, MeasurementPoint::Rear)).WillByDefault(
                    [](const WorldObjectInterface* object, const MeasurementPoint&)
        {return StreamPosition{object->GetDistanceToStartOfRoad(MeasurementPoint::Rear, ROADID),0};});
        FakeLaneStream* laneStream3 = new FakeLaneStream;
        ON_CALL(*laneStream3, GetAgentsInRange(_,_)).WillByDefault(
                    [&](StreamPosition start, StreamPosition end)
        {
            AgentInterfaces agentsInRange;
            for (const auto agent : agentsOnLane[-3])
            {
                auto agentS = agent->GetDistanceToStartOfRoad(MeasurementPoint::Reference, "");
                if (start.s <= agentS && agentS <= end.s)
                {
                    agentsInRange.push_back(agent);
                }
            }
            return agentsInRange;
        });
        ON_CALL(*laneStream3, GetStreamPosition(_)).WillByDefault(
                    [](const GlobalRoadPosition& roadPosition){return StreamPosition{roadPosition.roadPosition.s, 0};});
        ON_CALL(*laneStream3, GetRoadPosition(_)).WillByDefault(
                    [](const StreamPosition& streamPosition){return GlobalRoadPosition{ROADID, -3, streamPosition.s, streamPosition.t, 0};});
        ON_CALL(*laneStream3, GetLaneTypes()).WillByDefault(Return(laneTypes));
        ON_CALL(*laneStream3, GetStreamPosition(_, MeasurementPoint::Rear)).WillByDefault(
                    [](const WorldObjectInterface* object, const MeasurementPoint&)
        {return StreamPosition{object->GetDistanceToStartOfRoad(MeasurementPoint::Rear, ROADID),0};});
        ON_CALL(*roadStream, GetLaneStream(_,-1)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream1))));
        ON_CALL(*roadStream, GetLaneStream(_,-2)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream2))));
        ON_CALL(*roadStream, GetLaneStream(_,-3)).WillByDefault(Return(ByMove(std::unique_ptr<LaneStreamInterface>(laneStream3))));
        ON_CALL(world, GetRoadStream(std::vector<RouteElement>{{ROADID,true}})).WillByDefault(Return(ByMove(std::unique_ptr<RoadStreamInterface>(roadStream))));

        LaneSection fakeLaneSection1 {1000.0, 1500.0, {-1, -2, -3}};
        LaneSections fakeLaneSections {fakeLaneSection1};
        ON_CALL(world, GetLaneSections(ROADID))
                .WillByDefault(Return(fakeLaneSections));

        AgentBlueprint agentBlueprint;
        VehicleModelParameters vehicleModelParameters;
        vehicleModelParameters.vehicleType = AgentVehicleType::Car;
        vehicleModelParameters.boundingBoxDimensions.length = 5.0;
        vehicleModelParameters.boundingBoxDimensions.width = 1.0;
        vehicleModelParameters.boundingBoxCenter.x = 0.5;
        agentBlueprint.SetVehicleModelParameters(vehicleModelParameters);
        ON_CALL(agentBlueprintProvider, SampleAgent(_,_)).WillByDefault(Return(agentBlueprint));
    }

    ~SpawnerPreRun_IntegrationTests()
    {
        for (auto agent : agents)
        {
            delete agent;
        }
        for (auto [lane, agents] : agentsOnLane)
        {
            for (auto agent : agents)
            {
                delete agent;
            }
        }
    }

    SpawnerPreRunCommon CreateSpawner()
    {
        return SpawnerPreRunCommon{&dependencies, &callbacks};
    }

    FakeAgentFactory agentFactory;
    FakeWorld world;
    FakeAgentBlueprintProvider agentBlueprintProvider;
    FakeStochastics stochastics;
    SpawnPointDependencies dependencies{&agentFactory, &world, &agentBlueprintProvider, &stochastics};
    FakeParameter parameters;

    std::shared_ptr<FakeParameter> spawnZone = std::make_shared<FakeParameter>();
    std::shared_ptr<FakeParameter> trafficGroup1 = std::make_shared<FakeParameter>();
    std::shared_ptr<FakeParameter> trafficGroup2 = std::make_shared<FakeParameter>();
    std::map<std::string, double> trafficGroup1Double {{"Weight", 2}};
    std::map<std::string, const openpass::parameter::StochasticDistribution> trafficGroup1Stochastic {{"Velocity", openpass::parameter::UniformDistribution{10,20}},
                                                                                                      {"TGap", openpass::parameter::UniformDistribution{1,3}}};
    std::map<std::string, const std::vector<double>> trafficGroup1DoubleVector {{"Homogeneity", {0.2, 0.5}}};
    std::map<std::string, bool> trafficGroup1Bool {{"RightLaneOnly", false}};
    std::shared_ptr<FakeParameter> agentProfile1a = std::make_shared<FakeParameter>();
    std::shared_ptr<FakeParameter> agentProfile1b = std::make_shared<FakeParameter>();
    std::map<std::string, ParameterInterface::ParameterLists> trafficGroup1Lists {{"AgentProfiles", {agentProfile1a, agentProfile1b}}};
    std::map<std::string, double> trafficGroup2Double {{"Weight", 5}};
    std::map<std::string, const openpass::parameter::StochasticDistribution> trafficGroup2Stochastic {{"Velocity", openpass::parameter::UniformDistribution{20,30}},
                                                                                                      {"TGap", openpass::parameter::UniformDistribution{2,4}}};
    std::map<std::string, const std::vector<double>> trafficGroup2DoubleVector {{"Homogeneity", {0.2, 0.5}}};
    std::map<std::string, bool> trafficGroup2Bool {{"RightLaneOnly", true}};
    std::shared_ptr<FakeParameter> agentProfile2a = std::make_shared<FakeParameter>();
    std::shared_ptr<FakeParameter> agentProfile2b = std::make_shared<FakeParameter>();
    std::map<std::string, ParameterInterface::ParameterLists> trafficGroup2Lists {{"AgentProfiles", {agentProfile2a, agentProfile2b}}};

    std::map<std::string, double> agentProfile1aDouble {{"Weight", 1}};
    std::map<std::string, double> agentProfile1bDouble {{"Weight", 2}};
    std::map<std::string, double> agentProfile2aDouble {{"Weight", 3}};
    std::map<std::string, double> agentProfile2bDouble {{"Weight", 4}};
    std::map<std::string, const std::string> agentProfile1aString {{"Name", "AgentProfile1a"}};
    std::map<std::string, const std::string> agentProfile1bString {{"Name", "AgentProfile1b"}};
    std::map<std::string, const std::string> agentProfile2aString {{"Name", "AgentProfile2a"}};
    std::map<std::string, const std::string> agentProfile2bString {{"Name", "AgentProfile2b"}};

    std::map<std::string, ParameterInterface::ParameterLists> parameterLists{{"SpawnZones", {spawnZone}},
                                                                             {"TrafficGroups", {trafficGroup1, trafficGroup2}}};

    std::map<std::string, const openpass::parameter::StochasticDistribution> parametersStochastic;
    std::map<std::string, double> parametersDouble;

    FakeCallback callbacks;
    std::vector<core::Agent*> agents;
    std::map<int, std::vector<FakeAgent*>> agentsOnLane;
};

const RelativeWorldView::Lane relativeLanePlus2 {2, true, LaneType::Driving, std::nullopt, std::nullopt};
const RelativeWorldView::Lane relativeLanePlus1 {1, true, LaneType::Driving, std::nullopt, std::nullopt};
const RelativeWorldView::Lane relativeLane0 {0, true, LaneType::Driving, std::nullopt, std::nullopt};
const RelativeWorldView::Lane relativeLaneMinus1 {-1, true, LaneType::Driving, std::nullopt, std::nullopt};
const RelativeWorldView::Lane relativeLaneMinus2 {-2, true, LaneType::Driving, std::nullopt, std::nullopt};

void CheckTGap(std::vector<FakeAgent*> agentsOnLane, double expectedTGap, double minS, double maxS)
{
    for (size_t i=0; i + 1 < agentsOnLane.size(); ++i)
    {
        auto frontAgent = agentsOnLane[i+1];
        auto frontS = frontAgent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "");
        auto rearAgent = agentsOnLane[i];
        auto rearS = rearAgent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "");
        if (minS <= frontS && frontS <= maxS)
        {
            double deltaS = frontS - rearS - rearAgent->GetLength();
            double velocity = rearAgent->GetVelocity();
            EXPECT_THAT(deltaS / velocity, DoubleEq(expectedTGap));
        }
    }
}

void CheckVelocity(std::vector<FakeAgent*> agentsOnLane, double expectedVelocity, double minS, double maxS)
{
    for (size_t i=0; i + 1 < agentsOnLane.size(); ++i)
    {
        auto frontAgent = agentsOnLane[i+1];
        auto frontS = frontAgent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "");
        auto rearAgent = agentsOnLane[i];
        if (minS <= frontS && frontS <= maxS)
        {
            double velocity = rearAgent->GetVelocity();
            EXPECT_THAT(velocity, DoubleEq(expectedVelocity));
        }
    }
}

void CheckAgentProfile(std::vector<FakeAgent*> agentsOnLane, std::string expectedProfile, double minS, double maxS)
{
    for (size_t i=0; i + 1 < agentsOnLane.size(); ++i)
    {
        auto frontAgent = agentsOnLane[i+1];
        auto frontS = frontAgent->GetDistanceToStartOfRoad(MeasurementPoint::Rear, "");
        auto rearAgent = agentsOnLane[i];
        if (minS <= frontS && frontS <= maxS)
        {
            auto agentProfile = rearAgent->GetAgentTypeName();
            EXPECT_THAT(agentProfile, Eq(expectedProfile));
        }
    }
}

auto GENERATE_AGENT_ADAPTER()
{
    return [](openpass::type::FlatParameter parameter) -> std::unique_ptr<AgentInterface>
    {
        return std::make_unique<FakeAgent>();
    };
}

TEST_F(SpawnerPreRun_IntegrationTests, ThreeContinuesLanes_SpawnWithCorrectTGapAndProfiles)
{
    std::map<std::string, const std::vector<std::string>> parametersStrings {{"Roads", {ROADID}}};
    ON_CALL(*spawnZone, GetParametersStringVector()).WillByDefault(ReturnRef(parametersStrings));
    std::map<std::string, const std::vector<int>> parametersIntVector {{"Lanes", {-1,-2,-3}}};
    ON_CALL(*spawnZone, GetParametersIntVector()).WillByDefault(ReturnRef(parametersIntVector));
    std::map<std::string, double> parametersDouble {{"SStart", 1000.0}, {"SEnd", 1500.0}};
    ON_CALL(*spawnZone, GetParametersDouble()).WillByDefault(ReturnRef(parametersDouble));

    ON_CALL(world, GetDistanceToEndOfLane(_,_,_,_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {
        return RouteQueryResult<double>{{startNode, 2000.0 - initialSearchDistance}};
    });
    ON_CALL(world, IsSValidOnLane(ROADID, AllOf(Le(-1),Ge(-3)),AllOf(Ge(0),Le(2000)))).WillByDefault(Return(true));
    ON_CALL(world, GetLaneWidth(_,_,_)).WillByDefault(Return(3.0));
    ON_CALL(world, GetRoadLength(_)).WillByDefault(Return(10000.));

    RouteQueryResult<std::vector<const WorldObjectInterface*>> noObjects{{0, {}}};
    ON_CALL(world, GetObjectsInRange(_,_,_,_,_,_)).WillByDefault(Return(noObjects));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1{{0, {{0,0,{relativeLaneMinus2, relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,_,_,_)).WillByDefault(Return(relativeLanesMinus1));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2{{0, {{0,0,{relativeLaneMinus1, relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,_,_,_)).WillByDefault(Return(relativeLanesMinus2));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus3{{0, {{0,0,{relativeLane0, relativeLanePlus1, relativeLanePlus2}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-3,_,_,_)).WillByDefault(Return(relativeLanesMinus3));

    ON_CALL(world, CreateAgentAdapter(_)).WillByDefault(GENERATE_AGENT_ADAPTER());

    auto spawner = CreateSpawner();

    auto spawnedAgents = spawner.Trigger(0);

    ASSERT_THAT(agentsOnLane[-3].empty(), false);
    CheckTGap(agentsOnLane[-3], 3, 1000, 1500);
    CheckVelocity(agentsOnLane[-3], 25, 1000, 1500);
    CheckAgentProfile(agentsOnLane[-3], "AgentProfile2a", 1000, 1500);

    ASSERT_THAT(agentsOnLane[-2].empty(), false);
    CheckTGap(agentsOnLane[-2], 2, 1000, 1500);
    CheckVelocity(agentsOnLane[-2], 27, 1000, 1500);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile1a", 1000, 1500);

    ASSERT_THAT(agentsOnLane[-1].empty(), false);
    CheckTGap(agentsOnLane[-1], 2, 1000, 1500);
    CheckVelocity(agentsOnLane[-1], 40.5, 1000, 1500);
    CheckAgentProfile(agentsOnLane[-1], "AgentProfile1a", 1000, 1500);
}

TEST_F(SpawnerPreRun_IntegrationTests, IncreasingLaneNumber_SpawnWithCorrectTGapAndProfiles)
{
    std::map<std::string, const std::vector<std::string>> parametersStrings {{"Roads", {ROADID}}};
    ON_CALL(*spawnZone, GetParametersStringVector()).WillByDefault(ReturnRef(parametersStrings));
    std::map<std::string, const std::vector<int>> parametersIntVector {{"Lanes", {-1,-2,-3}}};
    ON_CALL(*spawnZone, GetParametersIntVector()).WillByDefault(ReturnRef(parametersIntVector));
    std::map<std::string, double> parametersDouble {{"SStart", 1000.0}, {"SEnd", 1500.0}};
    ON_CALL(*spawnZone, GetParametersDouble()).WillByDefault(ReturnRef(parametersDouble));

    ON_CALL(world, GetDistanceToEndOfLane(_,_,_,_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {return RouteQueryResult<double>{{startNode, 2000.0 - initialSearchDistance}};});
    ON_CALL(world, IsSValidOnLane(ROADID, AllOf(Le(-1),Ge(-2)),AllOf(Ge(0),Le(2000)))).WillByDefault(Return(true));
    ON_CALL(world, IsSValidOnLane(ROADID, -3,AllOf(Ge(1400),Le(2000)))).WillByDefault(Return(true));
    ON_CALL(world, GetLaneWidth(_,_,_)).WillByDefault(Return(3.0));
    ON_CALL(world, GetRoadLength(_)).WillByDefault(Return(10000.));

    RouteQueryResult<std::vector<const WorldObjectInterface*>> noObjects{{0, {}}};
    ON_CALL(world, GetObjectsInRange(_,_,_,_,_,_)).WillByDefault(Return(noObjects));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1First{{0, {{0,0,{relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Le(1400),_,_)).WillByDefault(Return(relativeLanesMinus1First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1Second{{0, {{0,0,{relativeLaneMinus2, relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Ge(1400),_,_)).WillByDefault(Return(relativeLanesMinus1Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2First{{0, {{0,0,{relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Le(1400),_,_)).WillByDefault(Return(relativeLanesMinus2First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2Second{{0, {{0,0,{relativeLaneMinus1, relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Ge(1400),_,_)).WillByDefault(Return(relativeLanesMinus2Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus3{{0, {{0,0,{relativeLane0, relativeLanePlus1, relativeLanePlus2}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-3,_,_,_)).WillByDefault(Return(relativeLanesMinus3));

    ON_CALL(world, CreateAgentAdapter(_)).WillByDefault(GENERATE_AGENT_ADAPTER());

    auto spawner = CreateSpawner();

    auto spawnedAgents = spawner.Trigger(0);

    ASSERT_THAT(agentsOnLane[-3].empty(), false);
    CheckTGap(agentsOnLane[-3], 3, 1400, 1500);
    CheckVelocity(agentsOnLane[-3], 25, 1400, 1500);
    CheckAgentProfile(agentsOnLane[-3], "AgentProfile2a", 1400, 1500);

    ASSERT_THAT(agentsOnLane[-2].empty(), false);
    CheckTGap(agentsOnLane[-2], 2, 1400, 1500);
    CheckVelocity(agentsOnLane[-2], 27, 1400, 1500);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile1a", 1400, 1500);
    CheckTGap(agentsOnLane[-2], 3, 1000, 1400);
    CheckVelocity(agentsOnLane[-2], 25, 1000, 1400);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile2a", 1000, 1400);

    ASSERT_THAT(agentsOnLane[-1].empty(), false);
    CheckTGap(agentsOnLane[-1], 2, 1000, 1500);
    CheckVelocity(agentsOnLane[-1], 40.5, 1400, 1500);
    CheckVelocity(agentsOnLane[-1], 27, 1000, 1400);
    CheckAgentProfile(agentsOnLane[-1], "AgentProfile1a", 1000, 1500);
}

TEST_F(SpawnerPreRun_IntegrationTests, DecreasingLaneNumber_SpawnWithCorrectTGapAndProfiles)
{
    std::map<std::string, const std::vector<std::string>> parametersStrings {{"Roads", {ROADID}}};
    ON_CALL(*spawnZone, GetParametersStringVector()).WillByDefault(ReturnRef(parametersStrings));
    std::map<std::string, const std::vector<int>> parametersIntVector {{"Lanes", {-1,-2,-3}}};
    ON_CALL(*spawnZone, GetParametersIntVector()).WillByDefault(ReturnRef(parametersIntVector));
    std::map<std::string, double> parametersDouble {{"SStart", 1000.0}, {"SEnd", 1500.0}};
    ON_CALL(*spawnZone, GetParametersDouble()).WillByDefault(ReturnRef(parametersDouble));

    ON_CALL(world, GetDistanceToEndOfLane(_,_,-3,_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {return RouteQueryResult<double>{{startNode, 1200.0 - initialSearchDistance}};});
    ON_CALL(world, GetDistanceToEndOfLane(_,_,AllOf(Le(-1),Ge(-2)),_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {return RouteQueryResult<double>{{startNode, 2000.0 - initialSearchDistance}};});
    ON_CALL(world, IsSValidOnLane(ROADID, AllOf(Le(-1),Ge(-2)),AllOf(Ge(0),Le(2000)))).WillByDefault(Return(true));
    ON_CALL(world, IsSValidOnLane(ROADID, -3,AllOf(Ge(0),Le(1200)))).WillByDefault(Return(true));
    ON_CALL(world, GetLaneWidth(_,_,_)).WillByDefault(Return(3.0));
    ON_CALL(world, GetRoadLength(_)).WillByDefault(Return(10000.));

    RouteQueryResult<std::vector<const WorldObjectInterface*>> noObjects{{0, {}}};
    ON_CALL(world, GetObjectsInRange(_,_,_,_,_,_)).WillByDefault(Return(noObjects));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1First{{0, {{0,0,{relativeLaneMinus2, relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Le(1200),_,_)).WillByDefault(Return(relativeLanesMinus1First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1Second{{0, {{0,0,{relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Ge(1200),_,_)).WillByDefault(Return(relativeLanesMinus1Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2First{{0, {{0,0,{relativeLaneMinus1, relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Le(1200),_,_)).WillByDefault(Return(relativeLanesMinus2First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2Second{{0, {{0,0,{relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Ge(1200),_,_)).WillByDefault(Return(relativeLanesMinus2Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus3{{0, {{0,0,{relativeLane0, relativeLanePlus1, relativeLanePlus2}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-3,_,_,_)).WillByDefault(Return(relativeLanesMinus3));

    ON_CALL(world, CreateAgentAdapter(_)).WillByDefault(GENERATE_AGENT_ADAPTER());

    auto spawner = CreateSpawner();

    auto spawnedAgents = spawner.Trigger(0);

    ASSERT_THAT(agentsOnLane[-3].empty(), false);
    CheckTGap(agentsOnLane[-3], 3, 1000, 1200);
    CheckVelocity(agentsOnLane[-3], 25, 1000, 1200);
    CheckAgentProfile(agentsOnLane[-3], "AgentProfile2a", 1000, 1200);

    ASSERT_THAT(agentsOnLane[-2].empty(), false);
    CheckTGap(agentsOnLane[-2], 3, 1200, 1500);
    CheckVelocity(agentsOnLane[-2], 25, 1200, 1500);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile2a", 1200, 1000);
    CheckTGap(agentsOnLane[-2], 2, 1000, 1200);
    CheckVelocity(agentsOnLane[-2], 27, 1000, 1200);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile1a", 1000, 1200);

    ASSERT_THAT(agentsOnLane[-1].empty(), false);
    CheckTGap(agentsOnLane[-1], 2, 1000, 1500);
    CheckVelocity(agentsOnLane[-1], 27, 1200, 1500);
    CheckVelocity(agentsOnLane[-1], 40.5, 1000, 1200);
    CheckAgentProfile(agentsOnLane[-1], "AgentProfile1a", 1000, 1500);
}

TEST_F(SpawnerPreRun_IntegrationTests, RightLaneStartsAndEndsWithinRange_SpawnWithCorrectTGapAndProfiles)
{
    std::map<std::string, const std::vector<std::string>> parametersStrings {{"Roads", {ROADID}}};
    ON_CALL(*spawnZone, GetParametersStringVector()).WillByDefault(ReturnRef(parametersStrings));
    std::map<std::string, const std::vector<int>> parametersIntVector {{"Lanes", {-1,-2,-3}}};
    ON_CALL(*spawnZone, GetParametersIntVector()).WillByDefault(ReturnRef(parametersIntVector));
    std::map<std::string, double> parametersDouble {{"SStart", 1000.0}, {"SEnd", 1500.0}};
    ON_CALL(*spawnZone, GetParametersDouble()).WillByDefault(ReturnRef(parametersDouble));

    ON_CALL(world, GetDistanceToEndOfLane(_,_,-3,_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {return RouteQueryResult<double>{{startNode, 1400.0 - initialSearchDistance}};});
    ON_CALL(world, GetDistanceToEndOfLane(_,_,AllOf(Le(-1),Ge(-2)),_,_,_)).WillByDefault(
                [](const RoadGraph&, RoadGraphVertex startNode, int, double initialSearchDistance, double, const LaneTypes&)
    {return RouteQueryResult<double>{{startNode, 2000.0 - initialSearchDistance}};});
    ON_CALL(world, IsSValidOnLane(ROADID, AllOf(Le(-1),Ge(-2)),AllOf(Ge(0),Le(2000)))).WillByDefault(Return(true));
    ON_CALL(world, IsSValidOnLane(ROADID, -3,AllOf(Ge(1200),Le(1400)))).WillByDefault(Return(true));
    ON_CALL(world, GetLaneWidth(_,_,_)).WillByDefault(Return(3.0));
    ON_CALL(world, GetRoadLength(_)).WillByDefault(Return(10000.));

    RouteQueryResult<std::vector<const WorldObjectInterface*>> noObjects{{0, {}}};
    ON_CALL(world, GetObjectsInRange(_,_,_,_,_,_)).WillByDefault(Return(noObjects));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1First{{0, {{0,0,{relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Ge(1400),_,_)).WillByDefault(Return(relativeLanesMinus1First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1Second{{0, {{0,0,{relativeLaneMinus2, relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,AllOf(Ge(1200),Le(1400)),_,_)).WillByDefault(Return(relativeLanesMinus1Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus1Third{{0, {{0,0,{relativeLaneMinus1, relativeLane0}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-1,Le(1200),_,_)).WillByDefault(Return(relativeLanesMinus1Third));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2First{{0, {{0,0,{relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Ge(1400),_,_)).WillByDefault(Return(relativeLanesMinus2First));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2Second{{0, {{0,0,{relativeLaneMinus1, relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,AllOf(Ge(1200),Le(1400)),_,_)).WillByDefault(Return(relativeLanesMinus2Second));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus2Third{{0, {{0,0,{relativeLane0, relativeLanePlus1}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-2,Le(1200),_,_)).WillByDefault(Return(relativeLanesMinus2Third));
    RouteQueryResult<RelativeWorldView::Lanes> relativeLanesMinus3{{0, {{0,0,{relativeLane0, relativeLanePlus1, relativeLanePlus2}}}}};
    ON_CALL(world, GetRelativeLanes(_,_,-3,_,_,_)).WillByDefault(Return(relativeLanesMinus3));

    ON_CALL(world, CreateAgentAdapter(_)).WillByDefault(GENERATE_AGENT_ADAPTER());

    auto spawner = CreateSpawner();

    auto spawnedAgents = spawner.Trigger(0);

    ASSERT_THAT(agentsOnLane[-3].empty(), false);
    CheckTGap(agentsOnLane[-3], 3, 1200, 1500);
    CheckVelocity(agentsOnLane[-3], 25, 1200, 1400);
    CheckAgentProfile(agentsOnLane[-3], "AgentProfile2a", 1200, 1400);

    ASSERT_THAT(agentsOnLane[-2].empty(), false);
    CheckTGap(agentsOnLane[-2], 3, 1400, 1500);
    CheckVelocity(agentsOnLane[-2], 25, 1400, 1500);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile2a", 1400, 1500);
    CheckTGap(agentsOnLane[-2], 2, 1200, 1400);
    CheckVelocity(agentsOnLane[-2], 27, 1200, 1400);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile1a", 1200, 1400);
    CheckTGap(agentsOnLane[-2], 3, 1000, 1200);
    CheckVelocity(agentsOnLane[-2], 25, 1000, 1200);
    CheckAgentProfile(agentsOnLane[-2], "AgentProfile2a", 1000, 1200);

    ASSERT_THAT(agentsOnLane[-1].empty(), false);
    CheckTGap(agentsOnLane[-1], 2, 1000, 1500);
    CheckVelocity(agentsOnLane[-1], 27, 1400, 1500);
    CheckVelocity(agentsOnLane[-1], 40.5, 1200, 1400);
    CheckVelocity(agentsOnLane[-1], 27, 1000, 1200);
    CheckAgentProfile(agentsOnLane[-1], "AgentProfile1a", 1000, 1500);
}
