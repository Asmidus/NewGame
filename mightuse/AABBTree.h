#pragma once
#include "AABB.h"
#include <memory>
#include <vector>
#include <map>
#include <forward_list>
#include "entt/entt.hpp"

#define AABB_NULL_NODE 0xffffffff

struct AABBNode
{
	AABB aabb;
	entt::entity object;
	// tree links
	unsigned parentNodeIndex;
	unsigned leftNodeIndex;
	unsigned rightNodeIndex;
	// node linked list link
	unsigned nextNodeIndex;

	bool isLeaf() const { return leftNodeIndex == AABB_NULL_NODE; }

	AABBNode() : parentNodeIndex(AABB_NULL_NODE), leftNodeIndex(AABB_NULL_NODE), rightNodeIndex(AABB_NULL_NODE), nextNodeIndex(AABB_NULL_NODE)
	{}
};

class AABBTree
{
private:
	std::map<entt::entity, unsigned> _objectNodeIndexMap;
	std::vector<AABBNode> _nodes;
	unsigned _rootNodeIndex;
	unsigned _allocatedNodeCount;
	unsigned _nextFreeNodeIndex;
	unsigned _nodeCapacity;
	unsigned _growthSize;

	unsigned allocateNode();
	void deallocateNode(unsigned nodeIndex);
	void insertLeaf(unsigned leafNodeIndex);
	void removeLeaf(unsigned leafNodeIndex);
	void updateLeaf(unsigned leafNodeIndex, const AABB& newAaab);
	void fixUpwardsTree(unsigned treeNodeIndex);
	
public:
	AABBTree(unsigned initialSize);
	~AABBTree();

	void insertObject(entt::entity object);
	void removeObject(entt::entity object);
	void updateObject(entt::entity object);
	std::forward_list<entt::entity> queryOverlaps(entt::entity object) const;
};

