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
#include "Common/MentalInfrastructure/RoadmapGraph/roadmap_graph.h"
#include "Common/WorldRepresentation.h"
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
    std::optional<const MentalInfrastructure::Lane *> TargetLane(const WorldRepresentation &representation);
   
    DReaMRoute::Waypoints waypoints;
    DReaMRoute::Waypoints::iterator targetWP;
    DReaMRoute::Waypoints::iterator lastTimeStepWP;
    const MentalInfrastructure::Lane *egoLane;
    std::list<const RoadmapGraph::RoadmapNode *> path;
};
} // namespace Interpreter
