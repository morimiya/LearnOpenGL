#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class CTexture2D
{
public:
	GLuint ID;
	GLuint m_Width, m_Height;
	GLuint m_Render_Format;
	GLuint m_Image_Format;

	GLuint m_Wrap_S, m_Wrap_T;
	GLuint m_Filter_Min, m_Filter_Max;

	CTexture2D();

	void Generate(GLuint width, GLuint height, unsigned char* data);
	void Bind() const;
};

#endif //TEXTURE_H
