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
    TrafficSignalMemory() {
        visibleTrafficSignals = std::make_unique<VisibleTrafficSignals>();
    }

    VisibleTrafficSignals *Update(int timestamp, std::vector<const MentalInfrastructure::TrafficSignal *> input, EgoPerception *ego);

private:
    void EraseFromVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *sign);
    void InsertIntoVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *sign);

private:
    std::unordered_map<DReaMId, MemorizedTrafficSignal> memory;
    std::unique_ptr<VisibleTrafficSignals> visibleTrafficSignals;

    // TODO load these values from the config
    const unsigned int maximumElementsInMemory = 20;
    const int maximumTimeInMemoryMs = 20000;
};

} // namespace TrafficSignalMemory

#endif // TRAFFICSIGNMEMORY_H
