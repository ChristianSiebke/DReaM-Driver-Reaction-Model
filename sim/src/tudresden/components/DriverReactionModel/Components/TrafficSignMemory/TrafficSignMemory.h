#ifndef TRAFFICSIGNMEMORY_H
#define TRAFFICSIGNMEMORY_H

#include "WorldRepresentation.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "Components/ComponentInterface.h"
#include "Trafficsign.h"

namespace TrafficSignMemory {

struct MemorizedTrafficSign {
    const MentalInfrastructure::TrafficSign* trafficSign;
    int firstTimeStamp;
    int lastTimeStamp;
};

class TrafficSignMemory {
  public:
    TrafficSignMemory() { visibleTrafficSigns = std::make_unique<VisibleTrafficSigns>(); }

    VisibleTrafficSigns* Update(int timestamp, std::vector<const MentalInfrastructure::TrafficSign*> input);

  private:
    void EraseFromVisibleTrafficSigns(const MentalInfrastructure::TrafficSign* sign);
    void InsertIntoVisibleTrafficSigns(const MentalInfrastructure::TrafficSign* sign);

  private:
    std::unordered_map<Id, MemorizedTrafficSign> memory;
    std::unique_ptr<VisibleTrafficSigns> visibleTrafficSigns;

    const unsigned int maximumElementsInMemory = 20;
    const int maximumTimeInMemoryMs = 20000;
};

} // namespace TrafficSignMemory

#endif // TRAFFICSIGNMEMORY_H
