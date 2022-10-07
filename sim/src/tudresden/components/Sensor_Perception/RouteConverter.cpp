/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "RouteConverter.h"

std::vector<InternWaypoint> RouteConverter::Convert(std::vector<Import::Position> route) {
    std::vector<InternWaypoint> result;
    std::transform(route.begin(), route.end(), std::back_inserter(result), [this](auto element) {
        InternWaypoint result;
        if (auto target = std::get_if<Import::RoadPosition>(&element)) {
            result.lane = GetLane(target->roadId, target->t, target->s);
            result.s = target->s;
            result.roadId = target->roadId;
        }
        else if (auto target = std::get_if<Import::LanePosition>(&element)) {
            result.lane = GetLane(target->roadId, target->laneId, target->s);
            result.s = target->s;
            result.roadId = target->roadId;
        }
        else {
            throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Waypoint is not supported");
        }
        return result;
    });
    return result;
}

OwlId RouteConverter::GetLane(OdId roadId, int laneId, double s) const {
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto roads = worldData->GetRoads();
    auto road = roads.at(roadId);
    auto sections = road->GetSections();

    for (auto section : sections) {
        if (section->GetSOffset() <= s && s <= section->GetSOffset() + section->GetLength()) {
            auto lanes = section->GetLanes();
            auto result = std::find_if(lanes.begin(), lanes.end(), [laneId](auto element) { return element->GetOdId() == laneId; });
            if (result == lanes.end()) {
                throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Waypoint does not match witch infrastructure");
            }
            return (*result)->GetId();
        }
    }
    throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Waypoint does not match witch infrastructure");
}

static bool negativeLaneSort(const OWL::Lane *a, const OWL::Lane *b) {
    return a->GetOdId() > b->GetOdId();
}
static bool positiveLaneSort(const OWL::Lane *a, const OWL::Lane *b) {
    return a->GetOdId() < b->GetOdId();
}

OwlId RouteConverter::GetLane(OdId roadId, double t, double s) const {
    auto worldData = static_cast<OWL::WorldData *>(world->GetWorldData());
    auto roads = worldData->GetRoads();
    auto road = roads.at(roadId);
    auto sections = road->GetSections();
    std::vector<const OWL::Lane *> sortedLanes;
    for (auto section : sections) {
        if (section->GetSOffset() <= s && s <= section->GetSOffset() + section->GetLength()) {
            auto lanes = section->GetLanes();
            if (t < 0) {
                std::copy_if(lanes.begin(), lanes.end(), std::back_inserter(sortedLanes),
                             [s, t](auto element) { return element->GetOdId() < 0; });
                std::sort(sortedLanes.begin(), sortedLanes.end(), negativeLaneSort);
            }
            else {
                std::copy_if(lanes.begin(), lanes.end(), std::back_inserter(sortedLanes),
                             [s, t](auto element) { return element->GetOdId() > 0; });
                std::sort(sortedLanes.begin(), sortedLanes.end(), positiveLaneSort);
            }
        }
    }
    double widthSum = 0;
    for (auto lane : sortedLanes) {
        widthSum += lane->GetWidth(s);
        if ((widthSum > t && t > 0) || (-widthSum < t && t < 0))
            return lane->GetId();
    }
    throw std::logic_error(__FILE__ " " + std::to_string(__LINE__) + " Waypoint does not match witch infrastructure");
}