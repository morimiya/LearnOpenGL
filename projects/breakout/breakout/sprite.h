#ifndef SPRITE_H
#define SPRITE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <util/texture.h>
#include <util/shader.h>


class CSprite
{
public:
	CSprite(CShader &shader);
	~CSprite();

	void DrawSprite(CTexture2D &texture, glm::vec2 position, glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
	CShader m_Shader;
	GLuint m_QuadVAO;
	void InitRenderData();
};

#endif // SPRITE_H
