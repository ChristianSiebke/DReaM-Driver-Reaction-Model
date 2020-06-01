/*******************************************************************************
* Copyright (c) 2019, 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "EventDetectorCommonBase.h"

// template for overload pattern used in CheckCondition()
template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...)->overload<Ts...>;

struct ConditionMetaInformation
{
    WorldInterface *const world{};
    int currentTime{};
};
using ConditionVisitorVariant = std::variant<std::vector<const AgentInterface *>, bool>;

class ConditionalEventDetector : public EventDetectorCommonBase
{
public:
    ConditionalEventDetector(WorldInterface *world,
                             const openScenario::ConditionalEventDetectorInformation &eventDetectorInformation,
                             SimulationSlave::EventNetworkInterface *eventNetwork,
                             const CallbackInterface *callbacks,
                             StochasticsInterface *stochastics);

    ConditionalEventDetector() = delete;
    ConditionalEventDetector(const ConditionalEventDetector &) = delete;
    ConditionalEventDetector(ConditionalEventDetector &&) = delete;
    ConditionalEventDetector &operator=(const ConditionalEventDetector &) = delete;
    ConditionalEventDetector &operator=(ConditionalEventDetector &&) = delete;

    virtual void Trigger(int time) override;

private:
    const openScenario::ConditionalEventDetectorInformation eventDetectorInformation;
    int executionCounter{0};
    int maximumNumberOfExecutions{1};

    void Reset() override;

    /*!
     * ------------------------------------------------------------------------
     * \brief TriggerEventInsertion inserts an event into the EventNetwork
     *        at the specified time.
     *
     * \param[in] time The time at which to insert the event into the
     *            EventNetwork.
     * ------------------------------------------------------------------------
     */
    void TriggerEventInsertion(int time, const std::vector<const AgentInterface *> triggeringAgents);

    /*!
     * ------------------------------------------------------------------------
     * \brief IsBelowMaximumNumberOfExecutions determines if the EventDetector
     *        should still be allowed to execute.
     *
     * \return true if the number of times the EventDetector is less than the
     *         maximum number of executions specified for it; otherwise false
     * ------------------------------------------------------------------------
     */
    bool IsBelowMaximumNumberOfExecutions();

    /*!
     * ------------------------------------------------------------------------
     * \brief GetActors gets the actors to be targeted by the Events emitted by
     *        the EventDetector; this can either be those targeted as Actors in
     *        the openScenario file, or those specified as TriggeringEntities
     *        if TriggeringAgentsAsActors is set
     *
     * \param[in] triggeringAgents the collection of triggeringAgents as was
     *            specified in the TriggeringEntities section of the
     *            openScenario file
     *
     * \return the collection of AgentInterfaces for the Actors to be targeted
     *         by the Events emitted by the EventDetector
     * ------------------------------------------------------------------------
     */
    std::vector<const AgentInterface *> GetActors(const std::vector<const AgentInterface *> triggeringAgents = {});

    /*!
     * ------------------------------------------------------------------------
     * \brief EvaluateConditionsAtTime evaluates all conditions for the
     *        specified time and returns triggering agents who meet all conditions.
     *
     * \param[in] time the time at which the conditions are being evaluated
     *
     * \return <0> true if all conditions are met; false otherwise
     *         <1> triggeringAgents who meet all conditions
     * ------------------------------------------------------------------------
     */

    std::pair<bool, std::vector<const AgentInterface *>> EvaluateConditions(const int time);

    /*!
     * ------------------------------------------------------------------------
     * \brief CheckCondition utilizes the overload pattern to build a variant
     *        visitor on the fly to appropriately evaluate whether a Condition
     *        is met.
     *
     * \param[in] cmi meta-information related to evaluating a Condition
     *                variant
     *
     * \return lambda function for evaluating a Condition variant
     * ------------------------------------------------------------------------
     */
    auto CheckCondition(const ConditionMetaInformation &cmi)
    {
        return [cmi](auto&& condition)
        {
            return std::visit(overload{
                                  [&](const openScenario::ReachPositionCondition &reachPositionCondition) { return ConditionVisitorVariant{reachPositionCondition.IsMet(cmi.world)}; },
                                  [&](const openScenario::RelativeSpeedCondition &relativeSpeedCondition) { return ConditionVisitorVariant{relativeSpeedCondition.IsMet(cmi.world)}; },
                                  [&](const openScenario::TimeToCollisionCondition &timeToCollisionCondition) { return ConditionVisitorVariant{timeToCollisionCondition.IsMet(cmi.world)}; },
                                  [&](const openScenario::TimeHeadwayCondition& timeHeadwayCondition) { return ConditionVisitorVariant{timeHeadwayCondition.IsMet(cmi.world)}; },
                                  [&](const openScenario::SimulationTimeCondition &simulationTimeCondition) { return ConditionVisitorVariant{simulationTimeCondition.IsMet(cmi.currentTime)}; }},
                              condition);
        };
    }
};
