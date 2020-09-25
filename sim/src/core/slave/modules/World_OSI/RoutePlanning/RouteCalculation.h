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
    RoadGraphVertex FilterRoadGraphByStartPositionRecursive(const RoadGraph& roadGraph, RoadGraphVertex current, int maxDepth, RoadGraph& filteredGraph)
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

    //! Returns the road graph as a tree with defined maximum depth relative to a given start position.
    //! The same route element can appear multiple times in the result, if there are multiple paths to it from the start position
    //!
    //! \param roadGraph    entire road network
    //! \param start        start position in the network
    //! \param maxDepth     maximum depth of resulting tree
    //! \return road network as tree with given root
    std::pair<RoadGraph, RoadGraphVertex> FilterRoadGraphByStartPosition(const RoadGraph& roadGraph, RoadGraphVertex start, int maxDepth)
    {
        RoadGraph filteredGraph;
        auto root = FilterRoadGraphByStartPositionRecursive(roadGraph, start, maxDepth, filteredGraph);
        return {filteredGraph, root};
    }

    //! Random draws a target leaf in the given road graph tree based on the propability of each edge
    //!
    //! \param roadGraph    tree of road network starting at the agents current position
    //! \param root         root vertex of the roadGraph
    //! \param weights      weight map of all edges of the graph
    //! \param stochastics  stochastics module
    //! \return sampled target vertex
    RoadGraphVertex SampleRoute(const RoadGraph& roadGraph,
                                RoadGraphVertex root,
                                const std::map<RoadGraphEdge, double>& weights,
                                StochasticsInterface& stochastics)
    {
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
                    current = target(*successor, roadGraph);
                    break;
                }
            }
        }
        return current;
    }
}
