/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#include "gtest/gtest.h"

#include "trajectoryTester.h"

#include "trajectoryFollowerCommonBase.h"

TEST(TrajectoryFollowerCommonBase_UpdateInput, ShouldNotReactivateAfterDeactivation)
{
    std::vector<int> fakeActingAgents {1};
    std::shared_ptr<ComponentChangeEvent> fakeActivationEvent = std::make_shared<ComponentChangeEvent>(0,
                                                                                             DontCare<std::string>(),
                                                                                             DontCare<std::string>(),
                                                                                             EventDefinitions::EventType::ComponentStateChange,
                                                                                             DontCare<std::vector<int>>(),
                                                                                             fakeActingAgents,
                                                                                             "Dynamics_TrajectoryFollower",
                                                                                             "Acting");

    std::shared_ptr<ComponentChangeEvent> fakeDeactivationEvent = std::make_shared<ComponentChangeEvent>(0,
                                                                                             DontCare<std::string>(),
                                                                                             DontCare<std::string>(),
                                                                                             EventDefinitions::EventType::ComponentStateChange,
                                                                                             DontCare<std::vector<int>>(),
                                                                                             fakeActingAgents,
                                                                                             "Dynamics_TrajectoryFollower",
                                                                                             "Disabled");

    EventContainer fakeActivatingEventContainer = {fakeActivationEvent};
    EventContainer fakeDeactivatingEventContainer = {fakeDeactivationEvent};
    EventContainer fakeEmptyEventContainer = {};

    NiceMock<FakeEventNetwork> fakeEventNetwork;
    EXPECT_CALL(fakeEventNetwork, GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange))
                                    .WillOnce(Return(fakeActivatingEventContainer))
                                    .WillOnce(Return(fakeDeactivatingEventContainer))
                                    .WillOnce(Return(fakeActivatingEventContainer));

    TrajectoryTester<TestTrajectoryFollowerCommonBaseImpl> trajectoryTester(100,
                                                                            &fakeEventNetwork);
    std::shared_ptr<TestTrajectoryFollowerCommonBaseImpl> trajectoryFollower = trajectoryTester.trajectoryFollower;

    trajectoryFollower->Trigger(0);
    ASSERT_EQ(trajectoryFollower->GetComponentState(), ComponentState::Acting);

    trajectoryFollower->Trigger(100);
    ASSERT_EQ(trajectoryFollower->GetComponentState(), ComponentState::Disabled);

    trajectoryFollower->Trigger(200);
    ASSERT_EQ(trajectoryFollower->GetComponentState(), ComponentState::Disabled);
}
