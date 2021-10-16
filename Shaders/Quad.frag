#version 450

// Texture sampler uniform
layout(binding = 0) uniform sampler2D shadowSampler;

// Input texture coord
layout(location = 0) in vec2 fragTexCoord;

// Output color of the fragment
layout(location = 0) out vec4 outColor;


// Main function
void main() 
{
	// Set the color
	outColor =texture(shadowSampler, fragTexCoord);
}