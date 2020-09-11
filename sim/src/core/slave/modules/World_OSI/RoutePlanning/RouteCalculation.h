/*******************************************************************************
* Copyright (c) 2020 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/worldDefinitions.h"
#include "include/stochasticsInterface.h"

namespace RouteCalculation
{
    RoadGraphVertex FilterRoadGraphByStartPositionRecursive (const RoadGraph& roadGraph, RoadGraphVertex current, int maxDepth, RoadGraph& filteredGraph)
    {
        const auto& routeElement = get(RouteElement(), roadGraph, current);
        auto newVertex = add_vertex(routeElement, filteredGraph);
        if (maxDepth > 1)
        {
            for (auto [successor, successorsEnd] = adjacent_vertices(current, roadGraph); successor != successorsEnd; successor++)
            {
                auto successorVertex = FilterRoadGraphByStartPositionRecursive(roadGraph, *successor, maxDepth - 1, filteredGraph);
                add_edge(newVertex, successorVertex, filteredGraph);
            }
        }
        return newVertex;
    }

    std::pair<RoadGraph, RoadGraphVertex> FilterRoadGraphByStartPosition (const RoadGraph& roadGraph, RoadGraphVertex start, int maxDepth)
    {
        RoadGraph filteredGraph;
        auto root = FilterRoadGraphByStartPositionRecursive(roadGraph, start, maxDepth, filteredGraph);
        return {filteredGraph, root};
    }

    std::pair<RoadGraph, RoadGraphVertex> SampleRoute (const RoadGraph& roadGraph,
                                                       RoadGraphVertex root,
                                                       const std::map<RoadGraphEdge, double>& weights,
                                                       StochasticsInterface& stochastics)
    {
        RoadGraph route;
        auto firstVertex = add_vertex(get(RouteElement(), roadGraph, root), route);
        auto current = root;
        bool reachedEnd = false;
        while (!reachedEnd)
        {
            if (out_degree(current, roadGraph) == 0)
            {
                reachedEnd = true;
                break;
            }
            double weightSum = 0.0;
            for (auto [successor, successorEnd] = out_edges(current, roadGraph); successor != successorEnd; successor++)
            {
                weightSum += weights.at(*successor);
            }
            auto roll = stochastics.GetUniformDistributed(0, weightSum);
            double probalitySum = 0.0;
            for (auto [successor, successorEnd] = out_edges(current, roadGraph); successor != successorEnd; successor++)
            {
                probalitySum += weights.at(*successor);
                if (roll <= probalitySum)
                {
                    auto secondVertex = add_vertex(get(RouteElement(), roadGraph, target(*successor, roadGraph)), route);
                    add_edge(firstVertex, secondVertex, route);
                    current = target(*successor, roadGraph);
                    firstVertex = secondVertex;
                    break;
                }
            }
        }
        return {route, firstVertex};
    }
}
