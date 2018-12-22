#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H
#include <glad/glad.h>
#include <string>
#include <vector>

void GenTexImage2D(const char *name);
GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
GLuint loadCubemap(std::vector<std::string> faces);

#endif // !IMAGEUTIL_H
