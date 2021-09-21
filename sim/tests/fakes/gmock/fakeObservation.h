/*********************************************************************
 * Copyright (c) 2018, 2019, 2020 in-tech
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 **********************************************************************/

#pragma once

#include "gmock/gmock.h"

class FakeObservation : public ObservationInterface
{
public:
    MOCK_METHOD0(OpSimulationManagerPreHook,
                 void());
    MOCK_METHOD1(OpSimulationManagerPostHook,
                 void(const std::string& filename));
    MOCK_METHOD0(OpSimulationPreHook,
                 void());
    MOCK_METHOD0(OpSimulationPreRunHook,
                 void());
    MOCK_METHOD2(OpSimulationUpdateHook,
                 void(int, RunResultInterface&));
    MOCK_METHOD1(OpSimulationPostRunHook,
                 void(const RunResultInterface&));
    MOCK_METHOD0(OpSimulationPostHook,
                 void());
    MOCK_METHOD0(OpSimulationResultFile,
                 const std::string());
    MOCK_METHOD5(Insert,
                 void(int, int, LoggingGroup, const std::string&, const std::string&));
    MOCK_METHOD1(InsertEvent,
                 void(std::shared_ptr<EventInterface>));
};


