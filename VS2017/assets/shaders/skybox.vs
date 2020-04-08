#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vertexUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 worldMatrix;

void main()
{
	vertexUV = aPos;
	vec4 pos = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}