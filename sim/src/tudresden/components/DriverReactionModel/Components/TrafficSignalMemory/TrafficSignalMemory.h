#ifndef TRAFFICSIGNMEMORY_H
#define TRAFFICSIGNMEMORY_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "Common/MentalInfrastructure/TrafficLight.h"
#include "Common/MentalInfrastructure/TrafficSign.h"
#include "Common/MentalInfrastructure/TrafficSignal.h"
#include "Common/WorldRepresentation.h"
#include "Components/ComponentInterface.h"

namespace TrafficSignalMemory {

class TrafficSignalMemory {
public:
    TrafficSignalMemory(unsigned int maxElements, int maxTime) :
        maximumElementsInMemory{maxElements},
        maximumTimeInMemoryMs{maxTime},
        visibleTrafficSignals(std::make_unique<VisibleTrafficSignals>()) {
    }

    VisibleTrafficSignals *Update(int timestamp, std::vector<const MentalInfrastructure::TrafficSignal *> input,
                                  DetailedAgentPerception *ego);

private:
    void EraseFromVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *sign);
    void InsertIntoVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *sign);

private:
    std::unordered_map<DReaMId, MemorizedTrafficSignal> memory{};
    std::unique_ptr<VisibleTrafficSignals> visibleTrafficSignals{nullptr};

    const unsigned int maximumElementsInMemory;
    const int maximumTimeInMemoryMs;
};

} // namespace TrafficSignalMemory

#endif // TRAFFICSIGNMEMORY_H
