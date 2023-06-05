/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef ACTIONSTATE_H
#define ACTIONSTATE_H

#include "Common/Helper.h"

enum class ActionState { Start = 0, CollisionImminent = 1, Following = 2, ReactToIntersectionSituation = 3, End = 4 };

class ActionStateHandler {
  public:
    ActionStateHandler(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation)
        : worldRepresentation{worldRepresentation}, worldInterpretation{worldInterpretation} {}

    ActionState GetState(const std::unique_ptr<AgentInterpretation>& agent);

  private:
    bool DetermineNextState(const std::unique_ptr<AgentInterpretation>& agent);
    void IncrementState();
    bool CloseToJunction(const std::unique_ptr<AgentInterpretation> &oAgent) const;
    bool EgoHasRightOfWay(const std::unique_ptr<AgentInterpretation> &agent) const;

    ActionState currentState;
    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;
};

#endif // ACTIONSTATE_H
