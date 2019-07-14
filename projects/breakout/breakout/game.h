#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gamelevel.h"
#include "gameobject.h"


// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

const glm::vec2 PLAYER_SIZE(100, 20);
const float PLAYER_VELOCITY = 500.0f;
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class CGame
{
public:
	// Game state
	GameState              m_State;
	GLboolean              m_Keys[1024];
	GLuint                 m_Width, m_Height;

	CGameLevel	m_GameLevel;
	int m_Level;

	std::vector<CPowerUp> m_PowerUps;

	// Constructor/Destructor
	CGame(GLuint width, GLuint height);
	~CGame();
	// Initialize game state (load all shaders/textures/levels)
	void Init();
	// GameLoop
	void ProcessInput(GLfloat dt, GLFWwindow *window);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();

	void SpawnPowerUps(CGameObject &block);
	void UpdatePowerUps(float dt);
};

#endif