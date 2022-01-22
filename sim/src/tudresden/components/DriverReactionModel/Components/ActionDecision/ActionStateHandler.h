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
#ifndef ACTIONSTATE_H
#define ACTIONSTATE_H

#include "TU_Dresden/Common/Helper.h"

enum class ActionState { Start = 0, Collision = 1, Following = 2, EgoRoW = 3, IntersectionSituation = 4, End = 5 };

class ActionStateHandler {
  public:
    ActionStateHandler(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation)
        : worldRepresentation{worldRepresentation}, worldInterpretation{worldInterpretation} {}

    ActionState GetState(const std::unique_ptr<AgentInterpretation>& agent);

  private:
    bool DetermineNextState(const std::unique_ptr<AgentInterpretation>& agent);
    void IncrementState();

    ActionState currentState;
    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;
};

#endif // ACTIONSTATE_H
