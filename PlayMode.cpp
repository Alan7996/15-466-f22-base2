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

		if (transform->name == "BlueSphere" || transform->name == "RedSphere") {
			// make copies of each sphere
			for (uint8_t i = 0; i < 32; i++) {
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();

				drawable.pipeline = lit_color_texture_program_pipeline;

				drawable.pipeline.vao = meshes_for_lit_color_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
			}
		}
		else {
			scene.drawables.emplace_back(transform);
			Scene::Drawable &drawable = scene.drawables.back();

			drawable.pipeline = lit_color_texture_program_pipeline;

			drawable.pipeline.vao = meshes_for_lit_color_texture_program;
			drawable.pipeline.type = mesh.type;
			drawable.pipeline.start = mesh.start;
			drawable.pipeline.count = mesh.count;
		}

	});
});

PlayMode::PlayMode() : scene(*main_scene) {

	for (auto &drawable : scene.drawables) {
		if (drawable.transform->name == "BlueSphere") {
			blueBalls[activeBlue] = &drawable;
			activeBlue++;
		}
		else if (drawable.transform->name == "RedSphere") {
			redBalls[activeRed] = &drawable;
			activeRed++;
		}
	}
	
	activeRed = 0;
	activeBlue = 0;

	// //get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
}

PlayMode::~PlayMode() {
}

void PlayMode::place_ball(uint8_t x, uint8_t y) {

	printf("%u", board[3][y][x]);
	std::cout << "" << std::endl;
	if (board[3][y][x] == 0) {
		if (isP1Turn) {
			// place red ball at highest z
			isP1Turn = false;
			if (board[0][y][x] == 0) {
		std::cout << "0" << std::endl;
				board[0][y][x] = 1;
				redBalls[activeRed]->transform->position = glm::vec3(-3 + 2 * x, 1, -3 + 2 * y);
				activeRed++;
			} else if (board[1][y][x] == 0) {
		std::cout << "1" << std::endl;
				board[1][y][x] = 1;
				redBalls[activeRed]->transform->position = glm::vec3(-3 + 2 * x, 3, -3 + 2 * y);
				activeRed++;
			} else if (board[2][y][x] == 0) {
		std::cout << "2" << std::endl;
				board[2][y][x] = 1;
				redBalls[activeRed]->transform->position = glm::vec3(-3 + 2 * x, 5, -3 + 2 * y);
				activeRed++;
			} else if (board[3][y][x] == 0) {
		std::cout << "3" << std::endl;
				board[3][y][x] = 1;
				redBalls[activeRed]->transform->position = glm::vec3(-3 + 2 * x, 7, -3 + 2 * y);
				activeRed++;
			}
		} else {
			// place blue ball at highest z
			isP1Turn = true;
			if (board[0][y][x] == 0) {
		std::cout << "0" << std::endl;
				board[0][y][x] = 1;
				blueBalls[activeBlue]->transform->position = glm::vec3(-3 + 2 * x, 1, -3 + 2 * y);
				activeBlue++;
			} else if (board[1][y][x] == 0) {
		std::cout << "1" << std::endl;
				board[1][y][x] = 1;
				blueBalls[activeBlue]->transform->position = glm::vec3(-3 + 2 * x, 3, -3 + 2 * y);
				activeBlue++;
			} else if (board[2][y][x] == 0) {
		std::cout << "2" << std::endl;
				board[2][y][x] = 1;
				blueBalls[activeBlue]->transform->position = glm::vec3(-3 + 2 * x, 5, -3 + 2 * y);
				activeBlue++;
			} else if (board[3][y][x] == 0) {
		std::cout << "3" << std::endl;
				board[3][y][x] = 1;
				blueBalls[activeBlue]->transform->position = glm::vec3(-3 + 2 * x, 7, -3 + 2 * y);
				activeBlue++;
			}
		}
	}
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
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
			place_ball(1, 1);
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
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

	//move camera:
	{

		//combine inputs into a move:
		constexpr float PlayerSpeed = 20.0f;
		glm::vec3 move = glm::vec3(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;
		if (zDown.pressed && !zUp.pressed) move.z =-1.0f;
		if (!zDown.pressed && zUp.pressed) move.z = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec3(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		//glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = glm::vec3(-1.0f, 0.0f, 0.0f);
		glm::vec3 frame_forward = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 frame_up = glm::vec3(0.0f, 0.0f, 1.0f);

		camera->transform->position += move.x * frame_right + move.y * frame_forward + 
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
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
