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

#include "DriverReactionModel.h"

void DriverReactionModel::UpdateComponents() {
    for (std::map<int, std::unique_ptr<Component::ComponentInterface>>::reverse_iterator iter = components.rbegin();
         iter != components.rend(); ++iter) {
        iter->second->Update();
    }
}

void DriverReactionModel::UpdateInput(int time, std::shared_ptr<EgoPerception> egoAgent,
                                      std::vector<std::shared_ptr<AgentPerception>> ambientAgents,
                                      std::shared_ptr<InfrastructurePerception> infrastructure,
                                      std::vector<const MentalInfrastructure::TrafficSign*> trafficSigns) {
    std::for_each(components.begin(), components.end(), [egoAgent, ambientAgents, infrastructure, trafficSigns, time](auto& component) {
        if (CognitiveMap::CognitiveMap* cognitiveMap = dynamic_cast<CognitiveMap::CognitiveMap*>(component.second.get())) {
            cognitiveMap->UpdateInput(time, egoAgent, ambientAgents, infrastructure, trafficSigns);
        };
    });
}

void DriverReactionModel::SetComponent(int priority, std::unique_ptr<Component::ComponentInterface> component) {
    components.insert(std::make_pair(priority, std::move(component)));
}

double DriverReactionModel::GetAcceleration() {
    for (auto& element : components) {
        if (ActionDecision::ActionDecision* actionDecision = dynamic_cast<ActionDecision::ActionDecision*>(element.second.get())) {
            return actionDecision->GetAcceleration();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no ActionDecision Component";
    throw std::runtime_error(message);
}

const NavigationDecision DriverReactionModel::GetRouteDecision() {
    for (auto& element : components) {
        if (Navigation::Navigation* navigation = dynamic_cast<Navigation::Navigation*>(element.second.get())) {
            return navigation->GetRouteDecision();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no Navigation Component";
    throw std::runtime_error(message);
}

const GazeState DriverReactionModel::GetGazeState() {
    for (auto& element : components) {
        if (GazeMovement::GazeMovement* gazeMovement = dynamic_cast<GazeMovement::GazeMovement*>(element.second.get())) {
            return gazeMovement->GetGazeState();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no GazeMovement Component";
    throw std::runtime_error(message);
}

const std::vector<Common::Vector2d> DriverReactionModel::GetSegmentControlFixationPoints() {
    for (auto& element : components) {
        if (GazeMovement::GazeMovement* gazeMovement = dynamic_cast<GazeMovement::GazeMovement*>(element.second.get())) {
            return gazeMovement->GetSegmentControlFixationPoints();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no GazeMovement Component";
    throw std::runtime_error(message);
}

const WorldRepresentation& DriverReactionModel::GetWorldRepresentation() {
    for (auto& element : components) {
        if (CognitiveMap::CognitiveMap* cognitiveMap = dynamic_cast<CognitiveMap::CognitiveMap*>(element.second.get())) {
            return cognitiveMap->GetWorldRepresentation();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no CognitiveMap Component";
    throw std::runtime_error(message);
}
const WorldInterpretation& DriverReactionModel::GetWorldInterpretation() {
    for (auto& element : components) {
        if (CognitiveMap::CognitiveMap* cognitiveMap = dynamic_cast<CognitiveMap::CognitiveMap*>(element.second.get())) {
            return cognitiveMap->GetWorldInterpretation();
        }
    }
    std::string message = __FILE__ " Line: " + std::to_string(__LINE__) + "There is no CognitiveMap Component";
    throw std::runtime_error(message);
}
