#pragma once
#include <map>
#include <memory>
#include "Collider.h"

class CollisionEngine
{
private:
	static CollisionEngine* instance;
	bool isActive;
	std::map<std::string,Collider*> colliders;
public:
	static CollisionEngine* GetInstance();
	CollisionEngine();
	void SetActive(bool isActive);
	void CollisionLoop();
	void AddCollider(std::string objectName, Collider* collider);
	void RemoveCollider(std::string objectName);
	~CollisionEngine();
};