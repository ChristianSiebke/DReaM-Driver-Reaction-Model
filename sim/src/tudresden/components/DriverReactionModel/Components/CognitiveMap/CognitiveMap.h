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
#include <list>
#include <memory>

#include "Common/BehaviourData.h"
#include "Common/ComplexSignals.h"
#include "Common/Helper.h"
#include "Common/WorldRepresentation.h"
#include "Components/ComponentInterface.h"
#include "Interpreter/CollisionInterpreter.h"
#include "Interpreter/ConflictSituationInterpreter.h"
#include "Interpreter/CrossingInfoInterpreter.h"
#include "Interpreter/FollowingInterpreter.h"
#include "Interpreter/RightOfWayInterpreter.h"
#include "Interpreter/TargetLaneInterpreter.h"
#include "Interpreter/WorldInterpreter.h"
#include "Memory.h"

namespace CognitiveMap {
class CognitiveMap : public Component::ComponentInterface {
  public:
    CognitiveMap(int cycleTime, StochasticsInterface* stochastics, LoggerInterface* loggerInterface, const BehaviourData& behaviourData);
    CognitiveMap(const CognitiveMap&) = delete;
    CognitiveMap(CognitiveMap&&) = delete;
    CognitiveMap& operator=(const CognitiveMap&) = delete;
    CognitiveMap& operator=(CognitiveMap&&) = delete;
    ~CognitiveMap() override = default;

    const WorldRepresentation& GetWorldRepresentation() const { return worldRepresentation; }

    const WorldInterpretation& GetWorldInterpretation() const { return worldInterpretation; }

    virtual void Update() override;
    virtual void UpdateInput(int time, std::shared_ptr<DetailedAgentPerception> egoAgent,
                             std::vector<std::shared_ptr<GeneralAgentPerception>> ambientAgents,
                             std::shared_ptr<InfrastructurePerception> infrastructure,
                             std::vector<const MentalInfrastructure::TrafficSignal *> trafficSignals);

private:
    void UpdateWorldRepresentation();
    void ResetWorldInterpretation();

    Interpreter::WorldInterpreter worldInterpreter;

    WorldRepresentation worldRepresentation;
    WorldInterpretation worldInterpretation;
    Memory memory;
};
} // namespace CognitiveMap
