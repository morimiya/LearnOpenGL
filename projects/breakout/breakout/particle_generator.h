#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <util/shader.h>
#include "gameobject.h"
#include <util/texture.h>


struct SParticle {
	glm::vec2 m_Position, m_Velocity;
	glm::vec4 m_Color;
	float m_Life;

	SParticle() {
		m_Position = glm::vec2(0.0f);
		m_Velocity = glm::vec2(0.0f);
		m_Color = glm::vec4(1.0f);
		m_Life = 0.0f;
	}
};

class CParticleGenerator
{
public:
	// Constructor
	CParticleGenerator(CShader shader, CTexture2D texture, int amount);
	// Update all particles
	void Update(float dt, CGameObject &object, int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	// Render all particles
	void Draw();
private:
	// State
	std::vector<SParticle> particles;
	int m_Amount;
	// Render state
	CShader m_Shader;
	CTexture2D m_Texture2D;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	int firstUnusedParticle();
	// Respawns particle
	void respawnParticle(SParticle &particle, CGameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif //PARTICLE_GENERATOR_H