/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/
#pragma once

#include "gmock/gmock.h"
#include "Interfaces/eventNetworkInterface.h"

class FakeEventNetwork : public SimulationSlave::EventNetworkInterface
{
public:
    MOCK_METHOD0(GetActiveEvents,
        Events*());
    MOCK_METHOD0(GetArchivedEvents,
        Events*());
    MOCK_METHOD1(GetActiveEventCategory,
        EventContainer(const EventDefinitions::EventCategory));
    MOCK_METHOD1(RemoveOldEvents,
        void(int));
    MOCK_METHOD1(InsertEvent,
        void(std::shared_ptr<EventInterface>));
    MOCK_METHOD0(ClearActiveEvents,
        void());
    MOCK_METHOD0(Clear,
        void());
    MOCK_METHOD1(Respawn,
        void(int));
    MOCK_METHOD1(AddCollision,
        void(const int));
    MOCK_METHOD1(Initialize,
        void(RunResultInterface*));
};
