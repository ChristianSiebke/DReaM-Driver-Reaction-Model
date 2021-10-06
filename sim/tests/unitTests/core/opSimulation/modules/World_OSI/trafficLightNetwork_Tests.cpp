/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "OWL/fakes/fakeTrafficLight.h"
#include "TrafficLightNetwork.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::SaveArg;
using ::testing::InSequence;

TEST(TrafficLightNetwork, TrafficLightControllerConstructor_SetsInitialState)
{
    OWL::Fakes::TrafficLight trafficLight1;
    OWL::Fakes::TrafficLight trafficLight2;

    TrafficLightController::Phase phase1 {10,
                                          {{&trafficLight1, CommonTrafficLight::State::Red},
                                           {&trafficLight2, CommonTrafficLight::State::Green}}};
    TrafficLightController::Phase phase2 {10,
                                          {{&trafficLight1, CommonTrafficLight::State::Green},
                                           {&trafficLight2, CommonTrafficLight::State::Red}}};

    EXPECT_CALL(trafficLight1, SetState(CommonTrafficLight::State::Red));
    EXPECT_CALL(trafficLight1, SetState(Ne(CommonTrafficLight::State::Red))).Times(0);
    EXPECT_CALL(trafficLight2, SetState(CommonTrafficLight::State::Green));
    EXPECT_CALL(trafficLight2, SetState(Ne(CommonTrafficLight::State::Green))).Times(0);

    TrafficLightController trafficLightController{{{phase1, phase2}}, 0};
}

TEST(TrafficLightNetwork, UpdateStates_SetsCorrectStates)
{
    OWL::Fakes::TrafficLight trafficLight1;
    OWL::Fakes::TrafficLight trafficLight2;

    TrafficLightController::Phase phase1 {1,
                                          {{&trafficLight1, CommonTrafficLight::State::Red},
                                           {&trafficLight2, CommonTrafficLight::State::Green}}};
    TrafficLightController::Phase phase2 {2,
                                          {{&trafficLight1, CommonTrafficLight::State::RedYellow},
                                           {&trafficLight2, CommonTrafficLight::State::Yellow}}};
    TrafficLightController::Phase phase3 {1,
                                          {{&trafficLight1, CommonTrafficLight::State::Green},
                                           {&trafficLight2, CommonTrafficLight::State::Red}}};

    CommonTrafficLight::State state1;
    CommonTrafficLight::State state2;
    ON_CALL(trafficLight1, SetState(_)).WillByDefault(SaveArg<0>(&state1));
    ON_CALL(trafficLight2, SetState(_)).WillByDefault(SaveArg<0>(&state2));

    TrafficLightController trafficLightController{{{phase1, phase2, phase3}}, 0};
    TrafficLightNetwork network;
    network.AddController(std::move(trafficLightController));

    network.UpdateStates(1);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::RedYellow));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Yellow));

    network.UpdateStates(2);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::RedYellow));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Yellow));

    network.UpdateStates(3);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Green));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Red));

    network.UpdateStates(4);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Red));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Green));
}

TEST(TrafficLightNetwork, UpdateStatesWithDelay_SetsCorrectStates)
{
    OWL::Fakes::TrafficLight trafficLight1;
    OWL::Fakes::TrafficLight trafficLight2;

    TrafficLightController::Phase phase1 {1,
                                          {{&trafficLight1, CommonTrafficLight::State::Red},
                                           {&trafficLight2, CommonTrafficLight::State::Green}}};
    TrafficLightController::Phase phase2 {2,
                                          {{&trafficLight1, CommonTrafficLight::State::RedYellow},
                                           {&trafficLight2, CommonTrafficLight::State::Yellow}}};
    TrafficLightController::Phase phase3 {1,
                                          {{&trafficLight1, CommonTrafficLight::State::Green},
                                           {&trafficLight2, CommonTrafficLight::State::Red}}};

    CommonTrafficLight::State state1;
    CommonTrafficLight::State state2;
    ON_CALL(trafficLight1, SetState(_)).WillByDefault(SaveArg<0>(&state1));
    ON_CALL(trafficLight2, SetState(_)).WillByDefault(SaveArg<0>(&state2));

    TrafficLightController trafficLightController{{{phase1, phase2, phase3}}, 2};
    TrafficLightNetwork network;
    network.AddController(std::move(trafficLightController));

    network.UpdateStates(1);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Red));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Green));

    network.UpdateStates(2);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Red));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Green));

    network.UpdateStates(3);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::RedYellow));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Yellow));

    network.UpdateStates(4);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::RedYellow));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Yellow));

    network.UpdateStates(5);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Green));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Red));

    network.UpdateStates(6);
    EXPECT_THAT(state1, Eq(CommonTrafficLight::State::Red));
    EXPECT_THAT(state2, Eq(CommonTrafficLight::State::Green));
}
