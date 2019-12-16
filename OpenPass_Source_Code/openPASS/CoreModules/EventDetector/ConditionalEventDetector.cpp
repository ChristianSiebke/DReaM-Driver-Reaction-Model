/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#include "ConditionalEventDetector.h"

ConditionalEventDetector::ConditionalEventDetector(WorldInterface *world,
                                                   const openScenario::ConditionalEventDetectorInformation& eventDetectorInformation,
                                                   SimulationSlave::EventNetworkInterface *eventNetwork,
                                                   const CallbackInterface *callbacks,
                                                   StochasticsInterface *stochastics) :
           EventDetectorCommonBase(world,
                                   eventNetwork,
                                   callbacks,
                                   stochastics),
           eventDetectorInformation(eventDetectorInformation)
{
    eventType = EventDefinitions::EventType::Conditional;
}

void ConditionalEventDetector::Reset()
{
    executionCounter = 0;
}

void ConditionalEventDetector::Trigger(int time)
{
    if (IsBelowMaximumNumberOfExecutions())
    {
        // this collection contains all of the agents who are defined as "TriggeringEntities" and who meet their conditions
        std::vector<const AgentInterface *> triggeringAgents {};
        bool allConditionsMet = EvaluateConditionsAtTime(time,
                                                         triggeringAgents);

        if (allConditionsMet)
        {
            TriggerEventInsertion(time, triggeringAgents);
        }
    }
}

void ConditionalEventDetector::TriggerEventInsertion(int time, const std::vector<const AgentInterface *> triggeringAgents)
{
    const auto actors = GetActors(triggeringAgents);

    std::vector<int> triggeringAgentIds{};
    for(const auto triggeringAgent : triggeringAgents)
    {
        triggeringAgentIds.push_back(triggeringAgent->GetId());
    }

    std::vector<int> actingAgentIds{};
    for(const auto actingAgent : actors)
    {
        actingAgentIds.push_back(actingAgent->GetId());
    }

    std::shared_ptr<AgentBasedManipulationEvent> event = std::make_shared<AgentBasedManipulationEvent>(time,
                                                                                                       COMPONENTNAME,
                                                                                                       eventDetectorInformation.sequenceName,
                                                                                                       eventType,
                                                                                                       triggeringAgentIds,
                                                                                                       actingAgentIds);

    eventNetwork->InsertEvent(event);

    executionCounter++;
}

bool ConditionalEventDetector::IsBelowMaximumNumberOfExecutions()
{
    return eventDetectorInformation.numberOfExecutions == -1 || executionCounter < eventDetectorInformation.numberOfExecutions;
}

std::vector<const AgentInterface *> ConditionalEventDetector::GetActors(const std::vector<const AgentInterface *> triggeringAgents)
{
    std::vector<const AgentInterface *> actors{};
    if (eventDetectorInformation.actorInformation.triggeringAgentsAsActors.value_or(false))
    {
        actors = triggeringAgents;
    }

    if(eventDetectorInformation.actorInformation.actors.has_value())
    {
        for (const auto &agentName : eventDetectorInformation.actorInformation.actors.value())
        {
            AgentInterface *agent = world->GetAgentByName(agentName);
            if(agent != nullptr && std::find(actors.begin(), actors.end(), agent) == actors.end())
            {
                actors.push_back(agent);
            }
        }
    }

    return actors;
}

bool ConditionalEventDetector::EvaluateConditionsAtTime(const int time,
                                                        std::vector<const AgentInterface*>& triggeringAgents)
{
    // this flag is used to ensure the first set of triggeringAgents found is correctly
    //  added to the triggeringAgents array (an intersection with an empty set is always empty)
    bool isFirstByEntityConditionEvaluated = false;

    // Create the lambda function for evaluating condition variants
    auto evaluateCondition = CheckCondition({world, time});
    // Evaluate if each condition is met. For some conditions, an array of Agents
    // is returned containing those entities described in the openScenario file
    // as "TriggeringEntities" that meet the prescribed condition.
    for (const auto& condition : eventDetectorInformation.conditions)
    {
        auto conditionEvaluation = evaluateCondition(condition);

        // if a vector of AgentInterfaces is returned, it contains those TriggeringEntities who meet the condition
        std::vector<const AgentInterface*>* triggeringEntitiesWhoMeetCondition = std::get_if<std::vector<const AgentInterface*>>(&conditionEvaluation);
        // get_if resolves to nullptr if the type specified is not found
        if (triggeringEntitiesWhoMeetCondition)
        {
            // if there are TriggeringEntities who meet the condition, add or intersect the set of
            //  entities for this condition into the triggeringAgents collection
            if (triggeringEntitiesWhoMeetCondition->size() > 0)
            {
                std::sort(triggeringEntitiesWhoMeetCondition->begin(),
                          triggeringEntitiesWhoMeetCondition->end(),
                          [](const AgentInterface* agentA, const AgentInterface* agentB) -> bool
                          {
                            return agentA->GetId() < agentB->GetId();
                          });
                if (!isFirstByEntityConditionEvaluated)
                {
                    triggeringAgents = *triggeringEntitiesWhoMeetCondition;
                    isFirstByEntityConditionEvaluated = true;
                }
                else
                {
                    std::vector<const AgentInterface*> intersection{};
                    std::set_intersection(triggeringAgents.begin(), triggeringAgents.end(),
                                          triggeringEntitiesWhoMeetCondition->begin(), triggeringEntitiesWhoMeetCondition->end(),
                                          std::back_inserter(intersection));
                    // if the intersection is empty, no TriggeringEntities have all conditions met.
                    if (intersection.empty())
                    {
                        // this condition was not met; no further condition checking needs to be done
                        return false;
                    }
                    triggeringAgents = intersection;
                }
            }
            // if no TriggeringEntities meet this condition, the condition was not met.
            else
            {
                // this condition was not met; no further condition checking needs to be done
                return false;
            }
        }
        else
        {
            bool* conditionIsMet = std::get_if<bool>(&conditionEvaluation);
            if (!(conditionIsMet == nullptr))
            {
                if (!(*conditionIsMet))
                {
                    // this condition was not met; no further condition checking needs to be done
                    return false;
                }
            }
            else
            {
                // we shouldn't get here - conditionEvaluation should have been handled by the above if statements
                LOGERROR("Unexpected condition evaluation return value");
                throw std::runtime_error("Unexpected condition evaluation return value");
            }
        }
    }

    return true;
}
