/********************************************************************************
 * Copyright (c) 2017-2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "signalPrioritizerImpl.h"

#include "fakeParameter.h"
#include "dontCare.h"

using ::testing::ReturnRef;
using ::testing::DontCare;
using ::testing::NiceMock;

struct SignalPrioritizer_UnitTests : public ::testing::Test
{
    std::shared_ptr<SignalInterface const> outputSignal;

    NiceMock<FakeParameter> stubParameters;
    std::map<std::string, int> fakeSenderPriorities = {{"0", 10}, {"1", 100}, {"2", 1000}};
    std::shared_ptr<SignalPrioritizerImplementation> signalPrioritizer;

    SignalPrioritizer_UnitTests()
    {
        ON_CALL(stubParameters, GetParametersInt()).WillByDefault(ReturnRef(fakeSenderPriorities));
        signalPrioritizer = std::make_shared<SignalPrioritizerImplementation>(
                    "SignalPrioritizer", false ,0, 0, 0, 0,
                    nullptr, &stubParameters, nullptr, nullptr, nullptr);
    }
};

struct FakeComponentSignal : public ComponentStateSignalInterface
{
    FakeComponentSignal(ComponentState componentState)
    {
        this->componentState = componentState;
    }

    explicit operator std::string() const override { return ""; }

    static std::shared_ptr<FakeComponentSignal const> AsSharedPtr(ComponentState componentState)
    {
        return std::make_shared<FakeComponentSignal const>(componentState);
    }
};

TEST_F(SignalPrioritizer_UnitTests, OneActiveSignal_IsHold)
{
    auto activeSignal = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);

    signalPrioritizer->UpdateInput(0, activeSignal, 0);
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);

    ASSERT_EQ(activeSignal, outputSignal);
}

TEST_F(SignalPrioritizer_UnitTests, ThreeActiveSignals_CorrectHighestPrioritySignal)
{
    auto activeSignal1 = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);
    auto activeSignal2 = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);
    auto activeSignal3 = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);

    signalPrioritizer->UpdateInput(0, activeSignal1, 0);
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);
    ASSERT_EQ(activeSignal1, outputSignal);

    signalPrioritizer->UpdateInput(1, activeSignal2, 0);
    signalPrioritizer->UpdateInput(0, activeSignal1, 0);
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);
    ASSERT_EQ(activeSignal2, outputSignal);

    signalPrioritizer->UpdateInput(2, activeSignal3, 0);
    signalPrioritizer->UpdateInput(1, activeSignal2, 0);
    signalPrioritizer->UpdateInput(0, activeSignal1, 0);
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);
    ASSERT_EQ(activeSignal3, outputSignal);
}

TEST_F(SignalPrioritizer_UnitTests, SignalWithHigherPriorityGetsDisabledAndSwitchToLowerPrioritySignal)
{
    auto activeSignal1 = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);
    auto activeSignal2 = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);
    auto disabledSignal1 = FakeComponentSignal::AsSharedPtr(ComponentState::Disabled);

    signalPrioritizer->UpdateInput(1, activeSignal1, 0);
    signalPrioritizer->UpdateOutput(0, outputSignal, 0);
    ASSERT_EQ(activeSignal1, outputSignal);

    signalPrioritizer->UpdateInput(0, activeSignal2, 100);
    signalPrioritizer->UpdateOutput(0, outputSignal, 100);
    ASSERT_EQ(activeSignal1, outputSignal);

    signalPrioritizer->UpdateInput(1, disabledSignal1, 200);
    signalPrioritizer->UpdateOutput(0, outputSignal, 200);
    ASSERT_EQ(activeSignal2, outputSignal);
}

TEST_F(SignalPrioritizer_UnitTests, OneActiveAndOnePassiveSignalWithHigherPriority_ActiveSignalIsHold)
{
    auto activeSignal = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);
    auto passiveSignal = FakeComponentSignal::AsSharedPtr(ComponentState::Armed);

    signalPrioritizer->UpdateInput(0, activeSignal, 0);
    signalPrioritizer->UpdateInput(1, passiveSignal, 0);
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);
    ASSERT_EQ(activeSignal, outputSignal);
}

TEST_F(SignalPrioritizer_UnitTests, ActiveSignalWithUnknownChannelId_ThrowsException)
{
    auto unknownSignal = FakeComponentSignal::AsSharedPtr(ComponentState::Acting);

     ASSERT_THROW(signalPrioritizer->UpdateInput(3, unknownSignal, 0);, std::runtime_error);
}

TEST_F(SignalPrioritizer_UnitTests, UpdateOutputWithoutUpdateInput_YieldsDummySignal)
{
    signalPrioritizer->UpdateOutput(DontCare<int>(), outputSignal, 0);
    ASSERT_EQ(std::dynamic_pointer_cast<ComponentStateSignalInterface const>(outputSignal)->componentState, ComponentState::Undefined);
}
