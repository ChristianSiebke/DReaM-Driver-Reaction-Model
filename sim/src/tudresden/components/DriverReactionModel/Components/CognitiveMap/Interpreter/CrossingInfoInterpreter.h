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
#include "Helper.h"
#include "InterpreterInterface.h"
#include "MentalInfrastructure/Road.h"
#include "PerceptionData.h"
#include "math.h"
#include "qglobal.h"
#include "vector2d.h"

namespace Interpreter {

class CrossingInfoInterpreter : public InterpreterInterface {
  public:
    CrossingInfoInterpreter(LoggerInterface* logger, const BehaviourData& behaviourData) : InterpreterInterface(logger, behaviourData) {}
    CrossingInfoInterpreter(const CrossingInfoInterpreter&) = delete;
    CrossingInfoInterpreter(CrossingInfoInterpreter&&) = delete;
    CrossingInfoInterpreter& operator=(const CrossingInfoInterpreter&) = delete;
    CrossingInfoInterpreter& operator=(CrossingInfoInterpreter&&) = delete;
    virtual ~CrossingInfoInterpreter() override = default;

    virtual void Update(WorldInterpretation* interpretation, const WorldRepresentation& representation) override;

  private:
    void Localize(const WorldRepresentation& representation);
    void UpdateStoppingPoints(const WorldRepresentation& representation);

    void SetDistanceSP(const AgentRepresentation* representation, const MentalInfrastructure::Road* road,
                       const MentalInfrastructure::Intersection* nextIntersection,
                       StoppingPointMap stoppingpoints);

    void DetermineCrossingType(const WorldRepresentation& representation);
    void DetermineCrossingPhase(const WorldRepresentation& representation);

    CrossingInfo crossingInfo;
};
} // namespace Interpreter
