#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include <iostream>

Texture::Texture(std::string file, unsigned int _type)
{
	//type = _type;

	unsigned char* data = stbi_load(file.c_str(), &width, &height, &colorChannels, 0);

	if (!data)
	{
		std::cout << "Failed to load texture @ " + file << std::endl;
		exit(-1);
	}

	init(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind()
{
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::init(unsigned char* data)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

//GLuint Texture::LoadSkybox(std::vector<const GLchar*> faces) 
//{
//	//GLuint textureID;
//	glGenTextures(1, &textureID);
//
//	//int width, height;
//	//unsigned char *image;
//
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	for (GLuint i = 0; i < faces.size(); i++)
//	{
//		// with soil: (to remove, just backup, in case conversions to stb throws error): image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
//		image = stbi_load(faces[i], &width, &height, 0, STBI_rgb);
//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//
//		// with soil: (to remove, just backup, in case conversions to stb throws error): SOIL_free_image_data(image);
//		stbi_image_free(image);
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//
//	return textureID;
//}
