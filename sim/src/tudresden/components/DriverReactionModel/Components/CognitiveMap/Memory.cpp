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
#include "Memory.h"
#include <functional>

namespace CognitiveMap {
void Memory::UpdateWorldRepresentation(WorldRepresentation& worldRepresentation) {
    worldRepresentation.egoAgent = UpdateEgoAgentRepresentation();
    worldRepresentation.infrastructure = UpdateInfrastructureRepresentation();
    worldRepresentation.agentMemory = UpdateAmbientAgentRepresentations();
    worldRepresentation.trafficSignalMemory = UpdateVisibleTrafficSignals();
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
    auto perceivedAgents = reactionTime.PerceivedAgents();

    std::for_each(perceivedAgents.rbegin(), perceivedAgents.rend(), [=, &newMemoryAgents](std::shared_ptr<AgentPerception> agent) {
        // initial processing time
        newMemoryAgents.push_back(std::make_unique<AmbientAgentRepresentation>(agent));
    });
    auto anyOfCollectionHasSameID = [](const auto& collection, auto id) {
        return std::any_of(collection.begin(), collection.end(), [id](const auto& agent) { return agent->GetID() == id; });
    };
    auto agentIsOutdated = std::bind(anyOfCollectionHasSameID, std::ref(newMemoryAgents), std::placeholders::_1);
    auto agentOnInvalidLane = [](const auto& agent) { return agent->GetLane() == nullptr; };
    auto agentExceedLifeTime = [this](const auto& agent) { return (agent->GetLifeTime() > behaviourData.cmBehaviour.memorytime); };
    auto extrapolationFailed = [this](const auto& agent) {
        return !agent->FindNewPositionInDistance(agent->ExtrapolateDistanceAlongLane(cycletime / 1000));
    };
    auto agentIsInvalide = [=](auto& oldMemoryAgent) {
        return agentOnInvalidLane(oldMemoryAgent) || agentExceedLifeTime(oldMemoryAgent) || extrapolationFailed(oldMemoryAgent);
    };
    auto eraseAgent = [=](auto& oldMemoryAgent) {
        oldMemoryAgent->IncrementLifeTimeTicker(cycletime);
        return agentIsOutdated(oldMemoryAgent->GetID()) || agentIsInvalide(oldMemoryAgent);
    };
    // delete agents
    agentMemory.erase(std::remove_if(agentMemory.begin(), agentMemory.end(), eraseAgent), agentMemory.end());

    for (const auto& agent : agentMemory) {
        if (agentIsInvalide(agent)) {
            reactionTime.EraseAgent(agent->GetID());
        }
    }

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
    auto position = agent->FindNewPositionInDistance(agent->ExtrapolateDistanceAlongLane(cycletime / 1000));
    try {
        AgentPerception data = agent->GetInternalData();
        const auto newRoad = position->newLane->GetRoad();
        auto nextVelocity = data.velocity + data.acceleration * cycletime / 1000;
        if (data.velocity * nextVelocity <= 0.0 && data.velocity != 0.0) {
            // agent change moving direction -->stop
            data.acceleration = 0;
            data.velocity = 0;
        }
        // extrapolate internal_Data
        data.sCoordinate = position->newSCoordinate;
        double diffAngle = data.movingInLaneDirection ? 0 : M_PI;
        auto newPoint = position->newLane->InterpolatePoint(position->newSCoordinate);
        data.refPosition.x = newPoint.x;
        data.refPosition.y = newPoint.y;
        data.yawAngle = std::fmod(newPoint.hdg + diffAngle, (2 * M_PI));
        data.velocity += data.acceleration * cycletime / 1000;
        data.laneType = position->newLane->GetType();
        data.road = newRoad;
        data.lane = position->newLane;
        data.nextLane = InfrastructureRepresentation::NextLane(data.indicatorState, data.movingInLaneDirection, data.lane);
        auto junctionDistance = data.CalculateJunctionDistance(newRoad, position->newLane);
        data.distanceOnJunction = junctionDistance.distanceOnJunction;
        data.distanceToNextJunction = junctionDistance.distanceToNextJunction;

        return std::make_unique<AmbientAgentRepresentation>(std::make_shared<AgentPerception>(data));
    } catch (std::out_of_range error) {
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
