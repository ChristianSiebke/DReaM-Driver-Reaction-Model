/********************************************************************************
 * Copyright (c) 2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#include "TrafficLightNetwork.h"

TrafficLightNetwork::TrafficLightNetwork()
{

}

void TrafficLightNetwork::UpdateStates(int time)
{
    for (auto& controller : controllers)
    {
        controller.UpdateStates(time);
    }
}

void TrafficLightNetwork::AddController(TrafficLightController &&controller)
{
    controllers.push_back(std::move(controller));
}

void TrafficLightController::UpdateStates(int time)
{
    double deltaTime = time - lastTime;
    lastTime = time;
    timeRemainingInCurrentPhase -= deltaTime;
    while (timeRemainingInCurrentPhase <= 0)
    {
        currentPhase++;
        if (currentPhase == phases.cend())
        {
            currentPhase = phases.begin();
        }
        timeRemainingInCurrentPhase += currentPhase->duration;
        for (auto [trafficLight, newState] : currentPhase->states)
        {
            trafficLight->SetState(newState);
        }
    }
}

TrafficLightController::TrafficLightController(std::vector<TrafficLightController::Phase> &&phases, double delay) :
    phases(phases),
    currentPhase(this->phases.begin()),
    timeRemainingInCurrentPhase(currentPhase->duration + delay),
    lastTime(0)
{
    for (auto [trafficLight, newState] : currentPhase->states)
    {
        trafficLight->SetState(newState);
    }
    UpdateStates(0);
}
