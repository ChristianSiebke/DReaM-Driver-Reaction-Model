
#include "transformAcquirePosition.h"

std::shared_ptr<AcquirePositionSignal>
openScenario::transformation::AcquirePosition::ConvertToSignal(const openpass::events::AcquirePositionEvent &event,
                                                               WorldInterface *world,
                                                               AgentInterface *agent,
                                                               int cycleTime)
{
    return std::make_shared<AcquirePositionSignal>(ComponentState::Acting, event.position);
}
