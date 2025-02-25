/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "Junction.h"

#include <algorithm>

#include "Road.h"

namespace MentalInfrastructure {

const std::vector<const Road *> Junction::GetConnectionRoads(const Road *from) const {
    std::vector<const Road *> toReturn;
    for (auto connection : connections) {
        if (connection.from == from) {
            toReturn.push_back(connection.with);
        }
    }
    return toReturn;
}

const std::vector<const Road *> Junction::PredecessorConnectionRoads(const Road *to) const {
    std::vector<const Road *> toReturn;
    for (auto connection : connections) {
        if (connection.to == to) {
            toReturn.push_back(connection.with);
        }
    }
    return toReturn;
}

void Junction::AddConnection(const Road *from, const Road *with, const Road *to) {
    JunctionConnection connection;
    connection.from = from;
    connection.to = to;
    connection.with = with;

    connections.push_back(connection);
    roadsOnJunction.push_back(with);

    if (std::none_of(incomingRoads.begin(), incomingRoads.end(), [from](auto element) { return element == from; })) {
        incomingRoads.push_back(from);
    }
}
} // namespace MentalInfrastructure
