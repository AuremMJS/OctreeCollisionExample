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
    vec4 lightPosition;
	vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	float ambientIntensity;
	float specularIntensity;
	float diffuseIntensity;
	float lightSpecularExponent;
	float ambientEnabled;
	float specularEnabled;
	float diffuseEnabled;
	float textureEnabled;
	float transparency;
	float useNormalMap;
	float useOpacityMap;
} lighting;

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
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragLightVector;
layout(location = 3) out vec4 fragEyeVector;
layout(location = 4) out vec4 fragSpecularLighting;
layout(location = 5) out vec4 fragDiffuseLighting;
layout(location = 6) out vec4 fragAmbientLighting;
layout(location = 7) out float fragSpecularCoefficient;
layout(location = 8) out vec4 fragNormal;
layout(location = 9) out vec4 stagesInfo;
layout(location = 10) out float fragSpecularIntensity;
layout(location = 11) out float fragDiffuseIntensity;
layout(location = 12) out float fragAmbientIntensity;
layout(location = 13) out float transparency;
layout(location = 14) out float useNormalMap;
layout(location = 15) out float useOpacityMap;

// Main function
void main() {
	
	// Calculate the rotation of the instance
	mat4 gRotMat;
	float s = sin(rotation.x);
	float c = cos(rotation.x);
	gRotMat[0] = vec4(c, 0.0, s, 0.0);
	gRotMat[1] = vec4(0.0, 1.0, 0.0, 0.0);
	gRotMat[2] = vec4(-s, 0.0, c, 0.0);
	gRotMat[3] = vec4(0.0, 0.0, 0.0, 1.0);	

	// Calculate vertex position
	vec4 VCS_position =  ubo.view * ubo.model * gRotMat * vec4((inPosition + instanceOffset), 
	1.0);
    gl_Position = ubo.proj *VCS_position;

	// Pass out color
    fragColor = inColor;

	// Pass Texture Coordinates
    fragTexCoord = vec2(inTexCoord.x,inTexCoord.y);

	// Calculate and pass normal
	fragNormal = ubo.view * ubo.model * vec4(normal,0.0);

	// Calculate vector from light positin to current vertex
	fragLightVector = ubo.view * lighting.lightPosition - VCS_position;

	// Pass specular and ambient values
	fragAmbientLighting = lighting.lightAmbient;
	fragSpecularCoefficient = lighting.lightSpecularExponent;
	fragSpecularIntensity = lighting.specularIntensity;
	fragAmbientIntensity = lighting.ambientIntensity;

	// Pass transparency
	transparency = lighting.transparency;

	// Pass bools specifying whether to use normal and opacity map
	useNormalMap = lighting.useNormalMap;
	useOpacityMap = lighting.useOpacityMap;

	// Pass stages info specifying which stages are enabled
	stagesInfo = vec4(lighting.ambientEnabled, lighting.diffuseEnabled, lighting.specularEnabled,lighting.textureEnabled);
}