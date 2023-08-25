/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
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
      Memory(int cycletime, const BehaviourData &behaviourData, StochasticsInterface *stochastics) :
          cycletime{static_cast<double>(cycletime)},
          behaviourData{behaviourData},
          reactionTime(behaviourData.cmBehaviour.initialPerceptionTime, behaviourData.cmBehaviour.perceptionLatency, cycletime,
                       stochastics),
          trafficSignalMemory(std::make_unique<TrafficSignalMemory::TrafficSignalMemory>(
              behaviourData.cmBehaviour.trafficSig_memoryCapacity, behaviourData.cmBehaviour.trafficSig_memorytime)) {
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

    void UpdateSensorInput(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                           std::vector<std::shared_ptr<GeneralAgentPerception>> agents,
                           std::shared_ptr<InfrastructurePerception> infrastruct,
                           const std::vector<const MentalInfrastructure::TrafficSignal *> &s) {
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
    std::shared_ptr<InfrastructurePerception> infrastructurePerception{nullptr};
    std::shared_ptr<DetailedAgentPerception> egoAgentPerception{nullptr};
    std::vector<const MentalInfrastructure::TrafficSignal *> lastSeenTrafficSignals;

    // current active data
    EgoAgentRepresentation egoAgent;
    InfrastructureRepresentation infrastructure;
    AmbientAgentRepresentations agentMemory;
    std::vector<std::shared_ptr<GeneralAgentPerception>> processedAgents;

    int timestamp = 0;
    double cycletime; // Important, otherwise all calculations are rounded with int!
    const BehaviourData& behaviourData;
    ReactionTime reactionTime;

    // sub components
    std::unique_ptr<TrafficSignalMemory::TrafficSignalMemory> trafficSignalMemory{nullptr};
};
} // namespace CognitiveMap
