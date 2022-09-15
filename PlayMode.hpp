#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	void place_ball(uint8_t x, uint8_t y);
	int PlayMode::check_win(int x, int y, int z, int player);

	//----- game state -----
	enum GameState {
		PLAYING,
		GAMEOVER,
	} gameState;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, zDown, zUp;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *arrow = nullptr;
	glm::quat arrow_rotation;
	Scene::Transform *text = nullptr;
	glm::quat text_rotation;
	float wobble = 0.0f;
	
	//camera:
	Scene::Camera *camera = nullptr;
	glm::quat initCam;

	//game variables:
	// holds 0 by default, 1 if player 1 (red) placed it, 2 if player 2 (blue) placed it
	bool isP1Turn = true;
	bool didP1Win = false;
	// the board is 5x5x5 to keep the last 1 entries of each dimension as a way of tracking win state
	// each red ball is +1, and blue ball is -1. if ever these entries become + or - 4 the corresponding player wins
	int board[5][5][5] = {0};
	int activeRed = 0;
	int activeBlue = 0;
	std::vector<Scene::Drawable *> redBalls;
	std::vector<Scene::Drawable *> blueBalls;

};
