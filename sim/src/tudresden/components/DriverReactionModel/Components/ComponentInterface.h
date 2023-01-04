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
#include "Common/BehaviourData.h"
#include "LoggerInterface.h"
#include "include/stochasticsInterface.h"

struct WorldRepresentation;
struct WorldInterpretation;

namespace Component {
class ComponentInterface {
  public:
    ComponentInterface(int cycleTime, StochasticsInterface* stochastics, const LoggerInterface* loggerInterface,
                       const BehaviourData& behaviourData)
        : cycleTime{cycleTime}, stochastics{stochastics}, loggerInterface{loggerInterface}, behaviourData{behaviourData} {}
    ComponentInterface(const ComponentInterface&) = delete;
    ComponentInterface(ComponentInterface&&) = delete;
    ComponentInterface& operator=(const ComponentInterface&) = delete;
    ComponentInterface& operator=(ComponentInterface&&) = delete;
    virtual ~ComponentInterface() = default;

    virtual void Update() = 0;

    int GetCycleTime() const { return cycleTime; }

    StochasticsInterface* GetStochastic() const { return stochastics; }

    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }

    const BehaviourData& GetBehaviourData() const { return behaviourData; }

private:
    int cycleTime;
    StochasticsInterface* stochastics;
    const LoggerInterface* loggerInterface;
    const BehaviourData& behaviourData;
};
} // namespace Component
