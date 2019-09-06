/*********************************************************************
* Copyright (c) 2018 - 2019 in-tech
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#pragma once

#include "gmock/gmock.h"
#include "Interfaces/spawnControlInterface.h"

namespace SimulationSlave
{

class FakeSpawnControl : public SpawnControlInterface
{
public:
    MOCK_METHOD1(Execute, bool(int));
    MOCK_METHOD0(PullNewAgents, std::list<const Agent*>());
    MOCK_CONST_METHOD0(GetError, SpawnControlError());
};

}
