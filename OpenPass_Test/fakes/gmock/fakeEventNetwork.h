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

#include "Interfaces/eventNetworkInterface.h"
#include "gmock/gmock.h"

class FakeEventNetwork : public SimulationSlave::EventNetworkInterface
{
public:
    MOCK_METHOD1(GetEvents, EventContainer(const EventDefinitions::EventCategory));
    MOCK_METHOD1(InsertEvent, void(SharedEvent));
    MOCK_METHOD0(Clear, void());
    MOCK_METHOD1(AddCollision, void(const int));
    MOCK_METHOD1(Initialize, void(RunResultInterface *));
    MOCK_METHOD2(InsertTrigger, void (const std::string &, std::unique_ptr<EventInterface>));
    MOCK_CONST_METHOD1(GetTrigger, std::vector<EventInterface const *>(const std::string &identifier));
};
