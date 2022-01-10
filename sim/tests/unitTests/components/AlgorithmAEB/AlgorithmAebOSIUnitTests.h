/********************************************************************************
 * Copyright (c) 2019 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "autonomousEmergencyBraking.h"
#include "fakeAgent.h"
#include "fakeParameter.h"
#include "fakePublisher.h"
#include "fakeStochastics.h"
#include "fakeWorld.h"
#include "fakeWorldObject.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::NiceMock;

class AlgorithmAutonomousEmergencyBraking_UnitTest : public ::testing::Test
{
public:
    AlgorithmAutonomousEmergencyBraking_UnitTest();

    ~AlgorithmAutonomousEmergencyBraking_UnitTest()
    {
        delete implementation;
    }

    void SetEgoValues(double velocity, double acceleration, double yawRate);

    AlgorithmAutonomousEmergencyBrakingImplementation *implementation;
    NiceMock<FakeWorld> fakeWorldInterface;
    NiceMock<FakeAgent> fakeEgoAgent;
    NiceMock<FakePublisher> fakePublisher;
};
