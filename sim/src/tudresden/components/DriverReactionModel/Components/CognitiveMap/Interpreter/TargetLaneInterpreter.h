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
#include "InterpreterInterface.h"

namespace Interpreter {

class TargetLaneInterpreter : public InterpreterInterface {
public:
    TargetLaneInterpreter(LoggerInterface *logger, const BehaviourData &behaviourData) : InterpreterInterface(logger, behaviourData) {
    }
    TargetLaneInterpreter(const TargetLaneInterpreter &) = delete;
    TargetLaneInterpreter(TargetLaneInterpreter &&) = delete;
    TargetLaneInterpreter &operator=(const TargetLaneInterpreter &) = delete;
    TargetLaneInterpreter &operator=(TargetLaneInterpreter &&) = delete;
    virtual ~TargetLaneInterpreter() override = default;

    virtual void Update(WorldInterpretation *interpretation, const WorldRepresentation &representation) override;

private:
    DReaMRoute::Waypoints waypoints;
    DReaMRoute::Waypoints::iterator targetWP;
    DReaMRoute::Waypoints::iterator lastTimeStepWP;
    std::list<const RoadmapGraph::RoadmapNode *> path;
    DReaMRoute::Waypoint oldWP;
};
} // namespace Interpreter
