#version 450
#extension GL_ARB_separate_shader_objects : enable

// Texture sampler uniform
layout(binding = 2) uniform sampler2D texSampler;

// Uniform for Iridescent colours
layout(binding = 6) uniform IridescentColors {
    vec4 colors[90];
} iridescentColors;

// Input values to the fragment
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in float isCollided;
layout(location = 4) in float useOpacityMap;
layout(location = 5) in float showAABB;

// Output color of the fragment
layout(location = 0) out vec4 outColor;

void main() {
	
	// Calculate the opacity from opacity map
	vec4 opacityColor = texture(texSampler, fragTexCoord);
	float opacity = opacityColor.x;

	if(useOpacityMap > 0.5f)
	{
		if(showAABB > 0.5f)
			outColor = vec4(0,0,0,opacity);
		else
			outColor = vec4(0);
	}
	else if(isCollided > 0.5f)
		outColor = vec4(1,0,0,1);
	else
		outColor = vec4(1,1,1,1);
}