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
#include <deque>

#include "Common/BehaviourData.h"
#include "Common/Helper.h"
#include "include/stochasticsInterface.h"

namespace CognitiveMap {
class ReactionTime {
  public:
    ReactionTime(DistributionEntry inPercTime, DistributionEntry percLatency, double cycleTime, StochasticsInterface* stochastics)
        : cycleTime{cycleTime} {
        double drawnInPercTime = inPercTime.mean <= 0 ? 0 : stochastics->GetLogNormalDistributed(inPercTime.mean, inPercTime.std_deviation);
        initialPerceptionTime = Common::ValueInBounds(inPercTime.min, drawnInPercTime, inPercTime.max);

        double drawnPercLatency =
            percLatency.mean <= 0 ? 0 : stochastics->GetLogNormalDistributed(percLatency.mean, percLatency.std_deviation);

        perceptionLatency = Common::ValueInBounds(percLatency.min, drawnPercLatency, percLatency.max);

        auto bufferTime = inPercTime.max > percLatency.max ? inPercTime.max : percLatency.max;
        bufferSize = static_cast<unsigned int>(std::round(bufferTime / (cycleTime / 1000))) + 1;
    }

    void Update(std::vector<std::shared_ptr<AgentPerception>> agents);
    void EraseAgent(int agentId);

    /*!
     * \brief model the processing latency time
     *
     *  return the  processed agents
     *
     */
    std::vector<std::shared_ptr<AgentPerception>> PerceivedAgents();

  private:
    // buffer the perceived agents to simulate processing time
    std::deque<std::vector<std::shared_ptr<AgentPerception>>> perceivedAgentBuffer;
    // buffer the perceived agents to simulate processing time
    std::unordered_map<int, double> processingAgentBuffer;
    unsigned int bufferSize;
    double cycleTime;
    double initialPerceptionTime;
    double perceptionLatency;
    StochasticsInterface* stochastics;
};
} // namespace CognitiveMap
