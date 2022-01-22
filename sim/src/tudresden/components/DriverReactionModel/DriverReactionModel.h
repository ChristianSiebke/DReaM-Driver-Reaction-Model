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
#include "Components/ActionDecision/ActionDecision.h"
#include "Components/CognitiveMap/CognitiveMap.h"
#include "Components/ComponentInterface.h"
#include "Components/GazeMovement/GazeMovement.h"
#include "Components/Navigation.h"
#include "Components/GazeMovement/RoadSegments/RoadSegmentInterface.h"
#include "Components/TrafficSignMemory/TrafficSignMemory.h"

class DriverReactionModel {
  public:
    DriverReactionModel() {}
    DriverReactionModel(const DriverReactionModel&) = delete;
    DriverReactionModel(DriverReactionModel&&) = delete;
    DriverReactionModel& operator=(const DriverReactionModel&) = delete;
    DriverReactionModel& operator=(DriverReactionModel&&) = delete;
    ~DriverReactionModel() = default;

    void UpdateInput(int time, std::shared_ptr<EgoPerception> egoAgent, std::vector<std::shared_ptr<AgentPerception>> ambientAgents,
                     std::shared_ptr<InfrastructurePerception> infrastructure,
                     std::vector<const MentalInfrastructure::TrafficSign*> trafficSigns);

    void UpdateComponents();

    void SetComponent(int priority, std::unique_ptr<Component::ComponentInterface> component);

    double GetAcceleration();

    const NavigationDecision GetRouteDecision();

    const GazeState GetGazeState();

    const std::vector<Common::Vector2d> GetSegmentControlFixationPoints();

    const WorldRepresentation& GetWorldRepresentation();

    const WorldInterpretation& GetWorldInterpretation();

  private:
    std::map<int, std::unique_ptr<Component::ComponentInterface>> components;
};
