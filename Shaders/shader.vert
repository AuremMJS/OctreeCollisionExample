#version 450
#extension GL_ARB_separate_shader_objects : enable

// Uniform for Model, View and Projection matrices
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// Uniform for Lighting Properties
layout(binding = 1) uniform LightingConstants {
	float isCollided;
	float useOpacityMap;
	float showAABB;
} lighting;

// Input values at a vertex
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inTexCoord;
layout(location = 3) in vec3 normal;

// Output values to fragment shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragNormal;
layout(location = 3) out float isCollided;
layout(location = 4) out float useOpacityMap;
layout(location = 5) out float showAABB;

// Main function
void main() {

	// Calculate vertex position
	vec4 VCS_position =  ubo.view * ubo.model * vec4((inPosition), 
	1.0);
    gl_Position = ubo.proj *VCS_position;

	// Pass out color
    fragColor = inColor;

	// Pass Texture Coordinates
    fragTexCoord = vec2(inTexCoord.x,inTexCoord.y);

	// Calculate and pass normal
	fragNormal = ubo.view * ubo.model * vec4(normal,0.0);

	// Pass bools specifying whether to use normal and opacity map
	isCollided = lighting.isCollided;
	useOpacityMap = lighting.useOpacityMap;
	showAABB = lighting.showAABB;
}