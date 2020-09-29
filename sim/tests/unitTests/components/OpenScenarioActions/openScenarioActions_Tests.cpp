/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "common/events/laneChangeEvent.h"
#include "common/events/trajectoryEvent.h"
#include "common/trajectorySignal.h"
#include "fakeAgent.h"
#include "fakeEgoAgent.h"
#include "fakeEventNetwork.h"
#include "fakeParameter.h"
#include "fakeWorld.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "openScenarioActionsImpl.h"
#include "oscActionsCalculation.h"

using ::testing::_;
using ::testing::DoubleNear;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

using namespace openpass::events;

constexpr int TEST_LINK = 10;

class CustomEvent : public OpenScenarioEvent
{
public:
    static constexpr char TOPIC[] {"OpenSCENARIO/UserDefinedAction/CustomCommandAction/CustomEvent"};

    CustomEvent(int time, const std::string eventName, std::string source, const int agentId, const int value):
        OpenScenarioEvent{time, eventName, source, {}, {{agentId}}},
        value(value)
    {}

    const int value;
};

class CustomEventSignal : public ComponentStateSignalInterface
{
public:
    CustomEventSignal(CustomEventSignal &other) :
        CustomEventSignal(other.componentState,
                         other.value)
    {}

    CustomEventSignal()
    {
        this->componentState = ComponentState::Disabled;
    }

    CustomEventSignal(ComponentState componentState,
                     int value) :
        value(value)
    {
        this->componentState = componentState;
    }

    virtual ~CustomEventSignal()
    {}

    explicit virtual operator std::string() const {return "CustomEventSignal";}

    const int value{0};
};

struct CustomEventTransform : public openScenario::transformation::TransformerBase<CustomEventTransform, CustomEventSignal, CustomEvent>
{
    static std::shared_ptr<CustomEventSignal> ConvertToSignal(const CustomEvent &event, WorldInterface *, AgentInterface *, int)
    {
        return std::make_shared<CustomEventSignal>(ComponentState::Acting, event.value);
    }
    static inline bool registered = ActionTransformRepository::Register(Transform);
};

TEST(OpenScenarioActions_Test, MissingAssignmentForUnusedEvent_DoesNotThrow)
{
    std::map<std::string, int> fakeLinkIdAssignement = {};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;

    std::vector<EventInterface const *> events{};
    ON_CALL(fakeEventNetwork, GetTrigger(CustomEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    EXPECT_NO_THROW(openScenarioActions.UpdateOutput(0, signal, 0));
}

TEST(OpenScenarioActions_Test, MissingAssignmentForUsedEvent_Throws)
{
    std::map<std::string, int> fakeLinkIdAssignement = {};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    int fakeDeltaLaneId = -1;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;

    const auto event{std::make_shared<CustomEvent>(0, "", "", ownAgentId, fakeDeltaLaneId)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(CustomEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    EXPECT_THROW(openScenarioActions.UpdateOutput(0, signal, 0), std::runtime_error);
}

TEST(OpenScenarioActions_Test, UpdateOutputForUnassignedLinkId_Throws)
{
    std::map<std::string, int> fakeLinkIdAssignement = {};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;

    std::vector<EventInterface const *> events{};
    ON_CALL(fakeEventNetwork, GetTrigger(CustomEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    EXPECT_THROW(openScenarioActions.UpdateOutput(TEST_LINK, signal, 0), std::runtime_error);
}

TEST(OpenScenarioActions_Test, TwoEventsForSameLink_Throws)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int agentId = 10;
    const openScenario::Trajectory trajectory{{}, "MyTrajectory"};
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));

    const auto event1{std::make_shared<TrajectoryEvent>(0, "", "", agentId, trajectory)};

    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(1.0));
    FakeEgoAgent fakeEgoAgent;
    ON_CALL(fakeAgent, GetEgoAgent()).WillByDefault(ReturnRef(fakeEgoAgent));
    std::string road{"Road"};
    ON_CALL(fakeEgoAgent, GetRoadId()).WillByDefault(ReturnRef(road));
    ObjectPosition position{{{road, GlobalRoadPosition{road,-1,0,0,0}}},{},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    openScenario::LaneChangeParameter laneChange{};
    laneChange.type = openScenario::LaneChangeParameter::Type::Absolute;
    laneChange.value = -1;
    laneChange.dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Distance;
    laneChange.dynamicsTarget = 1;

    const auto event2{std::make_shared<LaneChangeEvent>(0, "", "", agentId, laneChange)};

    FakeEventNetwork fakeEventNetwork;
    std::vector<EventInterface const *> events1{event1.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(TrajectoryEvent::TOPIC)).WillByDefault(Return(events1));
    std::vector<EventInterface const *> events2{event2.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(LaneChangeEvent::TOPIC)).WillByDefault(Return(events2));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    EXPECT_THROW(openScenarioActions.UpdateOutput(0, signal, 0), std::runtime_error);
}

TEST(OpenScenarioActions_Test, TrajectoryEventForOwnAgent_IsForwardedAsSignal)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int agentId = 10;
    const openScenario::Trajectory trajectory{{}, "MyTrajectory"};
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;

    const auto event{std::make_shared<TrajectoryEvent>(0, "", "", agentId, trajectory)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(TrajectoryEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto trajectorySignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(trajectorySignal);
    ASSERT_THAT(trajectorySignal->componentState, Eq(ComponentState::Acting));
    ASSERT_THAT(trajectorySignal->trajectory.name, Eq("MyTrajectory"));
}

TEST(OpenScenarioActions_Test, TrajectoryEventForOtherAgent_IsIgnored)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    const openScenario::Trajectory trajectory{{}, "MyTractory"};
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;

    const auto event{std::make_shared<TrajectoryEvent>(0, "", "", otherAgentId, trajectory)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(TrajectoryEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto trajectorySignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(trajectorySignal);
    ASSERT_THAT(trajectorySignal->componentState, Eq(ComponentState::Disabled));
}

TEST(OpenScenarioActions_Test, LaneChangeEventForOwnAgent_IsForwardedAsSignal)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int agentId = 10;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    ON_CALL(fakeAgent, GetVelocity()).WillByDefault(Return(1.0));
    FakeEgoAgent fakeEgoAgent;
    ON_CALL(fakeAgent, GetEgoAgent()).WillByDefault(ReturnRef(fakeEgoAgent));
    std::string road{"Road"};
    ON_CALL(fakeEgoAgent, GetRoadId()).WillByDefault(ReturnRef(road));
    ObjectPosition position{{{road, GlobalRoadPosition{road,-1,0,0,0}}},{},{}};
    ON_CALL(fakeAgent, GetObjectPosition()).WillByDefault(ReturnRef(position));
    FakeWorld fakeWorld;
    FakeEventNetwork fakeEventNetwork;
    openScenario::LaneChangeParameter laneChange{};
    laneChange.type = openScenario::LaneChangeParameter::Type::Absolute;
    laneChange.value = -2;
    laneChange.dynamicsType = openScenario::LaneChangeParameter::DynamicsType::Distance;
    laneChange.dynamicsTarget = 1;

    const auto event{std::make_shared<LaneChangeEvent>(0, "", "", agentId, laneChange)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(LaneChangeEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 100,
                                                                 nullptr,
                                                                 &fakeWorld,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(laneChangeSignal);
    ASSERT_THAT(laneChangeSignal->componentState, Eq(ComponentState::Acting));
}

TEST(OpenScenarioActions_Test, LaneChangeEventForOtherAgent_IsIgnored)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;
    openScenario::LaneChangeParameter laneChange{};

    const auto event{std::make_shared<LaneChangeEvent>(0, "", "", otherAgentId, laneChange)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(LaneChangeEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(0, signal, 0);

    const auto laneChangeSignal = std::dynamic_pointer_cast<const TrajectorySignal>(signal);
    ASSERT_TRUE(laneChangeSignal);
    ASSERT_THAT(laneChangeSignal->componentState, Eq(ComponentState::Disabled));
}

TEST(OpenScenarioActions_Test, CustomEventForOwnAgent_IsForwardedAsSignal)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int agentId = 10;
    int fakeValue = -1;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(agentId));
    FakeEventNetwork fakeEventNetwork;

    const auto event{std::make_shared<CustomEvent>(0, "", "", agentId, fakeValue)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(CustomEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(TEST_LINK, signal, 0);

    const auto customLaneChangeSignal = std::dynamic_pointer_cast<const CustomEventSignal>(signal);
    ASSERT_TRUE(customLaneChangeSignal);
    ASSERT_THAT(customLaneChangeSignal->componentState, Eq(ComponentState::Acting));
    ASSERT_THAT(customLaneChangeSignal->value, fakeValue);
}

TEST(OpenScenarioActions_Test, CustomEventForOtherAgent_IsIgnored)
{
    std::map<std::string, int> fakeLinkIdAssignement = {
        {CustomEvent::TOPIC, TEST_LINK}};

    NiceMock<FakeParameter> fakeParameter;
    ON_CALL(fakeParameter, GetParametersInt()).WillByDefault(ReturnRef(fakeLinkIdAssignement));

    constexpr int ownAgentId = 10;
    constexpr int otherAgentId = 11;
    int fakeDeltaLaneId = -1;
    FakeAgent fakeAgent;
    ON_CALL(fakeAgent, GetId()).WillByDefault(Return(ownAgentId));
    FakeEventNetwork fakeEventNetwork;

    const auto event{std::make_shared<CustomEvent>(0, "", "", otherAgentId, fakeDeltaLaneId)};
    std::vector<EventInterface const *> events{event.get()};
    ON_CALL(fakeEventNetwork, GetTrigger(CustomEvent::TOPIC)).WillByDefault(Return(events));

    auto openScenarioActions = OpenScenarioActionsImplementation("",
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeParameter,
                                                                 nullptr,
                                                                 nullptr,
                                                                 &fakeAgent,
                                                                 &fakeEventNetwork);

    openScenarioActions.Trigger(0);

    std::shared_ptr<SignalInterface const> signal;
    openScenarioActions.UpdateOutput(TEST_LINK, signal, 0);

    const auto customLaneChangeSignal = std::dynamic_pointer_cast<const CustomEventSignal>(signal);
    ASSERT_TRUE(customLaneChangeSignal);
    ASSERT_THAT(customLaneChangeSignal->componentState, Eq(ComponentState::Disabled));
    ASSERT_THAT(customLaneChangeSignal->value, 0);
}

struct OscActionsCalculation_LaneChangeData
{
    double deltaS;
    double deltaT;
    double deltaTime;
};

class OscActionCalculation_LaneChangeTest : public ::testing::TestWithParam<OscActionsCalculation_LaneChangeData>
{
};

TEST_P(OscActionCalculation_LaneChangeTest, CorrectStartAndEndConstraints)
{
    auto data = GetParam();
    GlobalRoadPosition startPosition{"Road", -2, 10., 11., 0.5};

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, RoadCoord2WorldCoord(_, "Road")).WillByDefault([](auto roadCoord, std::string) { return Position{roadCoord.s + 1.0, roadCoord.t + 2.0, roadCoord.hdg + 0.1, 0.0}; });

    OscActionsCalculation calculation{&fakeWorld};

    auto trajectoryResult = calculation.CalculateSinusiodalLaneChange(data.deltaS, data.deltaT, data.deltaTime, 0.1, startPosition, 1.5);

    const auto &startPoint = trajectoryResult.points.front();
    ASSERT_THAT(startPoint.time, DoubleNear(1.5, 1e-3));
    ASSERT_THAT(startPoint.x, DoubleNear(11.0, 1e-3));
    ASSERT_THAT(startPoint.y, DoubleNear(13.0, 1e-3));
    ASSERT_THAT(startPoint.yaw, DoubleNear(0.1, 1e-3));

    const auto &endPoint = trajectoryResult.points.back();
    ASSERT_THAT(endPoint.time, DoubleNear(1.5 + data.deltaTime, 1e-3));
    ASSERT_THAT(endPoint.x, DoubleNear(11.0 + data.deltaS, 1e-3));
    ASSERT_THAT(endPoint.y, DoubleNear(13.0 + data.deltaT, 1e-3));
    ASSERT_THAT(endPoint.yaw, DoubleNear(0.1, 1e-3));
}

TEST_P(OscActionCalculation_LaneChangeTest, ShapeIsSineCurve)
{
    auto data = GetParam();
    GlobalRoadPosition startPosition{"Road", -2, 10., 11., 0.5};

    FakeWorld fakeWorld;
    ON_CALL(fakeWorld, RoadCoord2WorldCoord(_, "Road")).WillByDefault([](auto roadCoord, std::string) { return Position{roadCoord.s + 1.0, roadCoord.t + 2.0, roadCoord.hdg + 0.1, 0.0}; });

    OscActionsCalculation calculation{&fakeWorld};

    auto trajectoryResult = calculation.CalculateSinusiodalLaneChange(data.deltaS, data.deltaT, data.deltaTime, 0.1, startPosition, 1.5);

    for (const auto &point : trajectoryResult.points)
    {
        const auto s = point.x - 11.0;
        const auto t = point.y - 13.0;
        ASSERT_THAT(t, DoubleNear(data.deltaT * 0.5 * (1 - std::cos(s * M_PI / data.deltaS)), 1e-3));
    }
}

INSTANTIATE_TEST_CASE_P(OscActionCalculation_LaneChangeTest, OscActionCalculation_LaneChangeTest, testing::Values(OscActionsCalculation_LaneChangeData{10.0, 4.0, 2.0}, OscActionsCalculation_LaneChangeData{20.0, -3.0, 10.0}, OscActionsCalculation_LaneChangeData{5.0, 5.0, 0.5}));
