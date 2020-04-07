#version 330 core
out vec4 FragColor;

in vec3 TextCoords;

uniform samplerCube skybox;

void main()
{
	fragColor = texture(skybox, TexCoords);
}