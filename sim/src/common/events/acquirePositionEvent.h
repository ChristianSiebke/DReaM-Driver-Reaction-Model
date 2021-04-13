#pragma once

#include <src/common/openScenarioDefinitions.h>
#include <utility>

#include "basicEvent.h"
namespace openpass::events {

class AcquirePositionEvent : public OpenScenarioEvent
{
public:
    AcquirePositionEvent(int time,
                         const std::string &eventName,
                         const std::string &source,
                         int agentId,
                         openScenario::Position position) :
        OpenScenarioEvent(time, eventName, source, {}, {{agentId}}), position(std::move(position))
    {
    }

    static constexpr char TOPIC[]{"OpenSCENARIO/Position"};
    const openScenario::Position position;
};

} // namespace openpass::events
