#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gameobject.h"
#include "sprite.h"
#include <util/res_manager.h>


class CGameLevel
{
public:
	std::vector<CGameObject> m_Bricks;
	CGameLevel() {}
	
	void Load(const char* pFilePath, int levelW, int levelH);
	void Draw(CSprite &sprite);
	bool IsCompleted();
};

#endif //GAMELEVEL_H