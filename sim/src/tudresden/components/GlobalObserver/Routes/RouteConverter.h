/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#pragma once
#include <vector>

#include "Common/Definitions.h"
#include "RouteImporter.h"
#include "WorldData.h"

namespace GlobalObserver::Routes {

struct InternWaypoint {
    OdId roadId{""};
    OwlId lane{999};
    double s{-999};
};

class RouteConverter {
public:
    RouteConverter(WorldInterface *world) : world{world}{};
    std::vector<InternWaypoint> Convert(std::vector<Import::Position> route);
    OwlId GetLane(OdId roadId, int laneId, double s) const;
    OwlId GetLane(OdId roadId, double t, double s) const;

private:
    WorldInterface *world;
};
}