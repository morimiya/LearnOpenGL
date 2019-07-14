#include "particle_generator.h"

CParticleGenerator::CParticleGenerator(CShader shader, CTexture2D texture, int amount)
{
	m_Shader = shader;
	m_Texture2D = texture;
	m_Amount = amount;
	init();
}

void CParticleGenerator::Update(float dt, CGameObject & object, int newParticles, glm::vec2 offset)
{
	// Add new particles 
	for (int i = 0; i < newParticles; ++i) {
		int unusedParticle = this->firstUnusedParticle();
		if (unusedParticle != -1)
			this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	// Update all particles
	for (int i = 0; i < this->m_Amount; ++i) {
		SParticle &p = this->particles[i];
		p.m_Life -= dt; // reduce life
		if (p.m_Life > 0.0f) {
			p.m_Position -= p.m_Velocity * dt;	// ·´·½Ïò·É
			p.m_Color.a -= dt * 2.5;
		}
	}
}

void CParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->m_Shader.use();
	for (SParticle particle : this->particles) {
		if (particle.m_Life > 0.0f) {
			this->m_Shader.setVec2("offset", particle.m_Position);
			this->m_Shader.setVec4("color", particle.m_Color);
			this->m_Texture2D.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void CParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < m_Amount; ++i)
		this->particles.push_back(SParticle());
}

int CParticleGenerator::firstUnusedParticle()
{
	static int lastUsedParticle = 0;
	// First search from last used particle, this will usually return almost instantly
	for (int i = lastUsedParticle; i < this->m_Amount; ++i) {
		if (this->particles[i].m_Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].m_Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void CParticleGenerator::respawnParticle(SParticle & particle, CGameObject & object, glm::vec2 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.m_Position = object.m_Position + random + offset;
	particle.m_Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.m_Life = 1.0f;
	particle.m_Velocity = object.m_Velocity * 0.1f;
}
