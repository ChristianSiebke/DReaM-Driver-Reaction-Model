/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*               2020 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "runStatisticCalculation.h"
#include "include/worldInterface.h"

void RunStatisticCalculation::DetermineEgoCollision(RunStatistic& runStatistic, const RunResultInterface& runResult, WorldInterface* world)
{
    auto egoAgent = world->GetEgoAgent();

    if (!egoAgent)
    {
        return;
    }

    const int egoId = egoAgent->GetId();
    auto collisionsIds = runResult.GetCollisionIds();

    if (std::find(collisionsIds->cbegin(), collisionsIds->cend(), egoId) != collisionsIds->cend())
    {
        runStatistic.EgoCollision = true;
    }
}
