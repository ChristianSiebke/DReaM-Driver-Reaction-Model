/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#ifndef TRAFFICSIGNMEMORY_H
#define TRAFFICSIGNMEMORY_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "Common/MentalInfrastructure/Trafficsign.h"
#include "Common/WorldRepresentation.h"
#include "Components/ComponentInterface.h"

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
    std::unordered_map<DReaMId, MemorizedTrafficSign> memory;
    std::unique_ptr<VisibleTrafficSigns> visibleTrafficSigns;

    const unsigned int maximumElementsInMemory = 20;
    const int maximumTimeInMemoryMs = 20000;
};

} // namespace TrafficSignMemory

#endif // TRAFFICSIGNMEMORY_H
