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

#include "Interfaces/eventInterface.h"
#include "gmock/gmock.h"

class FakeEvent : public EventInterface
{
public:
    MOCK_CONST_METHOD0(GetEventTime, int());
    MOCK_CONST_METHOD0(GetCategory, EventDefinitions::EventCategory());
    MOCK_CONST_METHOD0(GetSource, std::string());
    MOCK_CONST_METHOD0(GetName, std::string());
    MOCK_CONST_METHOD0(GetTriggeringAgents, const TriggeringEntities());
    MOCK_CONST_METHOD0(GetActingAgents, const AffectedEntities());
    MOCK_CONST_METHOD0(GetParameter, const openpass::type::FlatParameter&());
};
