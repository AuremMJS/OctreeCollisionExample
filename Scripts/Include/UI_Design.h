#pragma once
#include "Matrix3.h"
#include "Constants.h"

// Values of for UI elements
struct UIParameters
{
	float scale;
	bool isCollisionEnabled;
	bool renderAABB;
};

class UIDesign
{
public:
	static UIParameters uiParams;
	static void DrawUI();
};
