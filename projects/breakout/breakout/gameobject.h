#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <util/texture.h>
#include "sprite.h"

const glm::vec2 POWERUP_SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class CGameObject
{
public:
	glm::vec2 m_Position, m_Size, m_Velocity;
	glm::vec3 m_Color;
	GLfloat m_Rotation;
	bool m_IsSolid;
	bool m_IsDestroyed;

	CTexture2D m_SpriteTexture2D;

	CGameObject();
	CGameObject(glm::vec2 pos, glm::vec2 size, CTexture2D spriteTexture, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	virtual void Draw(CSprite &sprite);
};


class CBallObject : public CGameObject
{
public:
	float m_Radius;
	bool m_IsStuck;
	bool m_IsSticky;
	bool m_IsPassThrough;

	CBallObject();
	CBallObject(glm::vec2 pos, float radius, glm::vec2 velocity, CTexture2D sprite);

	glm::vec2 Move(float dt, int wndW);
	void Reset(glm::vec2 position, glm::vec2 velocity);
};


class CPowerUp : public CGameObject
{
public:
	std::string m_sType;
	float m_Duration;
	bool m_IsActivated;

	CPowerUp(std::string sType, glm::vec3 color, float duration, glm::vec2 pos, CTexture2D texture)
		: CGameObject(pos, POWERUP_SIZE, texture, color, VELOCITY),
		m_sType(sType), m_Duration(duration), m_IsActivated(false) {

	}
};

#endif //GAMEOBJECT_H
