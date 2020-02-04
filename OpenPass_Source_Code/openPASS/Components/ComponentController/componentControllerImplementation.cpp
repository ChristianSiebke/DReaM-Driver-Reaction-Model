/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  ComponentControllerImplementation.cpp
//! @brief This file contains the implementation of the header file
//-----------------------------------------------------------------------------

#include "componentControllerImplementation.h"
#include "Common/agentCompToCompCtrlSignal.h"
#include "Common/primitiveSignals.h"
#include "Interfaces/eventNetworkInterface.h"
#include "Common/eventTypes.h"
#include "Common/componentStateChangeEvent.h"
#include "Common/componentWarningEvent.h"

ComponentControllerImplementation::ComponentControllerImplementation(std::string componentName,
                                                                     bool isInit,
                                                                     int priority,
                                                                     int offsetTime,
                                                                     int responseTime,
                                                                     int cycleTime,
                                                                     StochasticsInterface *stochastics,
                                                                     WorldInterface *world,
                                                                     const ParameterInterface *parameters,
                                                                     const std::map<int, ObservationInterface *> *observations,
                                                                     const CallbackInterface *callbacks,
                                                                     AgentInterface *agent,
                                                                     SimulationSlave::EventNetworkInterface* const eventNetwork) :
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
        observations,
        callbacks,
        agent,
        eventNetwork),
  stateManager(callbacks)
{
}

void ComponentControllerImplementation::UpdateInput(int localLinkId, const std::shared_ptr<SignalInterface const>& data, int time)
{
    Q_UNUSED(time);

    if (std::dynamic_pointer_cast<AgentCompToCompCtrlSignal const>(data) != nullptr)
    {
        const auto signal = SignalCast<AgentCompToCompCtrlSignal const>(data, localLinkId);

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
                const auto castedSignal = SignalCast<VehicleCompToCompCtrlSignal const>(signal, localLinkId);
                componentStateInformation = std::shared_ptr<ComponentStateInformation>(new AdasComponentStateInformation(castedSignal->GetComponentType(),
                                                                                                                        castedSignal->GetAgentComponentName(),
                                                                                                                        castedSignal->GetCurrentState(),
                                                                                                                        castedSignal->GetAdasType()));
            }
            else
            {
                componentStateInformation = std::shared_ptr<ComponentStateInformation>(new ComponentStateInformation(signal->GetComponentType(),
                                                                                       signal->GetAgentComponentName(),
                                                                                       signal->GetCurrentState()));
            }

            stateManager.AddComponent(localLinkId, componentStateInformation);
        }

        const auto warning = signal->GetComponentWarning();
        if (warning)
        {
            const auto warningComponent = stateManager.GetComponent(localLinkId);
            const auto componentWarningEvent = std::make_shared<ComponentWarningEvent>(time,
                                                                                       COMPONENTNAME,
                                                                                       "",
                                                                                       GetAgent()->GetId(),
                                                                                       warningComponent->GetComponentName(),
                                                                                       warning.value());
            GetEventNetwork()->InsertEvent(componentWarningEvent);

            driverWarnings.try_emplace(warningComponent->GetComponentName(), warning.value());
        }
    }
    else
    {
        const std::string msg = COMPONENTNAME + " invalid input signal";
        LOG(CbkLogLevel::Debug, msg);

        throw std::runtime_error(msg);
    }
}

void ComponentControllerImplementation::UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const>& data, int time)
{
    Q_UNUSED(time);

    ComponentState maxReachableState = ComponentState::Undefined;

    if (stateManager.LocalLinkIdIsRegistered(localLinkId))
    {
        const auto componentAtLocalLinkId = stateManager.GetComponent(localLinkId);
        maxReachableState = stateManager.GetComponent(localLinkId)->GetMaxReachableState();

        if (componentAtLocalLinkId->GetComponentType() == ComponentType::Driver)
        {
            data = std::make_shared<CompCtrlToDriverCompSignal const>(maxReachableState,
                                                                      stateManager.GetVehicleComponentNamesToTypeAndStateMap(),
                                                                      driverWarnings.size() > 0
                                                                        ? std::make_optional(driverWarnings)
                                                                        : std::nullopt);
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

/*
 * Each trigger, pull ComponentChangeEvents for this agent and pass the list of them
 * to the stateManager for proper handling of changes of component max reachable state
 */
void ComponentControllerImplementation::Trigger(int time)
{
    Q_UNUSED(time);

    // get the event list and filter by ComponentChangeEvents and this agent's id
    const auto stateChangeEventList = GetEventNetwork()->GetActiveEventCategory(EventDefinitions::EventCategory::ComponentStateChange);
    const auto agentId = GetAgent()->GetId();
    std::list<std::shared_ptr<ComponentChangeEvent const>> castedStateChangeEventListForAgentId;

    // filter state change event list by this agentid
    for (const auto &stateChangeEvent : stateChangeEventList)
    {
        const auto &componentChangeEvent = std::dynamic_pointer_cast<ComponentChangeEvent>(stateChangeEvent);
        const auto &actingAgentIds = componentChangeEvent->actingAgents;

        if (componentChangeEvent && std::find(actingAgentIds.begin(), actingAgentIds.end(), agentId) != actingAgentIds.end())
        {
           castedStateChangeEventListForAgentId.push_back(componentChangeEvent);
        }
    }

    // Instruct the stateManager to updateMaxReachableStates
    // - this prioritizes the provided event list for event-triggered max reachable states
    // - this also uses registered conditions to determine each component's max reachable
    //   state dependent on each other component's current state
    stateManager.UpdateMaxReachableStatesForRegisteredComponents(castedStateChangeEventListForAgentId);
}
