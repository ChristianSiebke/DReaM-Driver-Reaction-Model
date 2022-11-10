/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/

#pragma once
#include "Common/BehaviourData.h"
#include "Common/Helper.h"
#include "Common/WorldRepresentation.h"
#include "Components/TrafficSignalMemory/TrafficSignalMemory.h"
#include "ReactionTime.h"

struct WorldRepresentation;

namespace CognitiveMap {
class Memory {
  public:
    Memory(int cycletime, const BehaviourData& behaviourData, StochasticsInterface* stochastics)
        : cycletime{static_cast<double>(cycletime)}, behaviourData{behaviourData},
          reactionTime(behaviourData.cmBehaviour.initialPerceptionTime, behaviourData.cmBehaviour.perceptionLatency, cycletime,
                       stochastics) {
        trafficSignMemory = std::make_unique<TrafficSignalMemory::TrafficSignalMemory>();
    }
    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;
    ~Memory() = default;

    /*!
     * \brief Update WorldRepresentation
     *
     */
    void UpdateWorldRepresentation(WorldRepresentation& worldRepresentation);

    void UpdateSensorInput(int time, std::shared_ptr<EgoPerception> egoAgent, std::vector<std::shared_ptr<AgentPerception>> agents,
                           std::shared_ptr<InfrastructurePerception> infrastruct,
                           std::vector<const MentalInfrastructure::TrafficSignal *> s) {
        timestamp = time;
        egoAgentPerception = egoAgent;
        infrastructurePerception = infrastruct;
        lastSeenTrafficSignals = s;
        reactionTime.Update(agents);
    }

  private:
    const EgoAgentRepresentation* UpdateEgoAgentRepresentation();
    const AmbientAgentRepresentations* UpdateAmbientAgentRepresentations();
    const InfrastructureRepresentation* UpdateInfrastructureRepresentation();
    const VisibleTrafficSignals *UpdateVisibleTrafficSignals();

    /*!
     * \brief extrapolate the internal data of an ambient agent
     */
    std::unique_ptr<AmbientAgentRepresentation> ExtrapolateAmbientAgent(const AmbientAgentRepresentation* agent);

    /*!
     * \brief human memory capacity
     *
     * If the agent capacity is exeeded the "oldest" agents are deleted.
     *
     */
    void CutAmbientAgentRepresentationsIfCapacityExceeded(AmbientAgentRepresentations& agentMemory);

    // perceived data
    std::shared_ptr<InfrastructurePerception> infrastructurePerception;
    std::shared_ptr<EgoPerception> egoAgentPerception;
    std::vector<const MentalInfrastructure::TrafficSignal *> lastSeenTrafficSignals;

    // current active data
    EgoAgentRepresentation egoAgent;
    InfrastructureRepresentation infrastructure;
    AmbientAgentRepresentations agentMemory;

    int timestamp;
    double cycletime; // Important, otherwise all calculations are rounded with int!
    const BehaviourData& behaviourData;
    ReactionTime reactionTime;

    // sub components
    std::unique_ptr<TrafficSignalMemory::TrafficSignalMemory> trafficSignMemory;
};
} // namespace CognitiveMap
