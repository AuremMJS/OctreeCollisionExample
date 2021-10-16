#version 450

// Input values at a vertex
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inTexCoord;
layout(location = 3) in vec3 normal;

// Instance Position
layout(location = 4) in vec3 instanceOffset;
layout(location = 5) in vec3 rotation;
layout(location = 6) in float scale;

// Output values to fragment shader
layout(location = 0) out vec2 fragTexCoord;

void main() 
{
	// Set position and tex coord
	gl_Position = vec4(inPosition,  1.0);
	fragTexCoord = vec2(inTexCoord.x,inTexCoord.y);
}