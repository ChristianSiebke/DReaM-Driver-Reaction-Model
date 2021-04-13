
#pragma once

#include <src/common/acquirePositionSignal.h>
#include <src/common/events/acquirePositionEvent.h>

#include "transformerBase.h"

namespace openScenario::transformation {

struct AcquirePosition : public TransformerBase<AcquirePosition, AcquirePositionSignal, openpass::events::AcquirePositionEvent>
{
    static std::shared_ptr<AcquirePositionSignal> ConvertToSignal(const openpass::events::AcquirePositionEvent &event,
                                                                  WorldInterface *world,
                                                                  AgentInterface *agent,
                                                                  int cycleTime);
};

} // namespace openScenario::transformation
