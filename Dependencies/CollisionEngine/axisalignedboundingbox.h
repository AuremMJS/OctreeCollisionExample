#pragma once
#include <vector>
#include "Matrix3.h"
#include "OctreeNode.h"

struct VertexIndices
{
	int positionIndex;
	int texCoordIndex;
};

class AxisAlignedBoundingBox
{
private:
	Vec3 position;

	Vec3 minVertex;
	Vec3 maxVertex;

	std::vector<Vec3> vertices;
	std::vector<VertexIndices> indices;
	std::vector<Vec3> texCoords;

	std::vector<Vec3> octreeVertices;
	std::vector<VertexIndices> octreeIndices;

	OctreeNode *octree;

	void Construct(std::vector<Vec3> mVertices);
	void FindMinMaxVertices(std::vector<Vec3> mVertices);
	void InitAABBVertices();
	void DrawSides();
	void DrawSide(std::initializer_list <int> positions);
	void InitAABBTexCoords();
	void InitOctreeIndices();
	void InitOctreeNodeIndices(int startVertex);
	void DrawOctreeSide(int startVertex, std::initializer_list<int> positions);
public:
	AxisAlignedBoundingBox(std::vector<Vec3> mVertices,int octreeDepth);
	std::vector<Vec3> GetVertices();
	std::vector<Vec3> GetTexCoords();
	std::vector<VertexIndices> GetIndices();

	std::vector<Vec3> GetOctreeVertices();
	std::vector<VertexIndices> GetOctreeIndices();

	bool CheckCollision(AxisAlignedBoundingBox *otherAABB);
	void Translate(Vec3 translateVec);
	Vec3 GetPosition();
};