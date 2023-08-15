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

#include "aabb.h"
#include "aabbInterface.h"

#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <vector>

#define AABB_NULL_NODE 0xffffffff

struct AABBNode {
    AABB aabb;
    std::shared_ptr<AABBInterface> object = nullptr;

    unsigned parentNodeIndex;
    unsigned leftNodeIndex;
    unsigned rightNodeIndex;
    // linked list link
    unsigned nextNodeIndex;

    bool IsLeaf() const { return leftNodeIndex == AABB_NULL_NODE; }

    AABBNode()
        : object(nullptr), parentNodeIndex(AABB_NULL_NODE), leftNodeIndex(AABB_NULL_NODE), rightNodeIndex(AABB_NULL_NODE),
          nextNodeIndex(AABB_NULL_NODE) {}
};

class AABBTree {
  public:
    AABBTree(unsigned initialSize);
    ~AABBTree();

    void InsertObject(const std::shared_ptr<AABBInterface>& object);
    void RemoveObject(const std::shared_ptr<AABBInterface>& object);
    void UpdateObject(const std::shared_ptr<AABBInterface>& object);
    std::forward_list<std::shared_ptr<AABBInterface>> QueryOverlaps(const std::shared_ptr<AABBInterface>& object) const;
    std::list<std::pair<std::shared_ptr<AABBInterface>, double>> QueryRay(const Ray& ray) const;
    std::shared_ptr<AABBInterface> FindClosestByRay(const Ray& ray, bool excludeFirst = true) const;

    int GetLeafElementCount() const { return objectNodeIndexMap.size(); }
    int GetTotalElementCount() const { return nodes.size(); }

  private:
    unsigned rootNodeIndex;
    unsigned allocatedNodeCount;
    unsigned nextFreeNodeIndex;
    unsigned nodeCapacity;
    unsigned growthSize;

    unsigned AllocateNode();
    void DeAllocateNode(unsigned nodeIndex);
    void InsertLeaf(unsigned leafNodeIndex);
    void RemoveLeaf(unsigned leafNodeIndex);
    void UpdateLeaf(unsigned leafNodeIndex, const AABB& aabb);
    void FixUpwardsTree(unsigned treeNodeIndex);

  private:
      std::map<std::shared_ptr<AABBInterface>, unsigned> objectNodeIndexMap{};
      std::vector<AABBNode> nodes{};
};
