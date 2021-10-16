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
layout(location = 2) in vec4 fragLightVector;
layout(location = 3) in vec4 fragEyeVector;
layout(location = 4) in vec3 fragSpecularLighting;
layout(location = 5) in vec3 fragDiffuseLighting;
layout(location = 6) in vec3 fragAmbientLighting;
layout(location = 7) in float fragSpecularCoefficient;
layout(location = 8) in vec3 fragNormal;
layout(location = 9) in vec4 stagesInfo;
layout(location = 10) in float fragSpecularIntensity;
layout(location = 11) in float fragDiffuseIntensity;
layout(location = 12) in float fragAmbientIntensity;
layout(location = 13) in float transparency;
layout(location = 14) in float useNormalMap;
layout(location = 15) in float useOpacityMap;

// Output color of the fragment
layout(location = 0) out vec4 outColor;

void main() {
	
	// Calculate the opacity from opacity map
	vec4 opacityColor = texture(texSampler, fragTexCoord);
	float opacity = opacityColor.x;

	if(useOpacityMap > 0.5f)
		outColor = vec4(0,0,0,opacity);
	else
		outColor = vec4(1,1,1,1);
}