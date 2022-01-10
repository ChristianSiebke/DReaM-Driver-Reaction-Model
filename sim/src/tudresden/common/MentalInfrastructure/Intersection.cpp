#include "Intersection.h"
#include "Road.h"
#include <algorithm>

namespace MentalInfrastructure {

std::string Intersection::GetOpenDriveId() const { return openDriveId; }

const Road* Intersection::GetRoadForConnection(const Road* from, const Road* to) const {

    for (auto connection : connections) {
        if (connection.from == from && connection.to == to) {
            return connection.with;
        }
    }

    return nullptr;
}

const std::vector<const Road*> Intersection::GetPossibleConnections(const Road* from) const {
    std::vector<const Road*> toReturn;
    for (auto connection : connections) {
        if (connection.from == from) {
            toReturn.push_back(connection.to);
        }
    }

    return toReturn;
}

const std::vector<const Road*> Intersection::GetConnectionRoads(const Road* from) const {
    std::vector<const Road*> toReturn;
    for (auto connection : connections) {
        if (connection.from == from) {
            toReturn.push_back(connection.with);
        }
    }
    return toReturn;
}

const Road* Intersection::GetNextRoad(const Road* on) {
    for (auto connection : connections) {
        if (connection.with == on) {
            return connection.to;
        }
    }
    return nullptr;
}
const std::vector<const Road*> Intersection::PredecessorConnectionRoads(const Road* to) const {
    std::vector<const Road*> toReturn;
    for (auto connection : connections) {
        if (connection.to == to) {
            toReturn.push_back(connection.with);
        }
    }
    return toReturn;
}

void Intersection::AddConnection(const Road* from, const Road* with, const Road* to) {
    IntersectionConnection connection;
    connection.from = from;
    connection.to = to;
    connection.with = with;

    connections.push_back(connection);
    roadsOnIntersection.push_back(with);

    if (std::find(incomingRoads.begin(), incomingRoads.end(), from) == incomingRoads.end()) {
        incomingRoads.push_back(from);
    }
}
} // namespace MentalInfrastructure
