#include <algorithm>

#include "game.h"
#include <util/res_manager.h>
#include <util/shader.h>
#include "sprite.h"
#include "gameobject.h"
#include "res_name.h"
#include "particle_generator.h"
#include "post_processor.h"

bool CheckAABBCollision(CGameObject &one, CGameObject &two); // AABB - AABB collision
bool CheckBallCollision(CBallObject &ball, CGameObject &obj); // AABB - Circle
Collision CheckCollision(CBallObject &ball, CGameObject &obj); // AABB - Circle collision
Direction VectorDirection(glm::vec2 target);
void ActivatePowerUp(CPowerUp &powerUp);

CSprite *g_Sprite;
CGameObject *g_Player;
CBallObject *g_Ball;
CParticleGenerator *g_Particles;
CPostProcessor *g_Effects;
GLfloat g_ShakeTime = 0.0f;

CGame::CGame(GLuint width, GLuint height)
	: m_State(GAME_ACTIVE), m_Keys(), m_Width(width), m_Height(height)
{

}

CGame::~CGame()
{
	delete g_Sprite;
	delete g_Player;
	delete g_Ball;
	delete g_Particles;
	delete g_Effects;
}

void CGame::Init()
{
	CResManager::LoadShader("../shaders/sprite.vs", "../shaders/sprite.fs", nullptr, ResName::SHADER_SPRITE);
	CResManager::LoadShader("../shaders/particle.vs", "../shaders/particle.fs", nullptr, ResName::SHADER_PARTICLE);
	CResManager::LoadShader("../shaders/post_processor.vs", "../shaders/post_processor.fs", nullptr, ResName::SHADER_POST_PROCESSOR);

	glm::mat4 projection = glm::ortho(0.0f, (GLfloat)m_Width, (GLfloat)m_Height, 0.0f, -1.0f, 1.0f);

	CShader spriteShader = CResManager::GetShader(ResName::SHADER_SPRITE);
	spriteShader.use();
	spriteShader.setInt("image", 0);
	spriteShader.setMatrix4fv("projection", projection);

	CShader particleShader = CResManager::GetShader(ResName::SHADER_PARTICLE);
	particleShader.use();
	particleShader.setInt("image", 0);
	particleShader.setMatrix4fv("projection", projection);

	CShader post_processorShader = CResManager::GetShader(ResName::SHADER_POST_PROCESSOR);

	CResManager::LoadTexture2D("../res/ball.png", ResName::TEXTURE2D_BALL);
	CResManager::LoadTexture2D("../res/background.jpg", ResName::TEXTURE2D_BACKGROUND);
	CResManager::LoadTexture2D("../res/block.png", ResName::TEXTURE2D_BLOCK);
	CResManager::LoadTexture2D("../res/block_solid.png", ResName::TEXTURE2D_BLOCK_SOLID);
	CResManager::LoadTexture2D("../res/paddle.png", ResName::TEXTURE2D_PADDLE);
	CResManager::LoadTexture2D("../res/paddle.png", ResName::TEXTURE2D_PARTICLE);
	CResManager::LoadTexture2D("../res/powerup_speed.png", ResName::TEXTURE2D_POWERUP_SPEED);
	CResManager::LoadTexture2D("../res/powerup_sticky.png", ResName::TEXTURE2D_POWERUP_STICKY);
	CResManager::LoadTexture2D("../res/powerup_increase.png", ResName::TEXTURE2D_POWERUP_INCREASE);
	CResManager::LoadTexture2D("../res/powerup_confuse.png", ResName::TEXTURE2D_POWERUP_CONFUSE);
	CResManager::LoadTexture2D("../res/powerup_chaos.png", ResName::TEXTURE2D_POWERUP_CHAOS);
	CResManager::LoadTexture2D("../res/powerup_passthrough.png", ResName::TEXTURE2D_POWERUP_PASSTHROUGH);

	g_Sprite = new CSprite(spriteShader);
	g_Particles = new CParticleGenerator(particleShader, CResManager::GetTexture2D(ResName::TEXTURE2D_PARTICLE), 500);
	g_Effects = new CPostProcessor(post_processorShader, m_Width, m_Height);

	m_GameLevel.Load("../res/level.txt", m_Width, m_Height / 2);
	m_Level = 0;

	glm::vec2 playerPos(m_Width * 0.5 - PLAYER_SIZE.x * 0.5, m_Height - PLAYER_SIZE.y);
	CTexture2D paddleTexture = CResManager::GetTexture2D(ResName::TEXTURE2D_PADDLE);
	g_Player = new CGameObject(playerPos, PLAYER_SIZE, paddleTexture);

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	CTexture2D ballTexture = CResManager::GetTexture2D(ResName::TEXTURE2D_BALL);
	g_Ball = new CBallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ballTexture);
}

void CGame::DoCollisions()
{
	for (CGameObject &box : m_GameLevel.m_Bricks) {
		if (!box.m_IsDestroyed) {
			Collision collision = CheckCollision(*g_Ball, box);
			if (!std::get<0>(collision))
				continue;

			if (!box.m_IsSolid) {
				box.m_IsDestroyed = true;
				SpawnPowerUps(box);
			}
			else {
				g_ShakeTime = 0.05f;
				g_Effects->m_Shake = true;
			}
			if (box.m_IsSolid || (!g_Ball->m_IsPassThrough && !box.m_IsSolid)) {
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) {
					g_Ball->m_Velocity.x = -g_Ball->m_Velocity.x;
					float diff = g_Ball->m_Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						g_Ball->m_Position.x += diff;
					else
						g_Ball->m_Position.x -= diff;
				}
				else {
					g_Ball->m_Velocity.y = -g_Ball->m_Velocity.y;
					float diff = g_Ball->m_Radius - std::abs(diff_vector.y);
					if (dir == UP)
						g_Ball->m_Position.y -= diff;
					else
						g_Ball->m_Position.y += diff;
				}
			}
		}
	}

	for (CPowerUp &powerUp : m_PowerUps) {
		if (!powerUp.m_IsDestroyed) {
			if (powerUp.m_Position.y >= m_Height) {
				powerUp.m_IsDestroyed = true;
			}
			if (CheckAABBCollision(*g_Player, powerUp)) {
				ActivatePowerUp(powerUp);
				powerUp.m_IsDestroyed = true;
				powerUp.m_IsActivated = true;
			}
		}
	}

	Collision result = CheckCollision(*g_Ball, *g_Player);
	if (!g_Ball->m_IsStuck && std::get<0>(result)) {
		float centerBoard = g_Player->m_Position.x + g_Player->m_Size.x / 2;
		float dis = (g_Ball->m_Position.x + g_Ball->m_Radius) - centerBoard;
		float percentage = dis / (g_Player->m_Size.x / 2);

		glm::vec2 oldVelocity = g_Ball->m_Velocity;
		g_Ball->m_Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * 2.0f;
		g_Ball->m_Velocity = glm::normalize(g_Ball->m_Velocity) * glm::length(oldVelocity);
		g_Ball->m_Velocity.y = -1 * abs(g_Ball->m_Velocity.y);
		g_Ball->m_IsStuck = g_Ball->m_IsSticky;
	}
}

void CGame::ResetLevel()
{
	m_GameLevel.Load("../res/level.txt", m_Width, m_Height / 2);
	m_Level = 0;
}

void CGame::ResetPlayer()
{
	g_Player->m_Size = PLAYER_SIZE;
	g_Player->m_Position = glm::vec2(m_Width / 2 - PLAYER_SIZE.x / 2, m_Height - PLAYER_SIZE.y);
	g_Ball->Reset(g_Player->m_Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
	g_Effects->m_Chaos = false;
	g_Effects->m_Confuse = false;
	g_Player->m_Color = glm::vec3(1.0f);
	g_Ball->m_Color = glm::vec3(1.0f);
}

bool ShouldSpawn(int chance)
{
	int random = rand() % chance;
	return random == 0;
}

void CGame::SpawnPowerUps(CGameObject & block)
{
	if (ShouldSpawn(75)) { // 1/75的几率
		CTexture2D tex_speed = CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_SPEED);
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_SPEED, glm::vec3(0.5f, 0.5f, 1.0f), 10.0f, block.m_Position, tex_speed
			));
	}
	if (ShouldSpawn(75))
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_STICKY, glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.m_Position, CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_STICKY)
			));
	if (ShouldSpawn(75))
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_PASS_THROUGH, glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.m_Position, CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_PASSTHROUGH)
			));
	if (ShouldSpawn(75))
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_PAD_SIZE_INCREASE, glm::vec3(1.0f, 0.6f, 0.4), 10.0f, block.m_Position, CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_INCREASE)
			));
	if (ShouldSpawn(15)) // 负面道具被更频繁地生成
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_CONFUSE, glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.m_Position, CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_CONFUSE)
			));
	if (ShouldSpawn(15))
		this->m_PowerUps.push_back(
			CPowerUp(ResName::POWERUP_CHAOS, glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.m_Position, CResManager::GetTexture2D(ResName::TEXTURE2D_POWERUP_CHAOS)
			));
}

bool IsOtherPowerUpActive(std::vector<CPowerUp> &powerUps, std::string type)
{
	for (const CPowerUp &powerUp : powerUps)
	{
		if (powerUp.m_IsActivated)
			if (powerUp.m_sType == type)
				return true;
	}
	return false;
}

void CGame::UpdatePowerUps(float dt)
{
	for (CPowerUp &powerUp : m_PowerUps) {
		powerUp.m_Position += powerUp.m_Velocity * dt;
		if (powerUp.m_IsActivated) {
			powerUp.m_Duration -= dt;
			if (powerUp.m_Duration <= 0.0f) {
				// 之后会将这个道具移除
				powerUp.m_IsActivated = false;
				// 停用效果
				if (powerUp.m_sType == ResName::POWERUP_SPEED) {
					// if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_SPEED)) {
					//  g_Ball->m_Velocity = INITIAL_BALL_VELOCITY;
					// }
				}
				else if (powerUp.m_sType == ResName::POWERUP_STICKY)
				{
					if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_STICKY))
					{   // 仅当没有其他sticky效果处于激活状态时重置，以下同理
						g_Ball->m_IsSticky = false;
						g_Player->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.m_sType == ResName::POWERUP_PASS_THROUGH)
				{
					if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_PASS_THROUGH))
					{
						g_Ball->m_IsPassThrough = false;
						g_Ball->m_Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.m_sType == ResName::POWERUP_PAD_SIZE_INCREASE)
				{
					if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_PAD_SIZE_INCREASE))
					{
						g_Player->m_Size = PLAYER_SIZE;
					}
				}
				else if (powerUp.m_sType == ResName::POWERUP_CONFUSE)
				{
					if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_CONFUSE))
					{
						g_Effects->m_Confuse = false;
					}
				}
				else if (powerUp.m_sType == ResName::POWERUP_CHAOS)
				{
					if (!IsOtherPowerUpActive(m_PowerUps, ResName::POWERUP_CHAOS))
					{
						g_Effects->m_Chaos = false;
					}
				}
			}
		}
	}
	m_PowerUps.erase(std::remove_if(m_PowerUps.begin(), m_PowerUps.end(), 
		[](const CPowerUp &powerUp) { return powerUp.m_IsDestroyed && !powerUp.m_IsActivated; }),
		m_PowerUps.end());
}

void CGame::Update(GLfloat dt)
{
	g_Ball->Move(dt, m_Width);
	DoCollisions();
	g_Particles->Update(dt, *g_Ball, 2, glm::vec2(g_Ball->m_Radius / 2));
	UpdatePowerUps(dt);
	if (g_ShakeTime > 0.0f) {
		g_ShakeTime -= dt;
		if (g_ShakeTime <= 0.0f)
			g_Effects->m_Shake = false;
	}
	if (g_Ball->m_Position.y >= m_Height) {
		ResetLevel();
		ResetPlayer();
	}
}


void CGame::ProcessInput(GLfloat dt, GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	else if (m_State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		//if (m_Keys[GLFW_KEY_A]) {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			if (g_Player->m_Position.x >= 0) {
				g_Player->m_Position.x -= velocity;
				if (g_Ball->m_IsStuck)
					g_Ball->m_Position.x -= velocity;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			if (g_Player->m_Position.x <= m_Width - g_Player->m_Size.x) {
				g_Player->m_Position.x += velocity;
				if (g_Ball->m_IsStuck)
					g_Ball->m_Position.x += velocity;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			g_Ball->m_IsStuck = false;
	}
}

void CGame::Render()
{
	if (m_State == GAME_ACTIVE) {
		g_Effects->BeginRender();
			// Draw background
			CTexture2D texture = CResManager::GetTexture2D(ResName::TEXTURE2D_BACKGROUND);
			g_Sprite->DrawSprite(texture, glm::vec2(0, 0), glm::vec2(m_Width, m_Height));
			// Draw level
			m_GameLevel.Draw(*g_Sprite);
			// Draw player
			g_Player->Draw(*g_Sprite);
			g_Particles->Draw();
			g_Ball->Draw(*g_Sprite);
			for (CPowerUp &powerUp : m_PowerUps)
				if (!powerUp.m_IsDestroyed)
					powerUp.Draw(*g_Sprite);
		g_Effects->EndRender();
		g_Effects->Render(glfwGetTime());
	}
}

bool CheckAABBCollision(CGameObject &one, CGameObject &two) // AABB - AABB collision
{
	// x轴方向碰撞？
	bool collisionX = one.m_Position.x + one.m_Size.x >= two.m_Position.x &&
		two.m_Position.x + two.m_Size.x >= one.m_Position.x;
	// y轴方向碰撞？
	bool collisionY = one.m_Position.y + one.m_Size.y >= two.m_Position.y &&
		two.m_Position.y + two.m_Size.y >= one.m_Position.y;
	// 只有两个轴向都有碰撞时才碰撞
	return collisionX && collisionY;
}

bool CheckBallCollision(CBallObject &ball, CGameObject &obj)
{
	// 获取圆的中心 
	glm::vec2 center(ball.m_Position + ball.m_Radius);
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(obj.m_Size.x / 2, obj.m_Size.y / 2);
	glm::vec2 aabb_center(
		obj.m_Position.x + aabb_half_extents.x,
		obj.m_Position.y + aabb_half_extents.y
	);
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	difference = closest - center;
	return glm::length(difference) < ball.m_Radius;
}

Collision CheckCollision(CBallObject & ball, CGameObject & obj)
{
	// 获取圆的中心 
	glm::vec2 center(ball.m_Position + ball.m_Radius);
	// 计算AABB的信息（中心、半边长）
	glm::vec2 aabb_half_extents(obj.m_Size.x / 2, obj.m_Size.y / 2);
	glm::vec2 aabb_center(
		obj.m_Position.x + aabb_half_extents.x,
		obj.m_Position.y + aabb_half_extents.y
	);
	// 获取两个中心的差矢量
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量并判断是否 length <= radius
	difference = closest - center;

	if (glm::length(difference) < ball.m_Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0, 0));
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void ActivatePowerUp(CPowerUp &powerUp)
{
	// 根据道具类型发动道具
	if (powerUp.m_sType == ResName::POWERUP_SPEED)
	{
		g_Ball->m_Velocity *= 1.2;
	}
	else if (powerUp.m_sType == ResName::POWERUP_STICKY)
	{
		g_Ball->m_IsSticky = true;
		g_Player->m_Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.m_sType == ResName::POWERUP_PASS_THROUGH)
	{
		g_Ball->m_IsPassThrough = true;
		g_Ball->m_Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.m_sType == ResName::POWERUP_PAD_SIZE_INCREASE)
	{
		g_Player->m_Size.x = PLAYER_SIZE.x + 50;
	}
	else if (powerUp.m_sType == ResName::POWERUP_CONFUSE)
	{
		if (!g_Effects->m_Chaos)
			g_Effects->m_Confuse = true; // 只在chaos未激活时生效，chaos同理
	}
	else if (powerUp.m_sType == ResName::POWERUP_CHAOS)
	{
		if (!g_Effects->m_Confuse)
			g_Effects->m_Chaos = true;
	}
}