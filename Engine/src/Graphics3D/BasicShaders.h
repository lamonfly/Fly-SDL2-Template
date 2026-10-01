#pragma once

// Blinn-Phong, one directional light
namespace BasicShaders
{
	inline constexpr const char* Vertex = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat3 uNormalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;

void main()
{
	vec4 world = uModel * vec4(aPosition, 1.0);
	vWorldPos = world.xyz;
	vNormal = uNormalMatrix * aNormal;
	vUV = aUV;
	gl_Position = uProj * uView * world;
}
)GLSL";

	inline constexpr const char* Fragment = R"GLSL(
#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uAmbient;
uniform vec3 uViewPos;
uniform vec4 uBaseColor;
uniform sampler2D uBaseColorTex;

out vec4 FragColor;

void main()
{
	vec3 n = normalize(vNormal);
	vec3 l = normalize(-uLightDir);
	vec3 v = normalize(uViewPos - vWorldPos);
	vec3 h = normalize(l + v);

	float diffuse = max(dot(n, l), 0.0);
	float specular = pow(max(dot(n, h), 0.0), 32.0) * 0.25;

	vec4 base = texture(uBaseColorTex, vUV) * uBaseColor;
	vec3 color = base.rgb * (uAmbient + uLightColor * diffuse) + uLightColor * specular;
	FragColor = vec4(color, base.a);
}
)GLSL";
}
