#include "gamelevel.h"
#include <fstream>
#include <sstream>
#include "res_name.h"

void CGameLevel::Load(const char * pFilePath, int levelW, int levelH)
{
	m_Bricks.clear();
	std::string line;
	std::ifstream fstream(pFilePath);
	int iCode;
	int x = 0, y = 0;
	int iRow, iCol;
	if (fstream) {
		std::getline(fstream, line);
		std::istringstream sstream0(line);
		sstream0 >> iRow >> iCol;
		float unit_w = levelW / float(iCol), unit_h = levelH / float(iRow);
		glm::vec2 size(unit_w, unit_h);
		while (std::getline(fstream, line)) {
			std::istringstream sstream(line);
			while (sstream >> iCode) {
				if (1 == iCode) {
					glm::vec2 pos(unit_w * x, unit_h * y);
					CGameObject obj(pos, size, CResManager::GetTexture2D(ResName::TEXTURE2D_BLOCK_SOLID), glm::vec3(0.8f, 0.8f, 0.7f));
					obj.m_IsSolid = true;
					m_Bricks.push_back(obj);
				}
				else if (iCode > 1) {
					glm::vec3 color = glm::vec3(1.0f);
					if (iCode == 2)
						color = glm::vec3(0.2f, 0.6f, 1.0f);
					else if (iCode == 3)
						color = glm::vec3(0.0f, 0.7f, 0.0f);
					else if (iCode == 4)
						color = glm::vec3(0.8f, 0.8f, 0.4f);
					else if (iCode == 5)
						color = glm::vec3(1.0f, 0.5f, 0.0f);

					glm::vec2 pos(unit_w * x, unit_h * y);
					CGameObject obj(pos, size, CResManager::GetTexture2D(ResName::TEXTURE2D_BLOCK), color);
					m_Bricks.push_back(obj);
				}
				++x;
			}
			++y;
			x = 0;
		}
	}
}

void CGameLevel::Draw(CSprite & sprite)
{
	for (CGameObject &brick : m_Bricks) {
		if (!brick.m_IsDestroyed) {
			brick.Draw(sprite);
		}
	}
}

bool CGameLevel::IsCompleted()
{
	for (CGameObject &brick : m_Bricks)
		if (!brick.m_IsSolid && !brick.m_IsDestroyed)
			return false;
	return true;
}
