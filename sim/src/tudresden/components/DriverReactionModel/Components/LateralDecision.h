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
#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Common/MentalInfrastructure/RoadmapGraph/roadmap_graph.h"
#include "Common/PerceptionData.h"
#include "ComponentInterface.h"
#include "LoggerInterface.h"
#include "include/stochasticsInterface.h"

namespace LateralDecision {

class LateralDecision : public Component::ComponentInterface {
public:
    LateralDecision(const WorldRepresentation &worldRepresentation, const WorldInterpretation &worldInterpretation, int cycleTime,
                    StochasticsInterface *stochastics, LoggerInterface *loggerInterface, const BehaviourData &behaviourData);
    LateralDecision(const LateralDecision &) = delete;
    LateralDecision(LateralDecision &&) = delete;
    LateralDecision &operator=(const LateralDecision &) = delete;
    LateralDecision &operator=(LateralDecision &&) = delete;
    ~LateralDecision() override = default;

    virtual void Update() override;

    const LateralAction &GetLateralAction() const {
        return lateralAction;
    }

private:
    bool NewLaneIsFree() const;
    bool TurningAtJunction() const;
    LateralAction ResetLateralAction(IndicatorState currentIndicator) const;
    IndicatorState SetIndicatorAtJunction(const MentalInfrastructure::Lane *targetLane) const;

    bool AgentIsTurningOnJunction() const;

    const WorldRepresentation &worldRepresentation;
    const WorldInterpretation &worldInterpretation;

    LateralAction lateralAction;
};
} // namespace LateralDecision
