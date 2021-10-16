#pragma once
#include "Matrix3.h"
#include <vector>

struct VertexIndices
{
	int positionIndex;
	int texCoordIndex;
};

class AxisAlignedBoundingBox
{
private:
	Vec3 minVertex;
	Vec3 maxVertex;

	std::vector<Vec3> vertices;
	std::vector<VertexIndices> indices;
	std::vector<Vec3> texCoords;

	void Construct(std::vector<Vec3> mVertices);
	void FindMinMaxVertices(std::vector<Vec3> mVertices);
	void InitAABBVertices();
	void DrawSides();
	void DrawSide(std::initializer_list <int> positions);
	void InitAABBTexCoords();
public:
	AxisAlignedBoundingBox(std::vector<Vec3> mVertices);
	std::vector<Vec3> GetVertices();
	std::vector<Vec3> GetTexCoords();
	std::vector<VertexIndices> GetIndices();
};