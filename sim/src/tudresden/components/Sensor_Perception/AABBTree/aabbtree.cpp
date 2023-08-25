/******************************************************************************
 * Copyright (c) 2019 TU Dresden
 * scientific assistant: Christian Siebke
 * student assistants:   Christian Gärber
 *                       Vincent   Adam
 *                       Jan       Sommer
 *
 * for further information please visit:  https://www.driver-model.de
 *****************************************************************************/
#include "aabbtree.h"

#include <assert.h>
#include <iostream>
#include <stack>

AABBTree::AABBTree(unsigned initialSize) :
    rootNodeIndex{AABB_NULL_NODE}, allocatedNodeCount{0}, nextFreeNodeIndex{0}, nodeCapacity{initialSize}, growthSize{initialSize} {
    assert(initialSize > 0);
    nodes.resize(initialSize);
    for (unsigned nodeIndex = 0; nodeIndex < initialSize; nodeIndex++)
    {
        auto &node = nodes.at(nodeIndex);
        node.nextNodeIndex = nodeIndex + 1;
    }
    nodes.at(initialSize - 1).nextNodeIndex = AABB_NULL_NODE;
}

AABBTree::~AABBTree()
{
}

unsigned AABBTree::AllocateNode()
{
    // no more free nodes, grow the pool
    if (nextFreeNodeIndex == AABB_NULL_NODE)
    {
        assert(allocatedNodeCount == nodeCapacity);

        nodeCapacity += growthSize;
        nodes.resize(nodeCapacity);

        for (unsigned nodeIndex = allocatedNodeCount; nodeIndex < nodeCapacity; nodeIndex++)
        {
            AABBNode &node = nodes.at(nodeIndex);
            node.nextNodeIndex = nodeIndex + 1;
        }
        nodes.at(nodeCapacity - 1).nextNodeIndex = AABB_NULL_NODE;
        nextFreeNodeIndex = allocatedNodeCount;
    }

    unsigned nodeIndex = nextFreeNodeIndex;
    AABBNode &allocatedNode = nodes.at(nodeIndex);
    allocatedNode.parentNodeIndex = AABB_NULL_NODE;
    allocatedNode.leftNodeIndex = AABB_NULL_NODE;
    allocatedNode.rightNodeIndex = AABB_NULL_NODE;
    nextFreeNodeIndex = allocatedNode.nextNodeIndex;
    allocatedNodeCount++;

    return nodeIndex;
}

void AABBTree::DeAllocateNode(unsigned int nodeIndex)
{
    AABBNode &deallocatedNode = nodes.at(nodeIndex);
    deallocatedNode.nextNodeIndex = nextFreeNodeIndex;
    nextFreeNodeIndex = nodeIndex;
    assert(allocatedNodeCount > 0);
    allocatedNodeCount--;
}

void AABBTree::InsertObject(const std::shared_ptr<AABBInterface> &object)
{
    auto nodeIndex = AllocateNode();
    AABBNode &node = nodes.at(nodeIndex);

    node.aabb = object->GetAABB();
    node.object = object;

    InsertLeaf(nodeIndex);
    objectNodeIndexMap.insert({object, nodeIndex});
}

void AABBTree::RemoveObject(const std::shared_ptr<AABBInterface> &object)
{
    unsigned nodeIndex = objectNodeIndexMap.at(object);
    RemoveLeaf(nodeIndex);
    DeAllocateNode(nodeIndex);
    objectNodeIndexMap.erase(object);
}

void AABBTree::UpdateObject(const std::shared_ptr<AABBInterface> &object)
{
    if (objectNodeIndexMap.find(object) == objectNodeIndexMap.end())
    {
        InsertObject(object);
    }

    unsigned nodeIndex = objectNodeIndexMap.at(object);
    UpdateLeaf(nodeIndex, object->GetAABB());
}

std::forward_list<std::shared_ptr<AABBInterface>> AABBTree::QueryOverlaps(const std::shared_ptr<AABBInterface> &object) const
{
    std::forward_list<std::shared_ptr<AABBInterface>> overlaps;
    std::stack<unsigned> stack;
    AABB testAABB = object->GetAABB();

    stack.push(rootNodeIndex); // begin traversal at root node
    while (!stack.empty())
    {
        unsigned nodeIndex = stack.top();
        stack.pop();

        if (nodeIndex == AABB_NULL_NODE)
            continue;

        const auto &node = nodes.at(nodeIndex);
        if (node.aabb.Overlaps(testAABB))
        {
            if (node.IsLeaf() && node.object != object)
            {
                overlaps.push_front(node.object);
            }
            else
            {
                stack.push(node.leftNodeIndex);
                stack.push(node.rightNodeIndex);
            }
        }
    }

    return overlaps;
}

std::shared_ptr<AABBInterface> AABBTree::FindClosestByRay(const Ray &ray, bool excludeFirst) const
{
    std::shared_ptr<AABBInterface> object = nullptr;
    std::shared_ptr<AABBInterface> lastObject = nullptr;

    double minDistance = __DBL_MAX__;

    std::stack<unsigned> stack;

    stack.push(rootNodeIndex); // begin traversal at root node
    while (!stack.empty())
    {
        unsigned nodeIndex = stack.top();
        stack.pop();

        if (nodeIndex == AABB_NULL_NODE)
            continue;

        const auto &node = nodes.at(nodeIndex);
        double distance = -__DBL_MAX__;

        if (node.aabb.HitsBox(ray, distance))
        {
            if (node.IsLeaf() && distance < minDistance)
            {
                lastObject = object;
                object = node.object;
                minDistance = distance;
            }
            else
            {
                stack.push(node.leftNodeIndex);
                stack.push(node.rightNodeIndex);
            }
        }
    }
    return excludeFirst ? lastObject : object;
}

std::list<std::pair<std::shared_ptr<AABBInterface>, double>> AABBTree::QueryRay(const Ray &ray) const
{
    std::list<std::pair<std::shared_ptr<AABBInterface>, double>> results;
    std::stack<unsigned> stack;

    stack.push(rootNodeIndex); // begin traversal at root node
    while (!stack.empty())
    {
        unsigned nodeIndex = stack.top();
        stack.pop();

        if (nodeIndex == AABB_NULL_NODE)
            continue;

        auto &node = nodes.at(nodeIndex);
        double distance = -__DBL_MAX__;

        if (node.aabb.HitsBox(ray, distance))
        {
            if (node.IsLeaf())
            {
                results.push_front(std::make_pair(node.object, distance));
            }
            else
            {
                stack.push(node.leftNodeIndex);
                stack.push(node.rightNodeIndex);
            }
        }
    }

    return results;
}

void AABBTree::InsertLeaf(unsigned int leafNodeIndex)
{
    assert(nodes.at(leafNodeIndex).parentNodeIndex == AABB_NULL_NODE);
    assert(nodes.at(leafNodeIndex).leftNodeIndex == AABB_NULL_NODE);
    assert(nodes.at(leafNodeIndex).rightNodeIndex == AABB_NULL_NODE);

    if (rootNodeIndex == AABB_NULL_NODE)
    {
        rootNodeIndex = leafNodeIndex;
        return;
    }

    unsigned treeNodeIndex = rootNodeIndex;
    while (!nodes.at(treeNodeIndex).IsLeaf()) {
        unsigned leftNodeIndex = nodes.at(treeNodeIndex).leftNodeIndex;
        unsigned rightNodeIndex = nodes.at(treeNodeIndex).rightNodeIndex;

        AABB combinedAABB = nodes.at(treeNodeIndex).aabb.Merge(nodes.at(leafNodeIndex).aabb);

        double newParentNodeCost = 2.0 * combinedAABB.surfaceArea;
        double minimumPushDownCost = 2.0 * (combinedAABB.surfaceArea - nodes.at(treeNodeIndex).aabb.surfaceArea);

        double costLeft, costRight;
        if (nodes.at(leftNodeIndex).IsLeaf()) {
            costLeft = nodes.at(leafNodeIndex).aabb.Merge(nodes.at(leftNodeIndex).aabb).surfaceArea + minimumPushDownCost;
        }
        else
        {
            AABB newLeftAabb = nodes.at(leafNodeIndex).aabb.Merge(nodes.at(leftNodeIndex).aabb);
            costLeft = (newLeftAabb.surfaceArea - nodes.at(leftNodeIndex).aabb.surfaceArea) + minimumPushDownCost;
        }
        if (nodes.at(rightNodeIndex).IsLeaf()) {
            costRight = nodes.at(leafNodeIndex).aabb.Merge(nodes.at(rightNodeIndex).aabb).surfaceArea + minimumPushDownCost;
        }
        else
        {
            AABB newRightAabb = nodes.at(leafNodeIndex).aabb.Merge(nodes.at(rightNodeIndex).aabb);
            costRight = (newRightAabb.surfaceArea - nodes.at(rightNodeIndex).aabb.surfaceArea) + minimumPushDownCost;
        }

        if (newParentNodeCost < costLeft && newParentNodeCost < costRight)
            break;

        treeNodeIndex = costLeft < costRight ? leftNodeIndex : rightNodeIndex;
    }

    unsigned leafSiblingIndex = treeNodeIndex;
    unsigned oldParentIndex = nodes.at(leafSiblingIndex).parentNodeIndex;
    unsigned newParentIndex = AllocateNode();

    nodes.at(newParentIndex).parentNodeIndex = oldParentIndex;
    nodes.at(newParentIndex).aabb = nodes.at(leafNodeIndex).aabb.Merge(nodes.at(leafSiblingIndex).aabb);
    nodes.at(newParentIndex).leftNodeIndex = leafSiblingIndex;
    nodes.at(newParentIndex).rightNodeIndex = leafNodeIndex;
    nodes.at(leafNodeIndex).parentNodeIndex = newParentIndex;
    nodes.at(leafSiblingIndex).parentNodeIndex = newParentIndex;

    if (oldParentIndex == AABB_NULL_NODE)
    {
        rootNodeIndex = newParentIndex;
    }
    else
    {
        auto &oldParent = nodes.at(oldParentIndex);
        if (oldParent.leftNodeIndex == leafSiblingIndex)
        {
            oldParent.leftNodeIndex = newParentIndex;
        }
        else
        {
            oldParent.rightNodeIndex = newParentIndex;
        }
    }

    treeNodeIndex = nodes.at(leafNodeIndex).parentNodeIndex;
    FixUpwardsTree(treeNodeIndex);
}

void AABBTree::RemoveLeaf(unsigned int leafNodeIndex)
{
    if (leafNodeIndex == rootNodeIndex)
    {
        rootNodeIndex = AABB_NULL_NODE;
        return;
    }

    auto &leafNode = nodes.at(leafNodeIndex);
    unsigned parentNodeIndex = leafNode.parentNodeIndex;
    const auto &parentNode = nodes.at(parentNodeIndex);
    unsigned grandParentNodeIndex = parentNode.parentNodeIndex;
    unsigned siblingNodeIndex = parentNode.leftNodeIndex == leafNodeIndex ? parentNode.rightNodeIndex : parentNode.leftNodeIndex;

    assert(siblingNodeIndex != AABB_NULL_NODE);
    auto &siblingNode = nodes.at(siblingNodeIndex);

    if (grandParentNodeIndex != AABB_NULL_NODE)
    {
        auto &grandParentNode = nodes.at(grandParentNodeIndex);
        if (grandParentNode.leftNodeIndex == parentNodeIndex)
        {
            grandParentNode.leftNodeIndex = siblingNodeIndex;
        }
        else
        {
            grandParentNode.rightNodeIndex = siblingNodeIndex;
        }

        siblingNode.parentNodeIndex = grandParentNodeIndex;
        DeAllocateNode(parentNodeIndex);

        FixUpwardsTree(grandParentNodeIndex);
    }
    else
    {
        rootNodeIndex = siblingNodeIndex;
        siblingNode.parentNodeIndex = AABB_NULL_NODE;
        DeAllocateNode(parentNodeIndex);
    }

    leafNode.parentNodeIndex = AABB_NULL_NODE;
}

void AABBTree::UpdateLeaf(unsigned int leafNodeIndex, const AABB &aabb)
{
    auto &node = nodes.at(leafNodeIndex);

    if (node.aabb.Contains(aabb))
        return;

    RemoveLeaf(leafNodeIndex);
    node.aabb = aabb;
    InsertLeaf(leafNodeIndex);
}

void AABBTree::FixUpwardsTree(unsigned treeNodeIndex)
{
    while (treeNodeIndex != AABB_NULL_NODE)
    {
        auto &treeNode = nodes.at(treeNodeIndex);

        assert(treeNode.leftNodeIndex != AABB_NULL_NODE && treeNode.rightNodeIndex != AABB_NULL_NODE);

        treeNode.aabb = nodes.at(treeNode.leftNodeIndex).aabb.Merge(nodes.at(treeNode.rightNodeIndex).aabb);
        treeNodeIndex = treeNode.parentNodeIndex;
    }
}
