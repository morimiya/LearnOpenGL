#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <util/texture.h>
#include <util/shader.h>
#include "sprite.h"

class CPostProcessor
{
public:
	// State
	CShader m_Shader;
	CTexture2D m_Texture2D;
	int m_Width, m_Height;
	// Options
	bool m_Confuse, m_Chaos, m_Shake;
	// Constructor
	CPostProcessor(CShader shader, int width, int height);
	// Prepares the postprocessor's framebuffer operations before rendering the game
	void BeginRender();
	// Should be called after rendering the game, so it stores all the rendered data into a texture object
	void EndRender();
	// Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
	void Render(GLfloat time);
private:
	// Render state
	GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
	GLuint RBO; // RBO is used for multisampled color buffer
	GLuint VAO;
	// Initialize quad for rendering postprocessing texture
	void initRenderData();
};

#endif // !POST_PROCESSOR_H
