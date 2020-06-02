/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "runStatisticCalculation.h"
#include "Interfaces/worldInterface.h"

void RunStatisticCalculation::CalculateTotalDistanceTraveled(RunStatistic& runStatistic, WorldInterface* world)
{
    runStatistic.TotalDistanceTraveled = 0.0;
    runStatistic.EgoDistanceTraveled = 0.0;

    const auto egoAgent = world->GetEgoAgent();

    if (egoAgent)
    {
        runStatistic.EgoDistanceTraveled = egoAgent->GetDistanceTraveled();
    }

    for (const auto& agent : world->GetAgents())
    {
        runStatistic.TotalDistanceTraveled += agent.second->GetDistanceTraveled();
    }

    for (const auto& agent : world->GetRemovedAgents())
    {
        runStatistic.TotalDistanceTraveled += agent->GetDistanceTraveled();
    }
}

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
