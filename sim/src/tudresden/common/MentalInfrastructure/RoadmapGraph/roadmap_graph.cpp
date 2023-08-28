/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/

#include "roadmap_graph.h"

#include <string>

#include "common/MentalInfrastructure/Lane.h"
#include "common/MentalInfrastructure/Road.h"

namespace RoadmapGraph {

RoadmapGraph::RoadmapGraph(std::vector<std::shared_ptr<const MentalInfrastructure::Lane>> &lanes) {
    nodes.clear();

    for (auto lane : lanes) {
        auto OdLaneId = lane->GetOpenDriveId();
        auto OdRoadId = lane->GetRoad()->GetOpenDriveId();
        auto length = lane->GetLength();

        std::shared_ptr<RoadmapNode> node = std::make_shared<RoadmapNode>(lane.get(), OdLaneId, OdRoadId, length);
        AddNode(std::move(node));
    }
    for (auto &[lane, node] : nodes) {
        auto predecessors = lane->GetPredecessors();
        auto successors = lane->GetSuccessors();
        for (const auto &pre : predecessors) {
            node->AddPredecessor(nodes.at(pre).get());
        }
        for (const auto &suc : successors) {
            node->AddSuccessor(nodes.at(suc).get());
        }

        auto leftLane = lane->GetLeftLane();
        if (leftLane != nullptr) {
            node->AddSuccessor(nodes.at(leftLane).get());
            auto leftLaneNode = nodes.at(leftLane).get();
            leftLaneNode->AddPredecessor(node.get());
        }

        auto rightLane = lane->GetRightLane();
        if (rightLane != nullptr) {
            node->AddSuccessor(nodes.at(rightLane).get());
            auto rightLaneNode = nodes.at(rightLane).get();
            rightLaneNode->AddPredecessor(node.get());
        }
    }

    for (const auto &[id, node] : nodes) {
        auto laneId = node->GetNode()->GetOwlId();
        auto OdRoadId = node->GetOdRoadId();

        if (OdMapping.find(OdRoadId) != OdMapping.end()) {
            OdMapping.at(OdRoadId).insert(std::make_pair(laneId, node.get()));
        }
        else {
            std::unordered_map<OwlId, const RoadmapNode *> temp;
            temp.insert(std::make_pair(laneId, node.get()));
            OdMapping.insert(std::make_pair(OdRoadId, temp));
        }
    }
}

const std::list<const RoadmapNode *> RoadmapGraph::FindShortestPath(const MentalInfrastructure::Lane *start,
                                                                    const MentalInfrastructure::Lane *end) const {
    if (start == nullptr || end == nullptr) {
        const std::string msg = "File: " + static_cast<std::string>(__FILE__) + " Line: " + std::to_string(__LINE__) + " error";
        throw std::runtime_error(msg);
    }
    const auto &startNode = nodes.at(start);
    const auto endNode = nodes.at(end).get();

    std::list<const RoadmapNode *> path;

    std::unordered_map<const RoadmapNode *, std::pair<double, const RoadmapNode *>> dijkstragraph = InitializeDijkstra(startNode);

    // use a list of nodes since we do not access the Id anymore (previously map)
    std::list<const RoadmapNode *> uncheckedNodes;

    std::for_each(nodes.begin(), nodes.end(),
                  [&uncheckedNodes](const std::pair<const MentalInfrastructure::Lane *, std::shared_ptr<RoadmapNode>> &element) {
                      uncheckedNodes.push_back(element.second.get());
                  });

    while (!uncheckedNodes.empty()) {
        // sort the list of unchecked nodes based on the stored distance inside the dijkstragraph
        uncheckedNodes.sort([&dijkstragraph](const RoadmapNode *&first, const RoadmapNode *&second) {
            return dijkstragraph.at(first).first > dijkstragraph.at(second).first;
        });

        const RoadmapNode *shortest = uncheckedNodes.back();
        uncheckedNodes.pop_back();

        for (const auto &neighbour : shortest->GetSuccessorNodes()) {
            // check if one node has the same ID as the neighbour
            auto res = std::any_of(uncheckedNodes.begin(), uncheckedNodes.end(),
                                   [&neighbour](const RoadmapNode *node) { return node->GetNode() == neighbour->GetNode(); });

            if (res) {
                auto alt = (dijkstragraph.at(shortest)).first + shortest->GetLength();

                if (dijkstragraph.at(neighbour).first > alt) {
                    dijkstragraph.at(neighbour) = std::make_pair(alt, shortest);
                }
            }
        }
    }

    path.push_back(endNode);
    while ((dijkstragraph.at(path.front())).second != nullptr) {
        const RoadmapNode *front = path.front();
        path.push_front((dijkstragraph.at(front)).second);
    }

    if (std::none_of(path.begin(), path.end(), [startNode](auto element) { return element == startNode.get(); }) ||
        std::none_of(path.begin(), path.end(), [endNode](auto element) { return element == endNode; })) {
        auto msg = __FILE__ " | " + std::to_string(__LINE__) + " | There exist no path to chosen TargetLane";
        throw std::runtime_error(msg);
    }
    return path;
}

std::unordered_map<const RoadmapNode *, std::pair<double, const RoadmapNode *>>
RoadmapGraph::InitializeDijkstra(const std::shared_ptr<RoadmapNode> &startNode) const {
    std::unordered_map<const RoadmapNode *, std::pair<double, const RoadmapNode *>> dijkstragraph;
    for (auto const &[id, node] : nodes) {
        if (&startNode == &node) {
            dijkstragraph.insert(
                std::pair<const RoadmapNode *, std::pair<double, const RoadmapNode *>>(node.get(), std::make_pair(0, nullptr)));
        }
        else {
            dijkstragraph.insert(
                std::pair<const RoadmapNode *, std::pair<double, const RoadmapNode *>>(node.get(), std::make_pair(INFINITY, nullptr)));
        }
    }
    return dijkstragraph;
}

void RoadmapNode::AddSuccessor(RoadmapNode *node) {
    this->successorNodes.push_back(node);
}

void RoadmapNode::AddPredecessor(RoadmapNode *node) {
    this->predecessorNodes.push_back(node);
}
} // namespace RoadmapGraph
