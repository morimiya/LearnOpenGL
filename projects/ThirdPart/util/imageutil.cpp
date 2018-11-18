#include "imageutil.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

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