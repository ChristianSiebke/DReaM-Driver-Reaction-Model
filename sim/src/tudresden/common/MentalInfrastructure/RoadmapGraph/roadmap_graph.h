/******************************************************************************
 * Copyright (c) 2020 TU Dresden
 * student assistantal:   Jan       Sommer
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

class RoadmapNode
{
public:
    RoadmapNode(const MentalInfrastructure::Lane *lane, int64_t OdLaneId, std::string OdRoadId, double length) :
        lane{lane}, OdLaneId{OdLaneId}, OdRoadId{OdRoadId}, length{length}
    {
    }

    ~RoadmapNode() = default;

    const std::list<const RoadmapNode *> &GetSuccessorNodes() const
    {
        return successorNodes;
    }
    const std::list<const RoadmapNode *> &GetPredecessorNodes() const
    {
        return predecessorNodes;
    }
    const MentalInfrastructure::Lane *GetNode() const
    {
        return lane;
    }
    double GetLength() const
    {
        return length;
    }
    int64_t GetOdLaneId() const
    {
        return OdLaneId;
    }
    const std::string GetOdRoadId() const
    {
        return OdRoadId;
    }
    void AddSuccessor(RoadmapNode *);
    void AddPredecessor(RoadmapNode *);

private:
    std::list<const RoadmapNode *> successorNodes;
    std::list<const RoadmapNode *> predecessorNodes;

    const MentalInfrastructure::Lane *lane;
    const int64_t OdLaneId;
    const std::string OdRoadId;
    double length;
};

class RoadmapGraph
{
public:
    RoadmapGraph()
    {
    }
    RoadmapGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes,
                 const std::unordered_map<Id, int64_t> &LaneIdMapping, const std::unordered_map<Id, std::string> &RoadIdMapping);

    const std::unordered_map<const MentalInfrastructure::Lane *, std::shared_ptr<RoadmapNode>> &GetNodes()
    {
        return nodes;
    }

    const RoadmapNode *NavigateToTargetNode(std::string targetRoadOdId, int64_t targetLaneOdId)
    {
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
    void AddNode(std::shared_ptr<RoadmapNode> node)
    {
        nodes.insert(std::make_pair(node->GetNode(), std::move(node)));
    }

    std::unordered_map<const RoadmapNode *, std::pair<double, const RoadmapNode *>>
    InitializeDijkstra(const std::shared_ptr<RoadmapNode> &node) const;

    std::unordered_map<const MentalInfrastructure::Lane *, std::shared_ptr<RoadmapNode>> nodes;
    std::unordered_map<std::string, std::unordered_map<int64_t, const RoadmapNode *>> OdMapping;
    std::unordered_map<Id, int64_t> LaneIdMapping;
    std::unordered_map<Id, std::string> RoadIdMapping;
};
} // namespace RoadmapGraph
