/********************************************************************************
 * Copyright (c) 2018 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "include/roadInterface/junctionInterface.h"

class FakeJunction : public JunctionInterface
{
  public:
    MOCK_METHOD4(AddConnection, ConnectionInterface *(std::string, std::string, std::string, ContactPointType));
    MOCK_CONST_METHOD0(GetConnections, std::map<std::string, ConnectionInterface *>());
    MOCK_CONST_METHOD0(GetId, std::string());
    MOCK_METHOD1(AddPriority, void (Priority priority));
    MOCK_CONST_METHOD0(GetPriorities, const std::vector<Priority>& ());
};
