/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "ReactionTime.h"
namespace CognitiveMap {
void ReactionTime::Update(std::vector<std::shared_ptr<GeneralAgentPerception>> agents) {
    try {
        perceivedAgentBuffer.push_front(agents);
        if (perceivedAgentBuffer.size() > bufferSize) {
            perceivedAgentBuffer.pop_back();
        }
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update ReactionTime failed";
        throw std::logic_error(message);
    }
}

void ReactionTime::EraseAgent(int agentId) { processingAgentBuffer.erase(agentId); }

std::vector<std::shared_ptr<GeneralAgentPerception>> ReactionTime::PerceivedAgents() {
    std::for_each(perceivedAgentBuffer.front().begin(), perceivedAgentBuffer.front().end(),
                  [this](const std::shared_ptr<GeneralAgentPerception> agent) {
                      processingAgentBuffer.insert({agent->id, 0});
                  });
    std::vector<std::shared_ptr<GeneralAgentPerception>> result;
    auto latencyElementPosition = static_cast<unsigned int>(std::round(perceptionLatency / (cycleTime / 1000)));
    for (auto agent : processingAgentBuffer) {
        if (agent.second < initialPerceptionTime)
            continue;
        if (latencyElementPosition < perceivedAgentBuffer.size())
            std::copy_if(perceivedAgentBuffer.at(latencyElementPosition).begin(), perceivedAgentBuffer.at(latencyElementPosition).end(),
                         std::back_inserter(result),
                         [agent](std::shared_ptr<GeneralAgentPerception> element) { return element->id == agent.first; });
    }
    // update agents time since first perception
    for (auto& element : processingAgentBuffer) {
        element.second += cycleTime / 1000;
    }
    return result;
}

} // namespace CognitiveMap
