/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "dontCare.h"

#include "dontCare.h"
#include "fakeCallback.h"
#include "fakeEventNetwork.h"
#include "fakeAgent.h"

#include "condition.h"
#include "stateManager.h"
#include "componentControllerImpl.h"
#include "common/agentCompToCompCtrlSignal.h"
#include "common/compCtrlToAgentCompSignal.h"
#include "common/events/componentStateChangeEvent.h"

using ::testing::_;
using ::testing::DontCare;
using ::testing::Ne;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SizeIs;

using namespace ComponentControl;

TEST(ComponentController_UpdateOutput, HasComponentWarningSignal_ForwardsWarningToDriver)
{
    NiceMock<FakeAgent> fakeAgent;
    NiceMock<FakeEventNetwork> fakeEventNetwork;
    ComponentControllerImplementation componentController{DontCare<std::string>(),
                                                          DontCare<bool>(),
                                                          DontCare<int>(),
                                                          DontCare<int>(),
                                                          DontCare<int>(),
                                                          DontCare<int>(),
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          nullptr,
                                                          &fakeAgent,
                                                          &fakeEventNetwork};

    // register linkId 0 as an undefined component in statemanager
    const int testLinkId = 0;
    const std::string testComponentName = "testComponent";
    const std::shared_ptr<SignalInterface const> testRegistrationData = std::make_shared<AgentCompToCompCtrlSignal const>(ComponentType::Undefined,
                                                                                                                      testComponentName,
                                                                                                                      ComponentState::Acting);
    componentController.UpdateInput(testLinkId, testRegistrationData, DontCare<int>());

    // register linkId 1 as a driver component in statemanager
    const int driverLinkId = 1;
    const std::string driverComponentName = "driverComponent";
    const std::shared_ptr<SignalInterface const> driverRegistrationData = std::make_shared<AgentCompToCompCtrlSignal const>(ComponentType::Driver,
                                                                                                                            driverComponentName,
                                                                                                                            ComponentState::Acting);
    componentController.UpdateInput(driverLinkId, driverRegistrationData, DontCare<int>());

    // send test signal
    const ComponentWarningInformation warning{true,
                                              ComponentWarningLevel::WARNING,
                                              ComponentWarningType::OPTIC,
                                              ComponentWarningIntensity::HIGH};
    std::vector<ComponentWarningInformation> warnings{warning};
    const std::shared_ptr<SignalInterface const> data = std::make_shared<AgentCompToCompCtrlSignal const>(ComponentType::Undefined,
                                                                                                          testComponentName,
                                                                                                          ComponentState::Acting,
                                                                                                          warnings);
    // send warning to componentController
    componentController.UpdateInput(testLinkId, data, DontCare<int>());

    // assert forwarding of warning to driver
    std::shared_ptr<SignalInterface const> compCtrlToDriverData;
    componentController.UpdateOutput(driverLinkId, compCtrlToDriverData, DontCare<int>());

    std::shared_ptr<CompCtrlToDriverCompSignal const> castedCompCtrlToDriverData;
    castedCompCtrlToDriverData = std::dynamic_pointer_cast<CompCtrlToDriverCompSignal const>(compCtrlToDriverData);
    bool compCtrlToDriverDataCastSuccessful = castedCompCtrlToDriverData == nullptr ? false : true;

    ASSERT_THAT(compCtrlToDriverDataCastSuccessful, true);

    const auto& warningsInDataSignal = castedCompCtrlToDriverData->GetWarnings();
    ASSERT_THAT(warningsInDataSignal, SizeIs(1));

    const auto& warningsFromTestComponent = warningsInDataSignal.at(testComponentName);
    ASSERT_THAT(warningsFromTestComponent, SizeIs(1));

    const auto& warningInData = warningsFromTestComponent.front();
    ASSERT_THAT(warningInData.activity, warning.activity);
    ASSERT_THAT(warningInData.level, warning.level);
    ASSERT_THAT(warningInData.type, warning.type);
    ASSERT_THAT(warningInData.intensity, warning.intensity);
}

TEST(StateManager_GetComponent, NoComponentsAdded_Throws)
{
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);

    ASSERT_THROW(stateManager.GetComponent(localLinkId), std::out_of_range);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, AgentComponentWithoutConditions_ReturnsDefaultState)
{
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    std::shared_ptr<ComponentStateInformation> componentStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::TrajectoryFollower,
                                                                                                                       "testComponent",
                                                                                                                       ComponentState::Undefined);
    stateManager.AddComponent(localLinkId, componentStateInformation);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkId), ComponentState::Undefined);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, VehicleComponentWithoutConditions_ReturnsDefaultState)
{
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    std::shared_ptr<ComponentStateInformation> componentStateInformation = std::shared_ptr<ComponentStateInformation>(new AdasComponentStateInformation(ComponentType::VehicleComponent,
                                                                                                                                                        "testComponent",
                                                                                                                                                        ComponentState::Undefined,
                                                                                                                                                        AdasType::Safety));
    stateManager.AddComponent(localLinkId, componentStateInformation);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkId), ComponentState::Acting);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, ComponentStateEqualityTrue_ReturnsCorrectState)
{
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    std::shared_ptr<ComponentStateInformation> componentStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::VehicleComponent,
                                                        "TestComponent",
                                                        ComponentState::Acting);

    std::unique_ptr<FixedComponentStateExpression> componentStateActing(new FixedComponentStateExpression(ComponentState::Acting));
    std::unique_ptr<VehicleComponentStateExpression> componentStateTestComponent(new VehicleComponentStateExpression("TestComponent"));
    std::unique_ptr<Condition> componentStateEquality(new ComponentStateEquality(std::move(componentStateActing),
                                                                                 std::move(componentStateTestComponent)));
    componentStateInformation->AddStateCondition(std::move(componentStateEquality), ComponentState::Acting);
    stateManager.AddComponent(localLinkId, componentStateInformation);

    std::vector<const openpass::events::ComponentStateChangeEvent *> placeholderEventList {};
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(placeholderEventList);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkId), ComponentState::Acting);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, ComponentStateEqualityFalse_ReturnsDefaultState)
{
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    std::shared_ptr<ComponentStateInformation> componentStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::VehicleComponent,
                                                        "TestComponent",
                                                        ComponentState::Armed);
    std::unique_ptr<FixedComponentStateExpression> componentStateActing(new FixedComponentStateExpression(ComponentState::Acting));
    std::unique_ptr<VehicleComponentStateExpression> componentStateTestComponent(new VehicleComponentStateExpression("TestComponent"));
    std::unique_ptr<Condition> componentStateEquality(new ComponentStateEquality(std::move(componentStateActing),
                                                                                 std::move(componentStateTestComponent)));
    componentStateInformation->AddStateCondition(std::move(componentStateEquality), ComponentState::Acting);
    stateManager.AddComponent(localLinkId, componentStateInformation);

    std::vector<const openpass::events::ComponentStateChangeEvent *> placeholderEventList {};
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(placeholderEventList);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkId), ComponentState::Undefined);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, TwoConditionsFullfilled_FirstFilledWins)
{
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    const int localLinkIdA = 0;
    std::shared_ptr<ComponentStateInformation> testComponentAStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::VehicleComponent,
                                                             "TestComponentA",
                                                             ComponentState::Acting);
    const int localLinkIdB = 1;
    std::shared_ptr<ComponentStateInformation> testComponentBStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::VehicleComponent,
                                                             "TestComponentB",
                                                             ComponentState::Acting);

    std::unique_ptr<FixedComponentStateExpression> componentStateActing(new FixedComponentStateExpression(ComponentState::Acting));
    std::unique_ptr<VehicleComponentStateExpression> componentStateTestComponentA(new VehicleComponentStateExpression("TestComponentA"));
    std::unique_ptr<Condition> componentStateEqualityA(new ComponentStateEquality(std::move(componentStateActing),
                                                                                              std::move(componentStateTestComponentA)));
    testComponentAStateInformation->AddStateCondition(std::move(componentStateEqualityA), ComponentState::Armed);

    std::unique_ptr<VehicleComponentStateExpression> componentStateTestComponentB(new VehicleComponentStateExpression("TestComponentB"));
    std::unique_ptr<Condition> componentStateEqualityB(new ComponentStateEquality(std::move(componentStateActing),
                                                                                              std::move(componentStateTestComponentB)));
    testComponentAStateInformation->AddStateCondition(std::move(componentStateEqualityB), ComponentState::Disabled);

    stateManager.AddComponent(localLinkIdA, testComponentAStateInformation);
    stateManager.AddComponent(localLinkIdB, testComponentBStateInformation);

    std::vector<const openpass::events::ComponentStateChangeEvent *> placeholderEventList {};
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(placeholderEventList);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkIdA), ComponentState::Armed);
}

TEST(StateManager_GetMaxReachableStateOfComponentAtLocalLinkId, ComponentMaxStateSetByEvent_ConditionsIgnored)
{
    const std::string componentName = "TestComponent";
    const int localLinkId = 0;
    FakeCallback fakeCallback;
    StateManager stateManager(&fakeCallback);
    std::shared_ptr<ComponentStateInformation> componentStateInformation = std::make_shared<ComponentStateInformation>(ComponentType::VehicleComponent,
                                                        componentName,
                                                        ComponentState::Acting);

    std::unique_ptr<FixedComponentStateExpression> componentStateActing(new FixedComponentStateExpression(ComponentState::Acting));
    std::unique_ptr<VehicleComponentStateExpression> componentStateTestComponent(new VehicleComponentStateExpression("TestComponent"));
    std::unique_ptr<Condition> componentStateEquality(new ComponentStateEquality(std::move(componentStateActing),
                                                                                 std::move(componentStateTestComponent)));
    componentStateInformation->AddStateCondition(std::move(componentStateEquality), ComponentState::Acting);
    stateManager.AddComponent(localLinkId, componentStateInformation);

    openpass::type::TriggeringEntities triggering {};

    openpass::type::AgentId agentId {0};
    openpass::type::AffectedEntities affected {};
    affected.entities.push_back(agentId);

    auto event = std::make_unique<openpass::events::ComponentStateChangeEvent const>(0,
                                                                   "",
                                                                   "",
                                                                triggering,
                                                                   affected,
                                                                   componentName,
                                                                   ComponentState::Disabled);
    std::vector<const openpass::events::ComponentStateChangeEvent *> placeholderEventList {};
    placeholderEventList.push_back(event.get());
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(placeholderEventList);

    ASSERT_EQ(stateManager.GetMaxReachableStateOfComponentAtLocalLinkId(localLinkId), ComponentState::Disabled);
}
