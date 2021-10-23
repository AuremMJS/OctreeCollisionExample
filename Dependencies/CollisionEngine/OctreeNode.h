#pragma once
#include <vector>
#include "Matrix3.h"

class OctreeNode
{
private:
	Vec3 position;
	Vec3 size;
	Vec3 minPosition;
	Vec3 maxPosition;
	int depth;
	bool hasMeshVertex;
	OctreeNode *subNodes;

	OctreeNode();
	OctreeNode(Vec3 pos, Vec3 size, int depth);
	void GetVertices(std::vector<Vec3> &vertices);
	bool CheckIfPointInOctree(Vec3 point);
	inline bool IsPointInLine(float point, float lineStart, float lineEnd);
	inline bool IsLinesIntersecting(float lineOneStart, float lineOneEnd, float lineTwoStart, float lineTwoEnd);
public:
	OctreeNode(int depth, Vec3 minPos, Vec3 maxPos);
	void Subdivide(int depth = 1);
	void GetLeafVertices(std::vector<Vec3> &vertices);
	void InitMeshVertexInfo(std::vector<Vec3> &meshVertices);
	bool IsLeaf();
	void Translate(Vec3 position);
	bool CheckCollision(OctreeNode *otherNode,
		Vec3 currentNodePosition, Vec3 otherNodePosition,
		std::vector<OctreeNode*> *collidedLeaves);
};