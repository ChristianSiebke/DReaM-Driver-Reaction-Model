/******************************************************************************
 * Copyright (c) 2019 TU Dresden
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
#include "ComponentInterface.h"
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "LoggerInterface.h"
#include "Common/PerceptionData.h"
#include "Common/MentalInfrastructure/RoadmapGraph/roadmap_graph.h"
#include "include/stochasticsInterface.h"

namespace Navigation {

class Navigation : public Component::ComponentInterface {
public:
    Navigation(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation, int cycleTime,
               StochasticsInterface *stochastics, LoggerInterface *loggerInterface, const BehaviourData &behaviourData);
    Navigation(const Navigation &) = delete;
    Navigation(Navigation &&) = delete;
    Navigation &operator=(const Navigation &) = delete;
    Navigation &operator=(Navigation &&) = delete;
    ~Navigation() override = default;
    /*!
     * \brief DetermineNavigationDecision select next lane and indicator state
     */
    virtual void Update() override;

    virtual const WorldRepresentation &GetWorldRepresentation() const override {
        return worldRepresentation;
    }

    virtual const WorldInterpretation &GetWorldInterpretation() const override {
        return worldInterpretation;
    }

    const NavigationDecision &GetRouteDecision() const {
        return routeDecision;
    }

private:
    bool NewLaneIsFree() const;
    bool TurningAtJunction() const;
    IndicatorState SetIndicatorAtJunction(std::list<const RoadmapGraph::RoadmapNode *> path) const;

    bool AgentIsTurningOnJunction() const;

    const WorldRepresentation &worldRepresentation;
    const WorldInterpretation &worldInterpretation;

    NavigationDecision routeDecision;
    DReaMRoute::Waypoints waypoints;
    DReaMRoute::Waypoints::iterator targetWP;
    DReaMRoute::Waypoints::iterator lastTimeStepWP;
    const MentalInfrastructure::Lane *egoLane;
    std::list<const RoadmapGraph::RoadmapNode *> path;
    DReaMRoute::Waypoint oldWP;
};
} // namespace Navigation
