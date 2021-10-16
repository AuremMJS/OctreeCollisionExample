#pragma once
#include <vector>
#include "Matrix3.h"
#include "AxisAlignedBoundingBox.h"

class ObjParser
{
public:
	ObjParser();
	std::vector<Vec3> ParseObjFile(const char* filename);
	void SaveAABBObjFile(AxisAlignedBoundingBox aabb);
};

class Collider
{
private:
	std::vector<Vec3> vertices;
	AxisAlignedBoundingBox* axisAlignedBoundingBox;

public:
	Collider(std::vector<Vec3> mVertices);
	AxisAlignedBoundingBox* GetAABB();
};