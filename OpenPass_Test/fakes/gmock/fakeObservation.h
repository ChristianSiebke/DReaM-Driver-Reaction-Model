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
#include "Interfaces/observationInterface.h"

class FakeObservation : public ObservationInterface
{
public:
    MOCK_METHOD0(MasterPreHook,
                 void());
    MOCK_METHOD1(MasterPostHook,
                 void(const std::string& filename));
    MOCK_METHOD1(SlavePreHook,
                 void(const std::string& path));
    MOCK_METHOD0(SlavePreRunHook,
                 void());
    MOCK_METHOD2(SlaveUpdateHook,
                 void(int, RunResultInterface&));
    MOCK_METHOD1(SlavePostRunHook,
                 void(const RunResultInterface&));
    MOCK_METHOD0(SlavePostHook,
                 void());
    MOCK_METHOD0(SlaveResultFile,
                 const std::string());
    MOCK_METHOD5(Insert,
                 void(int, int, LoggingGroup, const std::string&, const std::string&));
    MOCK_METHOD1(InsertEvent,
                 void(std::shared_ptr<EventInterface>));
    MOCK_METHOD0(GatherFollowers,
                 void());
    MOCK_METHOD1(InformObserverOnSpawn,
                 void(AgentInterface*));
};


