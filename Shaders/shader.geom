#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;

// Output values to fragment shader
layout(location = 0) in vec3 geomColor[];
layout(location = 1) in vec2 geomTexCoord[];
layout(location = 2) in vec4 fragLightVector[];
layout(location = 3) in vec4 fragEyeVector[];
layout(location = 4) in vec4 fragSpecularLighting[];
layout(location = 5) in vec4 fragDiffuseLighting[];
layout(location = 6) in vec4 fragAmbientLighting[];
layout(location = 7) in float fragSpecularCoefficient[];
layout(location = 8) in vec4 fragNormal[];
layout(location = 9) in vec4 stagesInfo[];
layout(location = 10) in float fragSpecularIntensity[];
layout(location = 11) in float fragDiffuseIntensity[];
layout(location = 12) in float fragAmbientIntensity[];
layout(location = 13) in float transparency[];
layout(location = 14) in float useNormalMap[];
layout(location = 15) in float useOpacityMap[];

//layout(location = 0) in vec3 geomColor[];

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

bool isNormalsEqual(vec3 n0, vec3 n1)
{
	n0 = normalize(n0);
	n1 = normalize(n1);
	return all(equal(n0,n1));
}

vec3 findTriangleNormal(vec3 v0, vec3 v1, vec3 v2)
{
	vec3 e0 = v0 - v1;
	vec3 e1 = v0 - v2;
	return cross(e0,e1);
}

void main()
{
	// Get the vertices
	vec3 P0 = gl_in[0].gl_Position.xyz;
	vec3 P1 = gl_in[1].gl_Position.xyz;
	vec3 P2 = gl_in[2].gl_Position.xyz;
	vec3 P3 = gl_in[3].gl_Position.xyz;
	vec3 P4 = gl_in[4].gl_Position.xyz;
	vec3 P5 = gl_in[5].gl_Position.xyz;
	
	vec3 N1 = findTriangleNormal(P0, P1, P2);
	vec3 N2 = findTriangleNormal(P0, P2, P4);
	vec3 N3 = findTriangleNormal(P2, P3, P4);
	vec3 N4 = findTriangleNormal(P0, P4, P5);
	
	if(isNormalsEqual(N1,N2))
	{
		gl_Position = vec4(P0,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P1,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P2,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P4,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
	}
	else if(isNormalsEqual(N2,N3))
	{
		gl_Position = vec4(P0,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P2,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P3,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P4,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
	}
	else if(isNormalsEqual(N2,N4))
	{
		gl_Position = vec4(P0,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P2,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P4,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
		
		gl_Position = vec4(P5,1);
		fragColor = vec3(1,1,1);
		EmitVertex();
	}
	
	EndPrimitive();
}