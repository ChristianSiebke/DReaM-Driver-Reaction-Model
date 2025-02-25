#include "TrafficSignalMemory.h"

namespace TrafficSignalMemory {

struct VectorCompare {
    bool operator()(std::pair<DReaMId, MemorizedTrafficSignal> const &left, std::pair<DReaMId, MemorizedTrafficSignal> const &right) const {
        return left.second.lastTimeStamp < right.second.lastTimeStamp;
    }
};

void UpdateSpeedLimits(VisibleTrafficSignals *visibleTrafficSignals, DetailedAgentPerception *ego) {
    // updating the current speed limit
    auto laneId = ego->lanePosition.lane->GetDReaMId();
    auto signsOnLane = visibleTrafficSignals->GetSignsForLane(laneId);

    // sort the signs based on s coordinate
    // take into account the direction the agent is moving on the lane
    std::sort(signsOnLane.begin(), signsOnLane.end(),
              [ego](const MentalInfrastructure::TrafficSign *a, const MentalInfrastructure::TrafficSign *b) {
                  return ego->movingInLaneDirection ? a->GetS() < b->GetS() : a->GetS() > b->GetS();
              });

    const MentalInfrastructure::TrafficSign *lastPassedSpeedLimitSign = nullptr;

    if (signsOnLane.size() > 1) {
        for (unsigned int i = 0; i < signsOnLane.size() - 1; i++) {
            if (ego->movingInLaneDirection) {
                if (signsOnLane.at(i)->GetS() < ego->lanePosition.sCoordinate &&
                    signsOnLane.at(i + 1)->GetS() > ego->lanePosition.sCoordinate && static_cast<int>(signsOnLane.at(i)->GetType()) > 100) {
                    lastPassedSpeedLimitSign = signsOnLane.at(i);
                    break;
                }
            }
            else {
                if (signsOnLane.at(i)->GetS() > ego->lanePosition.sCoordinate &&
                    signsOnLane.at(i + 1)->GetS() < ego->lanePosition.sCoordinate && static_cast<int>(signsOnLane.at(i)->GetType()) > 100) {
                    lastPassedSpeedLimitSign = signsOnLane.at(i);
                    break;
                }
            }
        }
    }
    else if (signsOnLane.size() == 1) {
        if (ego->movingInLaneDirection && ego->lanePosition.sCoordinate > signsOnLane.at(0)->GetS())
            lastPassedSpeedLimitSign = signsOnLane.at(0);
        else if (!ego->movingInLaneDirection && ego->lanePosition.sCoordinate < signsOnLane.at(0)->GetS())
            lastPassedSpeedLimitSign = signsOnLane.at(0);
    }

    if (lastPassedSpeedLimitSign != nullptr) {
        if (visibleTrafficSignals->currentSpeedLimitSign != lastPassedSpeedLimitSign) {
            visibleTrafficSignals->previousSpeedLimitSign = visibleTrafficSignals->currentSpeedLimitSign;
            visibleTrafficSignals->currentSpeedLimitSign = lastPassedSpeedLimitSign;
        }
    }
}

VisibleTrafficSignals *TrafficSignalMemory::Update(int timestamp, std::vector<const MentalInfrastructure::TrafficSignal *> input,
                                                   DetailedAgentPerception *ego) {
    for (const auto &trafficSignal : input) {
        if (memory.find(trafficSignal->GetDReaMId()) == memory.end()) {
            // the sign has its first appearance
            memory.insert(std::make_pair(trafficSignal->GetDReaMId(), MemorizedTrafficSignal{trafficSignal, timestamp, timestamp}));
            InsertIntoVisibleTrafficSignals(trafficSignal);
            continue;
        }

        memory.at(trafficSignal->GetDReaMId()).lastTimeStamp = timestamp;
    }

    // in loop element removal based on https://stackoverflow.com/questions/8234779/how-to-remove-from-a-map-while-iterating-it
    for (auto it = memory.cbegin(); it != memory.cend() /* not hoisted */; /* no increment */) {
        if (timestamp - it->second.lastTimeStamp > maximumTimeInMemoryMs) {
            EraseFromVisibleTrafficSignals(it->second.trafficSignal);
            it = memory.erase(it);
        }
        else {
            ++it;
        }
    }

    // checking if the memory capacity is exceeded
    int overhead = memory.size() - maximumTimeInMemoryMs;
    if (overhead > 0) {
        // sort signs based on their last appearance
        std::vector<std::pair<DReaMId, MemorizedTrafficSignal>> toSort(memory.begin(), memory.end());
        std::sort(toSort.begin(), toSort.end(), VectorCompare());

        for (auto i = toSort.size() - 1; i >= overhead; i--) {
            EraseFromVisibleTrafficSignals(toSort.at(i).second.trafficSignal);
            memory.erase(toSort.at(i).first);
        }
    }

    UpdateSpeedLimits(visibleTrafficSignals.get(), ego);
    visibleTrafficSignals->memory = &memory;
    return visibleTrafficSignals.get();
}

void TrafficSignalMemory::InsertIntoVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *signal) {
    auto validLanes = signal->GetValidLanes();
    for (const auto &lane : validLanes) {
        if (visibleTrafficSignals->laneTrafficSignalMap.find(lane->GetDReaMId()) == visibleTrafficSignals->laneTrafficSignalMap.end()) {
            std::list<const MentalInfrastructure::TrafficSignal *> tmp;
            tmp.push_back(signal);
            visibleTrafficSignals->laneTrafficSignalMap.insert(std::make_pair(lane->GetDReaMId(), tmp));
        }
        else {
            auto &trafficSignalList = visibleTrafficSignals->laneTrafficSignalMap.at(lane->GetDReaMId());
            if (std::find(trafficSignalList.begin(), trafficSignalList.end(), signal) == trafficSignalList.end()) {
                trafficSignalList.push_back(signal);
            }
        }
    }
}

void TrafficSignalMemory::EraseFromVisibleTrafficSignals(const MentalInfrastructure::TrafficSignal *signal) {
    auto validLanes = signal->GetValidLanes();
    for (const auto &lane : validLanes) {
        if (visibleTrafficSignals->laneTrafficSignalMap.find(lane->GetDReaMId()) != visibleTrafficSignals->laneTrafficSignalMap.end()) {
            visibleTrafficSignals->laneTrafficSignalMap.at(lane->GetDReaMId()).remove(signal);
        }
    }
}

} // namespace TrafficSignalMemory
