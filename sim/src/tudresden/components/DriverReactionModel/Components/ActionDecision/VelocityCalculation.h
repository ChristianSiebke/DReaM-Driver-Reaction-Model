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
#ifndef VELOCITYCALCULATION_H
#define VELOCITYCALCULATION_H

#include "Common/Definitions.h"
#include "Common/Helper.h"
#include "Components/ComponentInterface.h"
#include "include/stochasticsInterface.h"

namespace ActionDecision {

class VelocityCalculation : public Component::ComponentInterface {
  public:
    VelocityCalculation(const WorldRepresentation& worldRepresentation, const WorldInterpretation& worldInterpretation, int cycleTime,
                        StochasticsInterface* stochastics, LoggerInterface* loggerInterface, const BehaviourData& behaviourData)
        : ComponentInterface(cycleTime, stochastics, loggerInterface, behaviourData), worldRepresentation{worldRepresentation},
          worldInterpretation{worldInterpretation} {}

    VelocityCalculation(const VelocityCalculation&) = delete;
    VelocityCalculation(VelocityCalculation&&) = delete;
    VelocityCalculation& operator=(const VelocityCalculation&) = delete;
    VelocityCalculation& operator=(VelocityCalculation&&) = delete;
    ~VelocityCalculation() override = default;

    void Update() override;

    virtual const WorldRepresentation& GetWorldRepresentation() const override { return worldRepresentation; }

    virtual const WorldInterpretation& GetWorldInterpretation() const override { return worldInterpretation; }

    double PrepareTargetVelocity();

  private:
    void CalculatePhaseVelocities();

    const WorldRepresentation& worldRepresentation;
    const WorldInterpretation& worldInterpretation;

    std::map<CrossingPhase, double> phaseVelocity;
};

} // namespace ActionDecision

#endif // VELOCITYCALCULATION_H
