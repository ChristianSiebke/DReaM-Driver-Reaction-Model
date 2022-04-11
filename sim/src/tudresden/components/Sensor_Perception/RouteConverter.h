/******************************************************************************
 * Copyright (c) 2022 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *****************************************************************************/

#pragma once
#include <vector>

#include "RouteImporter.h"
#include "WorldData.h"

struct InternWaypoint {
    OdId roadId{};
    OwlId lane;
    double s{};
};

class RouteConverter {
public:
    RouteConverter(WorldInterface *world) : world(world){};
    std::vector<InternWaypoint> Convert(std::vector<Import::Position> route);
    OwlId GetLane(OdId roadId, int laneId, double s) const;
    OwlId GetLane(OdId roadId, double t, double s) const;

private:
    WorldInterface *world;
};