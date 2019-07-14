#include "gameobject.h"

CGameObject::CGameObject()
{
	m_Position = glm::vec2(0.0f, 0.0f);
	m_Size = glm::vec2(1.0f, 1.0f);
	m_Velocity = glm::vec2(0.0f, 0.0f);
	m_Color = glm::vec3(1.0f);

	m_Rotation = 0.0f;
	m_IsSolid = false;
	m_IsDestroyed = false;
}

CGameObject::CGameObject(glm::vec2 pos, glm::vec2 size, CTexture2D spriteTexture, glm::vec3 color, glm::vec2 velocity)
{
	m_Position = pos;
	m_Size = size;
	m_Velocity = velocity;
	m_Color = color;
	m_SpriteTexture2D = spriteTexture;

	m_Rotation = 0.0f;
	m_IsSolid = false;
	m_IsDestroyed = false;
}

void CGameObject::Draw(CSprite & sprite)
{
	sprite.DrawSprite(m_SpriteTexture2D, m_Position, m_Size, m_Rotation, m_Color);
}

CBallObject::CBallObject()
	: CGameObject()
{
	m_Radius = 12.5f;
	m_IsStuck = true;
	m_IsSticky = false;
	m_IsPassThrough = false;
}

CBallObject::CBallObject(glm::vec2 pos, float radius, glm::vec2 velocity, CTexture2D sprite)
	: CGameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity)
{
	m_Radius = radius;
	m_IsStuck = true;
	m_IsSticky = false;
	m_IsPassThrough = false;
}

glm::vec2 CBallObject::Move(float dt, int wndW)
{
	if (!m_IsStuck) {
		m_Position += m_Velocity * dt;
		if (m_Position.x < 0.0f) {
			m_Velocity.x = -m_Velocity.x;
			m_Position.x = 0.0f;
		}
		else if (m_Position.x + m_Size.x >= wndW) {
			m_Velocity.x = -m_Velocity.x;
			m_Position.x = wndW - m_Size.x;
		}
		if (m_Position.y <= 0.0f) {
			m_Velocity.y = -m_Velocity.y;
			m_Position.y = 0.0f;
		}
	}
	return m_Position;
}

void CBallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	m_Position = position;
	m_Velocity = velocity;
	m_IsStuck = true;
	m_IsSticky = false;
	m_IsPassThrough = false;
}
