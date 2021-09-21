/*********************************************************************
 * Copyright (c) 2018, 2019 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/eventDetectorInterface.h"
#include "include/eventDetectorNetworkInterface.h"

using namespace core;

class FakeEventDetectorNetwork : public EventDetectorNetworkInterface
{
public:
    MOCK_METHOD4(Instantiate,
                 bool(std::string, ScenarioInterface*, EventNetworkInterface*, StochasticsInterface*));
    MOCK_METHOD0(Clear,
                 void());
    MOCK_METHOD0(GetEventDetectors,
                 std::vector<const core::EventDetector*>());
    MOCK_METHOD0(ResetAll,
                 void());
};


class FakeEventDetector : public EventDetectorInterface
{
    MOCK_METHOD1(Trigger, void(int time));
    MOCK_METHOD0(GetCycleTime, int());
    MOCK_METHOD0(IsInit, bool());
    MOCK_METHOD0(Reset, void());
};
