#include "TrafficSignMemory.h"

namespace TrafficSignMemory {

struct VectorCompare {
    bool operator()(std::pair<OwlId, MemorizedTrafficSign> const &left, std::pair<OwlId, MemorizedTrafficSign> const &right) const {
        return left.second.lastTimeStamp < right.second.lastTimeStamp;
    }
};

VisibleTrafficSigns* TrafficSignMemory::Update(int timestamp, std::vector<const MentalInfrastructure::TrafficSign*> input) {

    for (const auto& trafficSign : input) {
        if (memory.find(trafficSign->GetOwlId()) == memory.end()) {
            // the sign has its first appearance
            memory.insert(std::make_pair(trafficSign->GetOwlId(), MemorizedTrafficSign{trafficSign, timestamp, timestamp}));
            InsertIntoVisibleTrafficSigns(trafficSign);
            continue;
        }

        memory.at(trafficSign->GetOwlId()).lastTimeStamp = timestamp;
    }

    // in loop element removal based on https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
    for (auto it = memory.cbegin(); it != memory.cend() /* not hoisted */; /* no increment */) {
        if (timestamp - it->second.lastTimeStamp > maximumTimeInMemoryMs) {
            auto toErase = it++;
            EraseFromVisibleTrafficSigns(toErase->second.trafficSign);
            memory.erase(toErase);
        } else {
            ++it;
        }
    }

    // checking if the memory capacity is exceeded
    auto overhead = memory.size() - maximumTimeInMemoryMs;
    if (overhead > 0) {
        // sort signs based on their last appearance
        std::vector<std::pair<OwlId, MemorizedTrafficSign>> toSort(memory.begin(), memory.end());
        std::sort(toSort.begin(), toSort.end(), VectorCompare());

        for (auto i = toSort.size(); i >= overhead; i--) {
            EraseFromVisibleTrafficSigns(toSort[i].second.trafficSign);
            memory.erase(toSort[i].first);
        }
    }

    return visibleTrafficSigns.get();
}

void TrafficSignMemory::InsertIntoVisibleTrafficSigns(const MentalInfrastructure::TrafficSign* sign) {
    // check if a sign on this road was already seen
    auto foundRoadInVisibleTrafficSigns = visibleTrafficSigns->trafficSigns.find(sign->GetRoad()->GetOpenDriveId());

    // if not add the road
    if (foundRoadInVisibleTrafficSigns == visibleTrafficSigns->trafficSigns.end()) {
        std::unordered_map<MentalInfrastructure::TrafficSignType, std::unordered_map<OwlId, const MentalInfrastructure::TrafficSign *>> tmp;
        visibleTrafficSigns->trafficSigns.insert(std::make_pair(sign->GetRoad()->GetOpenDriveId(), tmp));
    }

    auto parentToEdit = &visibleTrafficSigns->trafficSigns.find(sign->GetRoad()->GetOpenDriveId())->second;

    // check if a sign of this type was already seen
    auto foundType = parentToEdit->find(sign->GetType());

    if (foundType == parentToEdit->end()) {
        std::unordered_map<OwlId, const MentalInfrastructure::TrafficSign *> tmp;
        tmp.insert(std::make_pair(sign->GetOwlId(), sign));
        parentToEdit->insert(std::make_pair(sign->GetType(), tmp));
    } else {
        foundType->second.insert(std::make_pair(sign->GetOwlId(), sign));
    }

    // hehe
}

void TrafficSignMemory::EraseFromVisibleTrafficSigns(const MentalInfrastructure::TrafficSign* sign) {
    visibleTrafficSigns->trafficSigns.at(sign->GetRoad()->GetOpenDriveId()).at(sign->GetType()).erase(sign->GetOwlId());
}

} // namespace TrafficSignMemory
