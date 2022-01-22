#include "aabbtree.h"

#include <assert.h>
#include <iostream>
#include <stack>

AABBTree::AABBTree(unsigned initialSize) :
    rootNodeIndex(AABB_NULL_NODE), allocatedNodeCount(0), nextFreeNodeIndex(0), nodeCapacity(initialSize), growthSize(initialSize)
{
    nodes.resize(initialSize);
    for (unsigned nodeIndex = 0; nodeIndex < initialSize; nodeIndex++)
    {
        auto &node = nodes[nodeIndex];
        node.nextNodeIndex = nodeIndex + 1;
    }
    nodes[initialSize - 1].nextNodeIndex = AABB_NULL_NODE;
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
            AABBNode &node = nodes[nodeIndex];
            node.nextNodeIndex = nodeIndex + 1;
        }
        nodes[nodeCapacity - 1].nextNodeIndex = AABB_NULL_NODE;
        nextFreeNodeIndex = allocatedNodeCount;
    }

    unsigned nodeIndex = nextFreeNodeIndex;
    AABBNode &allocatedNode = nodes[nodeIndex];
    allocatedNode.parentNodeIndex = AABB_NULL_NODE;
    allocatedNode.leftNodeIndex = AABB_NULL_NODE;
    allocatedNode.rightNodeIndex = AABB_NULL_NODE;
    nextFreeNodeIndex = allocatedNode.nextNodeIndex;
    allocatedNodeCount++;

    return nodeIndex;
}

void AABBTree::DeAllocateNode(unsigned int nodeIndex)
{
    AABBNode &deallocatedNode = nodes[nodeIndex];
    deallocatedNode.nextNodeIndex = nextFreeNodeIndex;
    nextFreeNodeIndex = nodeIndex;
    allocatedNodeCount--;
}

void AABBTree::InsertObject(const std::shared_ptr<AABBInterface> &object)
{
    auto nodeIndex = AllocateNode();
    AABBNode &node = nodes[nodeIndex];

    node.aabb = object->GetAABB();
    node.object = object;

    InsertLeaf(nodeIndex);
    objectNodeIndexMap[object] = nodeIndex;
}

void AABBTree::RemoveObject(const std::shared_ptr<AABBInterface> &object)
{
    unsigned nodeIndex = objectNodeIndexMap[object];
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

    unsigned nodeIndex = objectNodeIndexMap[object];
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

        const auto &node = nodes[nodeIndex];
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

        const auto &node = nodes[nodeIndex];
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

        auto &node = nodes[nodeIndex];
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
    assert(nodes[leafNodeIndex].parentNodeIndex == AABB_NULL_NODE);
    assert(nodes[leafNodeIndex].leftNodeIndex == AABB_NULL_NODE);
    assert(nodes[leafNodeIndex].rightNodeIndex == AABB_NULL_NODE);

    if (rootNodeIndex == AABB_NULL_NODE)
    {
        rootNodeIndex = leafNodeIndex;
        return;
    }

    unsigned treeNodeIndex = rootNodeIndex;
    while (!nodes[treeNodeIndex].IsLeaf())
    {
        unsigned leftNodeIndex = nodes[treeNodeIndex].leftNodeIndex;
        unsigned rightNodeIndex = nodes[treeNodeIndex].rightNodeIndex;

        AABB combinedAABB = nodes[treeNodeIndex].aabb.Merge(nodes[leafNodeIndex].aabb);

        float newParentNodeCost = 2.0f * combinedAABB.surfaceArea;
        float minimumPushDownCost = 2.0f * (combinedAABB.surfaceArea - nodes[treeNodeIndex].aabb.surfaceArea);

        float costLeft, costRight;
        if (nodes[leftNodeIndex].IsLeaf())
        {
            costLeft = nodes[leafNodeIndex].aabb.Merge(nodes[leftNodeIndex].aabb).surfaceArea + minimumPushDownCost;
        }
        else
        {
            AABB newLeftAabb = nodes[leafNodeIndex].aabb.Merge(nodes[leftNodeIndex].aabb);
            costLeft = (newLeftAabb.surfaceArea - nodes[leftNodeIndex].aabb.surfaceArea) + minimumPushDownCost;
        }
        if (nodes[rightNodeIndex].IsLeaf())
        {
            costRight = nodes[leafNodeIndex].aabb.Merge(nodes[rightNodeIndex].aabb).surfaceArea + minimumPushDownCost;
        }
        else
        {
            AABB newRightAabb = nodes[leafNodeIndex].aabb.Merge(nodes[rightNodeIndex].aabb);
            costRight = (newRightAabb.surfaceArea - nodes[rightNodeIndex].aabb.surfaceArea) + minimumPushDownCost;
        }

        if (newParentNodeCost < costLeft && newParentNodeCost < costRight)
            break;

        treeNodeIndex = costLeft < costRight ? leftNodeIndex : rightNodeIndex;
    }

    unsigned leafSiblingIndex = treeNodeIndex;
    unsigned oldParentIndex = nodes[leafSiblingIndex].parentNodeIndex;
    unsigned newParentIndex = AllocateNode();
    nodes[newParentIndex].parentNodeIndex = oldParentIndex;
    nodes[newParentIndex].aabb = nodes[leafNodeIndex].aabb.Merge(nodes[leafSiblingIndex].aabb);
    nodes[newParentIndex].leftNodeIndex = leafSiblingIndex;
    nodes[newParentIndex].rightNodeIndex = leafNodeIndex;
    nodes[leafNodeIndex].parentNodeIndex = newParentIndex;
    nodes[leafSiblingIndex].parentNodeIndex = newParentIndex;

    if (oldParentIndex == AABB_NULL_NODE)
    {
        rootNodeIndex = newParentIndex;
    }
    else
    {
        auto &oldParent = nodes[oldParentIndex];
        if (oldParent.leftNodeIndex == leafSiblingIndex)
        {
            oldParent.leftNodeIndex = newParentIndex;
        }
        else
        {
            oldParent.rightNodeIndex = newParentIndex;
        }
    }

    treeNodeIndex = nodes[leafNodeIndex].parentNodeIndex;
    FixUpwardsTree(treeNodeIndex);
}

void AABBTree::RemoveLeaf(unsigned int leafNodeIndex)
{
    if (leafNodeIndex == rootNodeIndex)
    {
        rootNodeIndex = AABB_NULL_NODE;
        return;
    }

    auto &leafNode = nodes[leafNodeIndex];
    unsigned parentNodeIndex = leafNode.parentNodeIndex;
    const auto &parentNode = nodes[parentNodeIndex];
    unsigned grandParentNodeIndex = parentNode.parentNodeIndex;
    unsigned siblingNodeIndex = parentNode.leftNodeIndex == leafNodeIndex ? parentNode.rightNodeIndex : parentNode.leftNodeIndex;

    assert(siblingNodeIndex != AABB_NULL_NODE);
    auto &siblingNode = nodes[siblingNodeIndex];

    if (grandParentNodeIndex != AABB_NULL_NODE)
    {
        auto &grandParentNode = nodes[grandParentNodeIndex];
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
    auto &node = nodes[leafNodeIndex];

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
        auto &treeNode = nodes[treeNodeIndex];

        assert(treeNode.leftNodeIndex != AABB_NULL_NODE && treeNode.rightNodeIndex != AABB_NULL_NODE);

        treeNode.aabb = nodes[treeNode.leftNodeIndex].aabb.Merge(nodes[treeNode.rightNodeIndex].aabb);
        treeNodeIndex = treeNode.parentNodeIndex;
    }
}
