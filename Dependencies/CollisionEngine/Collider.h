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
	Vec3 position;
	std::vector<Vec3> vertices;
	AxisAlignedBoundingBox* axisAlignedBoundingBox;
	
public:
	Collider(std::string objectName, std::vector<Vec3> mVertices, int octreeDepth = 4);
	AxisAlignedBoundingBox* GetAABB();
	bool CheckCollision(Collider *otherCollider);
	void Translate(Vec3 translateVec);
	std::vector<std::string> *CollidedObjects;
	bool IsCollidedWithAny();
};