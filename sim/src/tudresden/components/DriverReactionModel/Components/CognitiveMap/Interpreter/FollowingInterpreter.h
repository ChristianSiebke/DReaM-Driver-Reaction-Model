/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#pragma once
#include "Common/WorldRepresentation.h"
#include "InterpreterInterface.h"

namespace Interpreter {

class FollowingInterpreter : public InterpreterInterface {
  public:
    FollowingInterpreter(LoggerInterface* logger, const BehaviourData& behaviourData) : InterpreterInterface(logger, behaviourData) {}
    FollowingInterpreter(const FollowingInterpreter&) = delete;
    FollowingInterpreter(FollowingInterpreter&&) = delete;
    FollowingInterpreter& operator=(const FollowingInterpreter&) = delete;
    FollowingInterpreter& operator=(FollowingInterpreter&&) = delete;
    virtual ~FollowingInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  protected:
      std::pair<std::optional<double>, bool> FollowingState(const AgentRepresentation &agent, const WorldRepresentation &representation,
                                                            const WorldInterpretation *interpretation);
      std::optional<ConflictSituation> MergeOrSplitManoeuvreDistanceToConflictArea(const EgoAgentRepresentation *ego,
                                                                                   const AgentRepresentation &observedAgent) const;
};
} // namespace Interpreter
