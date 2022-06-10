/******************************************************************************
 * Copyright (c) 2020 TU Dresden
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

#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <unordered_map>

#include "common/Definitions.h"
// namespace OWL {
// namespace Interfaces {
// class Lane;
// }
// } // namespace OWL

namespace MentalInfrastructure {
class Lane;
class Section;
class Road;
} // namespace MentalInfrastructure

namespace RoadmapGraph {

class RoadmapNode {
public:
    RoadmapNode(const MentalInfrastructure::Lane *lane, OdId OdLaneId, OdId OdRoadId, double length) :
        lane{lane}, OdLaneId{OdLaneId}, OdRoadId{OdRoadId}, length{length} {
    }

    ~RoadmapNode() = default;

    const std::list<const RoadmapNode *> &GetSuccessorNodes() const {
        return successorNodes;
    }
    const std::list<const RoadmapNode *> &GetPredecessorNodes() const {
        return predecessorNodes;
    }
    const MentalInfrastructure::Lane *GetNode() const {
        return lane;
    }
    double GetLength() const {
        return length;
    }
    OdId GetOdLaneId() const {
        return OdLaneId;
    }
    const OdId GetOdRoadId() const {
        return OdRoadId;
    }
    void AddSuccessor(RoadmapNode *);
    void AddPredecessor(RoadmapNode *);

private:
    std::list<const RoadmapNode *> successorNodes;
    std::list<const RoadmapNode *> predecessorNodes;

    const MentalInfrastructure::Lane *lane;
    const OdId OdLaneId;
    const OdId OdRoadId;
    double length;
};

class RoadmapGraph {
public:
    RoadmapGraph() {
    }
    RoadmapGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes);

    const std::unordered_map<const MentalInfrastructure::Lane *, std::shared_ptr<RoadmapNode>> &GetNodes() {
        return nodes;
    }

    const RoadmapNode *NavigateToTargetNode(OdId targetRoadOdId, OwlId targetLaneOdId) {
        return OdMapping.at(targetRoadOdId).at(targetLaneOdId);
    }

    /*!
     * \brief Finds shortest Path between two Lanes
     *
     * Takes the lengt of lanes to calculate the shortest path between two given lanes in a road network
     *
     * \param
     * \return shortest path as a list of lane-IDs from start to end
     */
    const std::list<const RoadmapNode *> FindShortestPath(const MentalInfrastructure::Lane *start,
                                                          const MentalInfrastructure::Lane *end) const;

    /*!
     * \brief Initialize a map with length and predecessor values for Dijkastra algorithm
     * \param
     * \return Map filled with default starting values
     */
private:
    void AddNode(std::shared_ptr<RoadmapNode> node) {
        nodes.insert(std::make_pair(node->GetNode(), std::move(node)));
    }

    std::unordered_map<const RoadmapNode *, std::pair<double, const RoadmapNode *>>
    InitializeDijkstra(const std::shared_ptr<RoadmapNode> &node) const;

    std::unordered_map<const MentalInfrastructure::Lane *, std::shared_ptr<RoadmapNode>> nodes;
    std::unordered_map<OdId, std::unordered_map<OwlId, const RoadmapNode *>> OdMapping;
};
} // namespace RoadmapGraph
