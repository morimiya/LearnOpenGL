#include "imageutil.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <string>
#include <vector>

void GenTexImage2D(const char *name)
{
	int width, height, nrChannels, iMode;
	unsigned char *tmpdata = stbi_load(name, &width, &height, &nrChannels, 0);
	iMode = nrChannels == 4 ? GL_RGBA :GL_RGB;

	int pitch = (width * nrChannels + 3) / 4 * 4;
	unsigned char *data = new unsigned char[pitch * height * nrChannels];
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			for (int k = 0; k < nrChannels; ++k) {
				data[i * pitch + j * nrChannels + k] = tmpdata[i * width * nrChannels + j * nrChannels + k];
			}
		}
	}
	stbi_image_free(tmpdata);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, iMode, width, height, 0, iMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	delete[] data;
}


GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrChannels;
	unsigned char *tmpdata = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (tmpdata) {
		int pitch = (width * nrChannels + 3) / 4 * 4;
		unsigned char *data = new unsigned char[pitch * height * nrChannels];
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				for (int k = 0; k < nrChannels; ++k) {
					data[i * pitch + j * nrChannels + k] = tmpdata[i * width * nrChannels + j * nrChannels + k];
				}
			}
		}
		stbi_image_free(tmpdata);

		GLenum internalFormat;
		GLenum dataFormat;
		if (nrChannels == 1)
			internalFormat = dataFormat = GL_RED;
		else if (nrChannels == 3) {
			internalFormat = gamma ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4) {
			internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		delete[] data;
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(tmpdata);
	}

	return textureID;
}


GLuint loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 解决两面边缘任何一面都无法命中的情况。

	return textureID;
}