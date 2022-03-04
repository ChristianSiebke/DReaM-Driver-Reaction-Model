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

struct Waypoint {
    std::string roadId;
    OwlId laneId;
    double s;

    std::string GetRoadId() const {
        return roadId;
    }
    OwlId GetLaneId() const {
        return laneId;
    }
};

struct Route {
    std::vector<Waypoint> GetWaypoints() const {
        return *waypoints;
    }
    std::shared_ptr<std::vector<Waypoint>> waypoints;
};

class Navigation : public Component::ComponentInterface {
public:
    Navigation(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation,
               const RouteElement &routeElement, int cycleTime, StochasticsInterface *stochastics, LoggerInterface *loggerInterface,
               const BehaviourData &behaviourData);
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
    bool TurningDecisionAtIntersectionHaveToBeSelected() const;
    CrossingType DetermineCrossingType(std::vector<int>) const;
    CrossingType DetermineCrossingType(std::list<const RoadmapGraph::RoadmapNode *> path) const;

    const MentalInfrastructure::Lane *GetTargetNode() const;

    IndicatorState ConvertCrossingTypeToIndicator(CrossingType decision) const;

    bool ResetDirectionChosen() const;

    bool ResetIndicator() const;

    IndicatorState Randomize() const;

    unsigned int intersectionCounter = 0;

    const WorldRepresentation &worldRepresentation;
    const WorldInterpretation &worldInterpretation;

    const RouteElement &routeElement;
    Route route;
    bool targetPassed = false;
    bool directionChosen = false;
    NavigationDecision routeDecision;
};
} // namespace Navigation
