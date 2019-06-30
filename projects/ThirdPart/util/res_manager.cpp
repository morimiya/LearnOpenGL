#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "res_manager.h"


std::map<std::string, CShader> CResManager::s_Shaders;
std::map<std::string, CTexture2D> CResManager::s_Texture2Ds;


CShader CResManager::LoadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const std::string &name)
{
	s_Shaders[name] = CShader(vertexPath, fragmentPath, geometryPath);
	return s_Shaders[name];
}

CShader CResManager::GetShader(const std::string &name)
{
	return s_Shaders[name];
}

CTexture2D CResManager::LoadTexture2D(const char* filePath, const std::string &name, bool gamma)
{
	s_Texture2Ds[name] = loadTexture2DFromFile(filePath, gamma);
	return s_Texture2Ds[name];
}

CTexture2D CResManager::GetTexture2D(const std::string &name)
{
	return s_Texture2Ds[name];
}

void CResManager::Clear()
{
	for (auto iter : s_Shaders) {
		glDeleteProgram(iter.second.ID);
	}
	s_Shaders.clear();
	for (auto iter : s_Texture2Ds) {
		glDeleteTextures(1, &iter.second.ID);
	}
	s_Texture2Ds.clear();
}

CTexture2D CResManager::loadTexture2DFromFile(const char* file, bool gamma)
{
	CTexture2D texture;

	int width, height, nrChannels;
	unsigned char *tmpdata = stbi_load(file, &width, &height, &nrChannels, 0);
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

		texture.m_Render_Format = internalFormat;
		texture.m_Image_Format = dataFormat;

		texture.Generate(width, height, data);

		delete[] data;
	}
	else {
		std::cout << "Texture failed to load at path: " << file << std::endl;
		stbi_image_free(tmpdata);
	}

	return texture;
}