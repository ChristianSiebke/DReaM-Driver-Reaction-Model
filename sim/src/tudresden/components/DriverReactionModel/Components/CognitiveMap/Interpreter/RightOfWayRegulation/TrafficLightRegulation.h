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
#include "RightOfWayRegulationInterface.h"

namespace RightOfWayRegulation {

class TrafficLightRegulationGermany : public RightOfWayRegulationInterface {
  public:
    TrafficLightRegulationGermany() {}
    virtual ~TrafficLightRegulationGermany() override = default;

  private:
};
} // namespace RightOfWayRegulation
