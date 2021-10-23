#include "UI_Design.h"
#include "ImGuiHelper.h"
#include "ImGUI\imgui.h"

// Value of GUI elements
UIParameters UIDesign::uiParams = {
0.75,			   // scale
true,			   // scale
false			   // scale
};

// Function to draw the GUI elements
void UIDesign::DrawUI()
{

	ImGui::SliderFloat("Scale", &uiParams.scale, 0.1, 50.0);
	ImGui::Checkbox("Collision Enabled", &uiParams.isCollisionEnabled);
	ImGui::Checkbox("Show Octree/AABB", &uiParams.renderAABB);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}