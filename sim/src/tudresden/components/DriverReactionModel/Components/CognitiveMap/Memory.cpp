/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Memory.h"
#include <functional>

namespace CognitiveMap {
void Memory::UpdateWorldRepresentation(WorldRepresentation& worldRepresentation) {
    try {
        worldRepresentation.egoAgent = UpdateEgoAgentRepresentation();
        worldRepresentation.infrastructure = UpdateInfrastructureRepresentation();
        worldRepresentation.agentMemory = UpdateAmbientAgentRepresentations();
        worldRepresentation.trafficSignalMemory = UpdateVisibleTrafficSignals();
        worldRepresentation.processedAgents = processedAgents;
    }
    catch (...) {
        std::string message =
            "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " Update Memory failed";
        throw std::logic_error(message);
    }
}

const VisibleTrafficSignals *Memory::UpdateVisibleTrafficSignals() {
    return trafficSignalMemory->Update(timestamp, lastSeenTrafficSignals, egoAgentPerception.get());
}

const EgoAgentRepresentation* Memory::UpdateEgoAgentRepresentation() {
    egoAgent.UpdateInternalData(egoAgentPerception);
    return &egoAgent;
};

const InfrastructureRepresentation* Memory::UpdateInfrastructureRepresentation() {
    infrastructure.UpdateInternalData(infrastructurePerception);
    return &infrastructure;
};

const AmbientAgentRepresentations* Memory::UpdateAmbientAgentRepresentations() {
    AmbientAgentRepresentations newMemoryAgents;
    processedAgents = reactionTime.PerceivedAgents();
    std::for_each(processedAgents.rbegin(), processedAgents.rend(), [=, &newMemoryAgents](std::shared_ptr<GeneralAgentPerception> agent) {
        // initial processing time
        newMemoryAgents.push_back(std::make_unique<AmbientAgentRepresentation>(agent));
    });
    auto anyOfCollectionHasSameID = [](const auto &collection, auto id) {
        return std::any_of(collection.begin(), collection.end(), [id](const auto &agent) { return agent->GetID() == id; });
    };
    auto agentIsOutdated = std::bind(anyOfCollectionHasSameID, std::ref(newMemoryAgents), std::placeholders::_1);
    auto agentOnInvalidLane = [](const auto &agent) { return agent->GetLanePosition().lane == nullptr; };
    auto agentExceedLifeTime = [this](const auto &agent) { return (agent->GetLifeTime() > behaviourData.cmBehaviour.memorytime); };
    auto extrapolationFailed = [this](const auto &agent) {
        return !agent->FindNewPositionInDistance(agent->ExtrapolateDistanceAlongLane(cycletime / 1000));
    };
    auto agentIsInvalide = [=](auto &oldMemoryAgent) {
        return agentOnInvalidLane(oldMemoryAgent) || agentExceedLifeTime(oldMemoryAgent) || extrapolationFailed(oldMemoryAgent);
    };
    auto eraseAgent = [=](auto &oldMemoryAgent) {
        oldMemoryAgent->IncrementLifeTimeTicker(cycletime);
        return agentIsOutdated(oldMemoryAgent->GetID()) || agentIsInvalide(oldMemoryAgent);
    };

    for (const auto &agent : agentMemory) {
        if (agentIsInvalide(agent)) {
            reactionTime.EraseAgent(agent->GetID());
        }
    }

    // delete agents
    agentMemory.erase(std::remove_if(agentMemory.begin(), agentMemory.end(), eraseAgent), agentMemory.end());

    // extrapolate agents when no new visual information is perceived
    std::for_each(agentMemory.begin(), agentMemory.end(), [this](std::unique_ptr<AmbientAgentRepresentation>& memoryAgent) {
        memoryAgent = ExtrapolateAmbientAgent(memoryAgent.get());
    });

    // add new visual perceived agent
    std::move(newMemoryAgents.begin(), newMemoryAgents.end(), std::back_inserter(agentMemory));
    CutAmbientAgentRepresentationsIfCapacityExceeded(agentMemory);
    return &agentMemory;
}

std::unique_ptr<AmbientAgentRepresentation> Memory::ExtrapolateAmbientAgent(const AmbientAgentRepresentation* agent) {
    auto newPosition = agent->FindNewPositionInDistance(agent->ExtrapolateDistanceAlongLane(cycletime / 1000));
    if (!newPosition.has_value()) {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + " Extrapolation failed ";
        throw std::logic_error(msg);
    }
    try {
        GeneralAgentPerception data = agent->GetInternalData();
        const auto newRoad = newPosition->lane->GetRoad();
        auto nextVelocity = data.velocity + data.acceleration * cycletime / 1000;
        if (data.velocity * nextVelocity <= 0.0 && data.velocity != 0.0) {
            // agent change moving direction -->stop
            data.acceleration = 0;
            data.velocity = 0;
        }
        // extrapolate internal_Data
        // TODO check if agentperception in agentrepresentation is a copy or the same object
        data.lanePosition.sCoordinate = newPosition->sCoordinate;
        double diffAngle = data.movingInLaneDirection ? 0 : M_PI;
        auto newPoint = newPosition->lane->InterpolatePoint(newPosition->sCoordinate);
        data.refPosition.x = newPoint.x;
        data.refPosition.y = newPoint.y;
        data.yaw = std::fmod(newPoint.hdg + diffAngle, (2 * M_PI));
        data.velocity += data.acceleration * cycletime / 1000;
        data.lanePosition.lane = newPosition->lane;
        data.nextLane = data.lanePosition.lane->NextLane(data.indicatorState, data.movingInLaneDirection);
        data.junctionDistance = GeneralAgentPerception::CalculateJunctionDistance(data, newRoad, newPosition->lane);

        return std::make_unique<AmbientAgentRepresentation>(std::make_shared<GeneralAgentPerception>(data), agent->GetLifeTime());
    }
    catch (std::out_of_range &error) {
        auto msg = __FILE__ " Line: " + std::to_string(__LINE__) + error.what() + " Extrapolation failed ";
        throw std::logic_error(msg);
    }
}

void Memory::CutAmbientAgentRepresentationsIfCapacityExceeded(AmbientAgentRepresentations& agentMemory) {
    // memory capacity is exceeded? ( "forget process")
    if (agentMemory.size() > behaviourData.cmBehaviour.memoryCapacity) {
        size_t number_of_deletions = agentMemory.size() - behaviourData.cmBehaviour.memoryCapacity;

        auto range_end = agentMemory.begin();
        std::advance(range_end, number_of_deletions);

        agentMemory.erase(agentMemory.begin(), range_end);
    }
}
} // namespace CognitiveMap
