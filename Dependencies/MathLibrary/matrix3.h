#pragma once
#include <cmath>

// Vec3
class Vec3
{
public:
	float x, y, z;

	Vec3()
	{
		x = y = z = 0;
	}
	Vec3(float mX, float mY, float mZ)
	{
		x = mX;
		y = mY;
		z = mZ;
	}

	double operator*(Vec3 *other)
	{
		return x * other->x + y * other->y + z * other->z;
	}
	
	Vec3 operator *(double scalar)
	{
		Vec3 result;
		result.x = scalar * x;
		result.y = scalar * y;
		result.z = scalar * z;
		return result;
	}

	Vec3 cross(Vec3 *other)
	{
		Vec3 result;
		result.x = y * other->z - z * other->y;
		result.y = z * other->x - x * other->z;
		result.z = x * other->y - y * other->x;
		return result;
	}

	double Angle(Vec3 *other)
	{
		float dotProduct = (*this * other) / (Magnitude() * other->Magnitude());
		float det = this->x * other->y - this->y * other->x;
		double angle = atan2(det,dotProduct);// acos(dotProduct);
		angle = round(angle * 1000.0) / 1000.0;
		angle *= (180.0 / 3.14);
		return angle;
	}

	double Magnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vec3 operator -(Vec3 other)
	{
		return { x - other.x,y - other.y,z - other.z };
	}

	void Normalize()
	{
		double mag = Magnitude();
		x /= mag;
		y /= mag;
		z /= mag;
	}
};

// Vec4
struct Vec4
{
	float x, y, z, w;
};
class Matrix3
{

public:
	double values[3][3];
	Matrix3();
	~Matrix3();
	Matrix3 Inverse();
	Vec3 operator *(Vec3 vector);
};

class Matrix4
{

public:
	double values[4][4];
	Matrix4();
	Matrix4(float values[16]);
	~Matrix4();
	Vec4 operator *(Vec4 vector);
	Matrix4 operator *(Matrix4 matrix);
	void InverseZAxis();
	static Matrix4 perspective(float fovy, float aspect, float zNear, float zFar);
	static Matrix4 lookAt(Vec3 eye, Vec3 center, Vec3 up);
	static Matrix4 scale(Vec3 scaleVec);
	static Matrix4 translate(Vec3 translateVec);
};

