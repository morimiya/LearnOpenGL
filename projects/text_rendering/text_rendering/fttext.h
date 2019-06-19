#ifndef FTTEXT_H
#define FTTEXT_H

#include <map>

#include <glad/glad.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <util/shader.h>

namespace FT_Text
{
	class CFTText;
	extern CFTText *s_pFTText;// = nullptr;

	struct CCharacter {
		GLuint m_TextureID;
		glm::ivec2 m_Size;
		glm::ivec2 m_Bearing;
		GLuint m_Advance;
	};

	class CFTText
	{
	public:
		static CFTText* GetInstance() {
			if (nullptr == s_pFTText) {
				s_pFTText = new CFTText();
			}
			return s_pFTText;
		}

		static void Uninstall() {
			if (s_pFTText) {
				s_pFTText->Destory();
			}
		}

		static void FrameBufferSizeChange(int iWidth, int iHeight);

		CFTText();
		
		void Destory();
		void OnFrameBufferSizeChange(int iWidth, int iHeight);
		void RenderText(const WCHAR *text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	private:
		std::map<WCHAR, CCharacter> m_Characters;
		GLuint VAO, VBO;
		CShader m_Shader;
		int m_ScrWidth, m_ScrHeight;

		FT_Library m_Lib;
		FT_Face m_Face;
	};
}

#endif