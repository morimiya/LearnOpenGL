#ifndef RES_MANAGER_H
#define RES_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "shader.h"
#include "texture.h"

class CResManager
{
public:
	static std::map<std::string, CShader> s_Shaders;
	static std::map<std::string, CTexture2D> s_Texture2Ds;

	static CShader LoadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const std::string &name);
	static CShader GetShader(const std::string &name);

	static CTexture2D LoadTexture2D(const char* filePath, const std::string &name, bool gamma=false);
	static CTexture2D GetTexture2D(const std::string &name);

	static void Clear();

private:
	CResManager() {};
	static CTexture2D loadTexture2DFromFile(const char* file, bool gamma);
};

#endif //RES_MANAGER_H
