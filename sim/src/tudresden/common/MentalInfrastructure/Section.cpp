#include "Section.h"

namespace MentalInfrastructure {

const Road* Section::GetRoad() const { return road; }

const Section* Section::GetSuccessor() const { return successorSection; }

const Section* Section::GetPredecessor() const { return predecessorSection; }

const std::vector<const Lane*>& Section::GetLanes() const { return lanes; }

void Section::AddLane(const Lane* lane) {
    if (std::find(lanes.begin(), lanes.end(), lane) == lanes.end()) {
        lanes.push_back(lane);
    }
}

std::vector<const Lane*> Section::GetLanesInRoadDirection() const {
    std::vector<const Lane*> toReturn;
    std::for_each(lanes.begin(), lanes.end(), [&toReturn](const Lane* element) {
        if (element->IsInRoadDirection())
            toReturn.push_back(element);
    });
    return toReturn;
}
std::vector<const Lane*> Section::GetLanesNotInRoadDirection() const {
    std::vector<const Lane*> toReturn;
    std::for_each(lanes.begin(), lanes.end(), [&toReturn](const Lane* element) {
        if (!element->IsInRoadDirection())
            toReturn.push_back(element);
    });
    return toReturn;
}
} // namespace MentalInfrastructure
