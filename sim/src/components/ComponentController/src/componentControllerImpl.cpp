/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentControllerImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "componentControllerImpl.h"
#include "common/agentCompToCompCtrlSignal.h"
#include "common/primitiveSignals.h"
#include "include/eventNetworkInterface.h"
#include "common/events/componentStateChangeEvent.h"

ComponentControllerImplementation::ComponentControllerImplementation(std::string componentName,
                                                                     bool isInit,
                                                                     int priority,
                                                                     int offsetTime,
                                                                     int responseTime,
                                                                     int cycleTime,
                                                                     StochasticsInterface *stochastics,
                                                                     WorldInterface *world,
                                                                     const ParameterInterface *parameters,
                                                                     PublisherInterface * const publisher,
                                                                     const CallbackInterface *callbacks,
                                                                     AgentInterface *agent,
                                                                     core::EventNetworkInterface* const eventNetwork) :
    UnrestrictedEventModelInterface(
        componentName,
        isInit,
        priority,
        offsetTime,
        responseTime,
        cycleTime,
        stochastics,
        world,
        parameters,
        publisher,
        callbacks,
        agent,
        eventNetwork),
  stateManager(callbacks)
{
}

void ComponentControllerImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, [[maybe_unused]] int time)
{
    const auto signal = SignalCast<AgentCompToCompCtrlSignal>(data, localLinkId);

    if (stateManager.LocalLinkIdIsRegistered(localLinkId))
    {
        stateManager.UpdateComponentCurrentState(localLinkId, signal->GetCurrentState());
    }
    else
    {
        std::shared_ptr<ComponentStateInformation> componentStateInformation;

        ComponentType componentType = signal->GetComponentType();
        if(componentType == ComponentType::VehicleComponent)
        {
            const auto castedSignal = SignalCast<VehicleCompToCompCtrlSignal>(signal, localLinkId);
            componentStateInformation = std::make_shared<AdasComponentStateInformation>(castedSignal->GetComponentType(),
                                                                                        castedSignal->GetAgentComponentName(),
                                                                                        castedSignal->GetCurrentState(),
                                                                                        castedSignal->GetAdasType());
        }
        else
        {
            componentStateInformation = std::make_shared<ComponentStateInformation>(signal->GetComponentType(),
                                                                                    signal->GetAgentComponentName(),
                                                                                    signal->GetCurrentState());
        }

        stateManager.AddComponent(localLinkId, componentStateInformation);
    }

    const auto warnings = signal->GetComponentWarnings();
    if (!warnings.empty())
    {
        const auto warningComponent = stateManager.GetComponent(localLinkId);

        driverWarnings.try_emplace(warningComponent->GetComponentName(), warnings);
    }
}

void ComponentControllerImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, [[maybe_unused]] int time)
{
    ComponentState maxReachableState = ComponentState::Undefined;

    if (stateManager.LocalLinkIdIsRegistered(localLinkId))
    {
        const auto componentAtLocalLinkId = stateManager.GetComponent(localLinkId);
        maxReachableState = stateManager.GetComponent(localLinkId)->GetMaxReachableState();

        if (componentAtLocalLinkId->GetComponentType() == ComponentType::Driver)
        {
            data = std::make_shared<CompCtrlToDriverCompSignal const>(maxReachableState,
                                                                      stateManager.GetVehicleComponentNamesToTypeAndStateMap(),
                                                                      driverWarnings);
            // clear all warnings to forward to avoid unwanted repeats
            driverWarnings.clear();
        }
        else
        {
            data = std::make_shared<CompCtrlToAgentCompSignal const>(maxReachableState, stateManager.GetVehicleComponentNamesToTypeAndStateMap());
        }
    }
    else
    {
        data = std::make_shared<CompCtrlToAgentCompSignal const>(maxReachableState, stateManager.GetVehicleComponentNamesToTypeAndStateMap());
    }
}

bool ComponentControllerImplementation::IsAgentAffectedByEvent(EventInterface const * event)
{
    return std::find(event->GetActingAgents().entities.begin(),event->GetActingAgents().entities.end(),
                     GetAgent()->GetId()) != event->GetActingAgents().entities.end();
}

/*
 * Each trigger, pull ComponentStateChangeEvents for this agent and pass the list of them
 * to the stateManager for proper handling of changes of component max reachable state
 */
void ComponentControllerImplementation::Trigger([[maybe_unused]] int time)
{
    // get the event list and filter by ComponentStateChangeEvents and this agent's id
    const auto stateChangeEventList = GetEventNetwork()->GetTrigger(openpass::events::ComponentStateChangeEvent::TOPIC);

    std::vector<openpass::events::ComponentStateChangeEvent const *> componentStateChanges;

    // filter state change event list by this agentid
    for (const auto &stateChangeEvent : stateChangeEventList)
    {
        const auto componentStateChangeEvent = dynamic_cast<openpass::events::ComponentStateChangeEvent const *>(stateChangeEvent);
        if (componentStateChangeEvent && IsAgentAffectedByEvent(componentStateChangeEvent))
        {
           componentStateChanges.push_back(componentStateChangeEvent);
        }
    }

    // Instruct the stateManager to updateMaxReachableStates
    // - this prioritizes the provided event list for event-triggered max reachable states
    // - this also uses registered conditions to determine each component's max reachable
    //   state dependent on each other component's current state
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(componentStateChanges);
}
