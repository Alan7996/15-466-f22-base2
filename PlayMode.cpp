#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > main_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("connectBoard.pnct"));
	meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("connectBoard.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = main_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*main_scene) {

	gameState = PLAYING;

	for (auto &transform : scene.transforms) {
		if (transform.name == "Cone") arrow = &transform;
		else if (transform.name == "Text") text = &transform;
	}
	arrow_rotation = arrow->rotation;
	text_rotation = text->rotation;

	for (auto &drawable : scene.drawables) {
		if (drawable.transform->name.find("BlueSphere") != std::string::npos) {
			blueBalls.emplace_back(&drawable);
		}
		else if (drawable.transform->name.find("RedSphere") != std::string::npos) {
			redBalls.emplace_back(&drawable);
		}
	}
	
	activeRed = 0;
	activeBlue = 0;

	// //get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	initCam = camera->transform->rotation;
}

PlayMode::~PlayMode() {
}

void PlayMode::place_ball(uint8_t x, uint8_t y) {

	int res = 0;
	if (board[3][y][x] == 0) {
		if (isP1Turn) {
			// place red ball at highest z
			isP1Turn = false;
			for (uint8_t z = 0; z < 4; z++) {
				if(board[z][y][x] == 0) {
					board[z][y][x] = 1;
					redBalls[activeRed]->transform->position = glm::vec3(-3 + 2 * x, -3 + 2 * y, 1 + 2 * z);

					board[4][y][x] += 1;
					board[z][4][x] += 1;
					board[z][y][4] += 1;

					activeRed++;

					res = check_win(x, y, z, 1);

					break;
				}
			}
		} else {
			// place blue ball at highest z
			isP1Turn = true;
			for (uint8_t z = 0; z < 4; z++) {
				if(board[z][y][x] == 0) {
					board[z][y][x] = -1;
					blueBalls[activeBlue]->transform->position = glm::vec3(-3 + 2 * x, -3 + 2 * y, 1 + 2 * z);

					board[4][y][x] -= 1;
					board[z][4][x] -= 1;
					board[z][y][4] -= 1;
					
					activeBlue++;

					res = check_win(x, y, z, -1);

					break;
				}
			}
		}
	}

	if (res != 0) {
		didP1Win = (res == 1) ? true : false;
		gameState = GAMEOVER;
	}
}

int PlayMode::check_win(int x, int y, int z, int player) {
	
	if (player == 1) {
		if (board[4][y][x] == 4 || board[z][4][x] == 4 || board[z][y][4] == 4) {
			// red wins
			return 1;
		}
	}
	else {
		if (board[4][y][x] == -4 || board[z][4][x] == -4 || board[z][y][4] == -4) {
			// blue wins
			return -1;
		}
	}
	// no winner yet
	return 0;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (gameState == PLAYING) {
			if (evt.key.keysym.sym == SDLK_a) {
				left.downs += 1;
				left.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_d) {
				right.downs += 1;
				right.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_w) {
				up.downs += 1;
				up.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_s) {
				down.downs += 1;
				down.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_e) {
				zUp.downs += 1;
				zUp.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_q) {
				zDown.downs += 1;
				zDown.pressed = true;
				return true;
			} else if (evt.key.keysym.sym == SDLK_v) {
				place_ball(0, 0);
				return true;
			} else if (evt.key.keysym.sym == SDLK_b) {
				place_ball(1, 0);
				return true;
			} else if (evt.key.keysym.sym == SDLK_n) {
				place_ball(2, 0);
				return true;
			} else if (evt.key.keysym.sym == SDLK_m) {
				place_ball(3, 0);
				return true;
			} else if (evt.key.keysym.sym == SDLK_g) {
				place_ball(0, 1);
				return true;
			} else if (evt.key.keysym.sym == SDLK_h) {
				place_ball(1, 1);
				return true;
			} else if (evt.key.keysym.sym == SDLK_j) {
				place_ball(2, 1);
				return true;
			} else if (evt.key.keysym.sym == SDLK_k) {
				place_ball(3, 1);
				return true;
			} else if (evt.key.keysym.sym == SDLK_y) {
				place_ball(0, 2);
				return true;
			} else if (evt.key.keysym.sym == SDLK_u) {
				place_ball(1, 2);
				return true;
			} else if (evt.key.keysym.sym == SDLK_i) {
				place_ball(2, 2);
				return true;
			} else if (evt.key.keysym.sym == SDLK_o) {
				place_ball(3, 2);
				return true;
			} else if (evt.key.keysym.sym == SDLK_7) {
				place_ball(0, 3);
				return true;
			} else if (evt.key.keysym.sym == SDLK_8) {
				place_ball(1, 3);
				return true;
			} else if (evt.key.keysym.sym == SDLK_9) {
				place_ball(2, 3);
				return true;
			} else if (evt.key.keysym.sym == SDLK_0) {
				place_ball(3, 3);
				return true;
			} else if (evt.key.keysym.sym == SDLK_SPACE) {
				camera->transform->rotation = initCam;
				return true;
			}
		}
	} else if (gameState == PLAYING && evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			zUp.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_q) {
			zDown.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	if (gameState != PLAYING) return;

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	arrow->rotation = arrow_rotation * glm::angleAxis(
		glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
		glm::vec3(0.0f, 1.0f, 0.0f)
	) * glm::angleAxis(
		glm::radians(5.0f * std::cos(wobble * 2.0f * float(M_PI))),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	text->rotation = text_rotation * glm::angleAxis(
		glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
		glm::vec3(0.0f, 1.0f, 0.0f)
	) * glm::angleAxis(
		glm::radians(5.0f * std::cos(wobble * 2.0f * float(M_PI))),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 20.0f;
		glm::vec3 move = glm::vec3(0.0f);
		if (left.pressed && !right.pressed) move.x = 1.0f;
		if (!left.pressed && right.pressed) move.x =-1.0f;
		if (down.pressed && !up.pressed) move.y = 1.0f;
		if (!down.pressed && up.pressed) move.y =-1.0f;
		if (zDown.pressed && !zUp.pressed) move.z =-1.0f;
		if (!zDown.pressed && zUp.pressed) move.z = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec3(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		//glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 frame_forward = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 frame_up = glm::vec3(0.0f, 0.0f, 1.0f);

		camera->transform->position += 
									((left.pressed && camera->transform->position.x <= -30.0f) ? 0 : (right.pressed && camera->transform->position.x >= 30.0f) ? 0 : move.x) * frame_right + 
									((down.pressed && camera->transform->position.y <= -30.0f) ? 0 : (up.pressed && camera->transform->position.y >= 30.0f) ? 0 : move.y) * frame_forward + 
									((zDown.pressed && camera->transform->position.z <= 1.0f) ? 0 : move.z) * frame_up;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; SPACE resets camera rotation; WASD moves; QE goes down/up",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; SPACE resets camera rotation; WASD moves; QE goes down/up",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		if (gameState == GAMEOVER) {
			if (didP1Win) {
				lines.draw_text("Red player wins!",
					glm::vec3(-aspect + 5.0f * H, -1.0 + 10.0f * H, 0.0),
					glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
					glm::u8vec4(0x00, 0x00, 0x00, 0x00));
				lines.draw_text("Red player wins!",
					glm::vec3(-aspect + 5.0f * H + ofs, -1.0 + + 10.0f * H + ofs, 0.0),
					glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
					glm::u8vec4(0xff, 0xff, 0xff, 0x00));
			} else {
				lines.draw_text("Blue player wins!",
					glm::vec3(-aspect + 5.0f * H, -1.0 + 10.0f * H, 0.0),
					glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
					glm::u8vec4(0x00, 0x00, 0x00, 0x00));
				lines.draw_text("Blue player wins!",
					glm::vec3(-aspect + 5.0f * H + ofs, -1.0 + + 10.0f * H + ofs, 0.0),
					glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
					glm::u8vec4(0xff, 0xff, 0xff, 0x00));
			}
		}
	}
}
