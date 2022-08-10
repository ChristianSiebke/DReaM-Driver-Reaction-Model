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
#include "Common/WorldRepresentation.h"
#include "InterpreterInterface.h"

namespace Interpreter {

class ConflictSituationInterpreter : public InterpreterInterface {
public:
    ConflictSituationInterpreter(LoggerInterface *logger, const BehaviourData &behaviourData) :
        InterpreterInterface(logger, behaviourData) {
    }
    ConflictSituationInterpreter(const ConflictSituationInterpreter &) = delete;
    ConflictSituationInterpreter(ConflictSituationInterpreter &&) = delete;
    ConflictSituationInterpreter &operator=(const ConflictSituationInterpreter &) = delete;
    ConflictSituationInterpreter &operator=(ConflictSituationInterpreter &&) = delete;
    virtual ~ConflictSituationInterpreter() override = default;

    virtual void Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) override;

private:
    std::optional<ConflictSituation> PossibleConflictSituationAlongLane(const EgoAgentRepresentation *ego,
                                                                        const AmbientAgentRepresentation &observedAgent) const;
    ConflictSituation CalculateConflictSituation(std::pair<const MentalInfrastructure::ConflictArea &, OwlId> egoCA,
                                                 std::pair<const MentalInfrastructure::ConflictArea &, OwlId> observedCA,
                                                 const EgoAgentRepresentation *ego, const AmbientAgentRepresentation &observedAgent) const;
    double DistanceToConflictPoint(const AgentRepresentation *agent, const MentalInfrastructure::LanePoint &junctionPoint,
                                   OwlId laneId) const;
};
} // namespace Interpreter