/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*               2020 BMW AG
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <map>

#include "Interfaces/modelInterface.h"
#include "actionTransformRepository.h"
#include "transformLaneChange.h"
#include "transformSpeedAction.h"
#include "transformTrajectory.h"
/**
* \brief Relays triggered OpenScenario actions as signals to other components
*
* When used in conjunction with autoregistering transformers (\sa actionTransformRepository),
* the linkIdMapping has to be provided via the parameter interface.
*
* That means that when a transformer for e.g. event X is autoregistered,
* a parameter for mapping X::Topic to a linkId needs to be defined.
*
* Syntax:
* <parameter>
*     <id>THE_TOPIC</id>
*     <type>int</type>
*     <unit/>
*     <value>THE_LINK_ID</value>
* </parameter>
*
* \ingroup OpenScenarioActions
*/
class OpenScenarioActionsImplementation : public UnrestrictedEventModelInterface
{
public:
    static constexpr char COMPONENTNAME[] {"OpenScenarioActions"};

    OpenScenarioActionsImplementation(std::string componentName,
                                      bool isInit,
                                      int priority,
                                      int offsetTime,
                                      int responseTime,
                                      int cycleTime,
                                      StochasticsInterface *stochastics,
                                      WorldInterface *world,
                                      const ParameterInterface *parameters,
                                      PublisherInterface *const publisher,
                                      const CallbackInterface *callbacks,
                                      AgentInterface *agent,
                                      SimulationSlave::EventNetworkInterface *const eventNetwork);

    void UpdateInput(int, const std::shared_ptr<SignalInterface const> &, int) override;
    void UpdateOutput(int localLinkId, std::shared_ptr<SignalInterface const> &data, int time) override;
    void Trigger(int time) override;

private:
    [[noreturn]] void ThrowUnregisteredIdentifier(const std::string identifier);
    [[noreturn]] void ThrowOnTooManySignals(LinkId localLinkId);
    [[noreturn]] void ThrowOnInvalidLinkId(LinkId localLinkId);

    TransformResults pendingSignals;

    inline static std::vector<bool> registeredActions{
           ActionTransformRepository::Register(openScenario::transformation::Trajectory::Transform),
           ActionTransformRepository::Register(openScenario::transformation::LaneChange::Transform),
           ActionTransformRepository::Register(openScenario::transformation::SpeedAction::Transform),
       };

    std::map<const std::string, LinkId> linkIdMapping{
        {openpass::events::TrajectoryEvent::TOPIC, 0},
        {openpass::events::LaneChangeEvent::TOPIC, 0},
        {openpass::events::SpeedActionEvent::TOPIC, 3},
    };
};
