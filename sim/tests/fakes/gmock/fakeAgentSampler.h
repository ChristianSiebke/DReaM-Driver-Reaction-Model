#pragma once

#include "gmock/gmock.h"
#include "Interfaces/agentSamplerInterface.h"

class FakeAgentSampler : public AgentSamplerInterface
{
 public:
  MOCK_METHOD3(SampleAgent,
      bool(AgentBlueprintInterface*,
           LaneCategory,
           unsigned int));
};


