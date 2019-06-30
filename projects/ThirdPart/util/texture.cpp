#include <iostream>
#include "texture.h"

CTexture2D::CTexture2D()
	: m_Width(0),
	m_Height(0),
	m_Render_Format(GL_RGB),
	m_Image_Format(GL_RGB),
	m_Wrap_S(GL_REPEAT),
	m_Wrap_T(GL_REPEAT),
	m_Filter_Min(GL_LINEAR),
	m_Filter_Max(GL_LINEAR)
{
	glGenTextures(1, &ID);
}

void CTexture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
	m_Width = width;
	m_Height = height;
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, m_Render_Format, width, height, 0, m_Image_Format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Filter_Max);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CTexture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}