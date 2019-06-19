#include <iostream>

#include "wnd.h"
#include "fttext.h"

namespace FT_Text
{
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec4 vertex;\n"	// <vec2 pos, vec2 tex>
		"out vec2 TexCoords;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
		"    TexCoords = vertex.zw;\n"
		"}";

	const char *fragmentShaderSource = "#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"void main() {\n"
		"    color = vec4(textColor, texture(text, TexCoords).r);\n"
		"}";

	CFTText *s_pFTText = nullptr;
	static const char *FONT_PATH = "msyh.ttc";

	CFTText::CFTText()
	{
		m_ScrWidth = 0;
		m_ScrHeight = 0;
		m_Shader.compileShaderCode(vertexShaderSource, fragmentShaderSource);
		OnFrameBufferSizeChange(WND::SCR_WIDTH, WND::SCR_HEIGHT);

		if (FT_Init_FreeType(&m_Lib))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		if (FT_New_Face(m_Lib, FONT_PATH, 0, &m_Face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

		FT_Set_Pixel_Sizes(m_Face, 0, 48);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void CFTText::Destory()
	{
		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_Lib);
	}

	void CFTText::FrameBufferSizeChange(int iWidth, int iHeight)
	{
		if (s_pFTText) {
			s_pFTText->OnFrameBufferSizeChange(iWidth, iHeight);
		}
	}

	void CFTText::OnFrameBufferSizeChange(int iWidth, int iHeight)
	{
		if (m_ScrWidth != iWidth || m_ScrHeight != iHeight) {
			m_ScrWidth = iWidth;
			m_ScrHeight = iHeight;
			glm::mat4 projection = glm::ortho(0.0f, GLfloat(m_ScrWidth), 0.0f, GLfloat(m_ScrHeight));
			m_Shader.use();
			m_Shader.setMatrix4fv("projection", projection);
		}
	}

	void CFTText::RenderText(const WCHAR *text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
	{
		m_Shader.use();
		m_Shader.setVec3("textColor", color);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (const WCHAR *c = text; *c; ++c) {
			if (m_Characters.find(*c) == m_Characters.end()) {
				// Load character glyph 
				if (FT_Load_Char(m_Face, *c, FT_LOAD_RENDER)) {
					std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
					continue;
				}
				// Generate texture
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					m_Face->glyph->bitmap.width,
					m_Face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					m_Face->glyph->bitmap.buffer
				);
				// Set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// Now store character for later use
				CCharacter character = {
					texture,
					glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
					glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
					m_Face->glyph->advance.x
				};
				m_Characters.insert(std::pair<WCHAR, CCharacter>(*c, character));
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			
			CCharacter ch = m_Characters[*c];

			GLfloat xpos = x + ch.m_Bearing.x * scale;
			GLfloat ypos = y - (ch.m_Size.y - ch.m_Bearing.y) * scale;

			GLfloat w = ch.m_Size.x * scale;
			GLfloat h = ch.m_Size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.m_TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.m_Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}