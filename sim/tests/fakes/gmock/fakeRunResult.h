/********************************************************************************
 * Copyright (c) 2019-2021 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "include/runResultInterface.h"

class FakeRunResult : public RunResultInterface {
 public:
    MOCK_METHOD0(SetEndCondition, void());
    MOCK_CONST_METHOD0(GetCollisionIds, const std::vector<int>*());
    MOCK_CONST_METHOD0(GetPositions, const std::map<const AgentInterface*, std::tuple<double, double>>*());
    MOCK_CONST_METHOD0(GetVelocities, const std::map<const AgentInterface*, std::tuple<double, double>>*());
    MOCK_CONST_METHOD0(GetYaws, const std::map<const AgentInterface*, double>*());
    MOCK_CONST_METHOD0(GetDistances, const std::map<const AgentInterface*, double>*());
    MOCK_CONST_METHOD0(IsCollision, bool());
    MOCK_CONST_METHOD0(IsTimeOver, bool());
    MOCK_CONST_METHOD0(IsEndCondition, bool());
    MOCK_METHOD1(AddCollisionId, void(const int));
};
