#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H
#include <glad/glad.h>
#include <string>

void GenTexImage2D(const char *name);
GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

#endif // !IMAGEUTIL_H
