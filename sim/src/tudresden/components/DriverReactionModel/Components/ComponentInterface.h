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

    virtual const WorldRepresentation& GetWorldRepresentation() const = 0;

    virtual const WorldInterpretation& GetWorldInterpretation() const = 0;

    StochasticsInterface* GetStochastic() const { return stochastics; }

    void Log(const std::string &message, DReaMLogLevel level = info) const {
        loggerInterface->Log(message, level);
    }

    const BehaviourData& GetBehaviourData() const { return behaviourData; }

  private:
    //!  Component with highest priority is updated first
    int cycleTime;
    StochasticsInterface* stochastics;
    const LoggerInterface* loggerInterface;
    const BehaviourData& behaviourData;
};
} // namespace Component
