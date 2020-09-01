/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "dontCare.h"

#include "fakePublisher.h"
#include "fakeParameter.h"
#include "fakeAgent.h"
#include "fakeWorld.h"

#include "tfImplementation.h"

using ::testing::NiceMock;
using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::DontCare;

class TrajectoryTester
{
public:
    static const int CC_LOCAL_LINK_ID = 83;

    std::vector<int> fakeActingAgents {1};

    EventContainer fakeEmptyEventContainer = {};

    NiceMock<FakePublisher> fakePublisher;

    std::map<std::string, bool> fakeBools;
    NiceMock<FakeParameter> fakeParameters;

    NiceMock<FakeAgent> fakeAgent;

    std::shared_ptr<TrajectoryFollowerImplementation> trajectoryFollower;

    TrajectoryTester(const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     FakeWorld *fakeWorld,
                     const int cycleTime = 100);

    TrajectoryTester(const bool enforceTrajectory,
                     const bool automaticDeactivation,
                     FakeWorld *fakeWorld,
                     FakeAgent *fakeAgent,
                     const int cycleTime = 100);
};
