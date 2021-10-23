#include "Matrix3.h"
#include <vector>
#include <array>

#ifndef VERTEX
#define VERTEX

// Vertex
struct Vertex
{
	Vec3 position;
	Vec3 color;
	Vec3 tex;
	Vec3 normal;
};

#endif

#ifndef INSTANCE_DATA
#define INSTANCE_DATA

// Instance Data
struct InstanceData
{
	Vec3 instancePosition;
	Vec3 rotation;
	float scale;
};

#endif // !INSTANCEDATA