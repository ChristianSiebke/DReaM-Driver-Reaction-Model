/********************************************************************************
 * Copyright (c) 2018-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "include/eventInterface.h"
#include "gmock/gmock.h"

class FakeEvent : public EventInterface
{
public:
    MOCK_CONST_METHOD0(GetEventTime, int());
    MOCK_CONST_METHOD0(GetCategory, EventDefinitions::EventCategory());
    MOCK_CONST_METHOD0(GetSource, std::string());
    MOCK_CONST_METHOD0(GetName, std::string());
    MOCK_CONST_METHOD0(GetTriggeringAgents, const openpass::type::TriggeringEntities());
    MOCK_CONST_METHOD0(GetActingAgents, const openpass::type::AffectedEntities());
    MOCK_CONST_METHOD0(GetParameter, const openpass::type::FlatParameter&());
};
