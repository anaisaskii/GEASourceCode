#include "GameEngine.h"
#include <iostream>

//Future Stuff:
//https://www.mbsoftworks.sk/tutorials/opengl4/020-fog/
//https://learnopengl.com/Advanced-Lighting/Bloom
//https://learnopengl.com/In-Practice/2D-Game/Particles

namespace GE 
{
	GameEngine::GameEngine() {
		w = 640;
		h = 640;
	}

	GameEngine::GameEngine(int _w, int _h) {
		w = _w;
		h = _h;
	}

	GameEngine::~GameEngine() {

	}

	bool GameEngine::init()
	{
		//Creating SDL Window
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		window = SDL_CreateWindow("SDL OpenGL", 50, 50, 640, 640, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		glContext = SDL_GL_CreateContext(window);

		if (glContext = nullptr) {
			std::cerr << "SDL could not be create GL context! GL error: " << SDL_GetError() << std::endl;
			return false;
		}

		//Initialising Glew
		GLenum status = glewInit();

		if (status != GLEW_OK) {
			std::cerr << "Error initialising GLEW! Error: " << glewGetErrorString(status) << std::endl;

			return false;
		}

		if (SDL_GL_SetSwapInterval(1) != 0)
		{
			std::cerr << "Warning: unable to set VSync! Error! " << SDL_GetError() << std::endl;

			return false;
		}

		//Camera
		cam = new Camera(glm::vec3(0.0f, 0.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

		//Models
		m = new Model();
		cat = new Model();
		tower = new Model();

		//Model Files
		m->loadFromFile("./assets/knight.obj");
		cat->loadFromFile("./assets/cat.obj");
		tower->loadFromFile("./assets/tower.obj");

		//Model Textures
		tex = new Texture("./assets/entirecolor.jpg");
		cattex = new Texture("./assets/cat.png");
		towertex = new Texture("./assets/tower.png");

		//Knight Model Renderer
		mr = new ModelRenderer();
		mr->init();
		mr->setPos(5.0f, 0.0f, -20.0f);
		mr->setScale(0.5, 0.5, 0.5);
		mr->setTexture(tex);

		//Cat Model Renderer
		cr = new ModelRenderer();
		cr->init();
		cr->setScale(5.0f, 5.0f, 5.0f);
		cr->setPos(10.0f, -6.5f, -20.0f);
		cr->setTexture(cattex);

		//Tower Model Renderer
		tr = new ModelRenderer();
		tr->init();
		tr->setScale(10.0f, 10.0f, 10.0f);
		tr->setPos(-10.0f, 12.0f, -5.0f);
		tr->setRotation(0.0f, -80.0f, 0.0f);
		tr->setTexture(towertex);
		
		//Terrain
		terrain = new Terrain();
		terrainTexture = new Texture("./assets/terraintexture.png");
		mrTerrain = new ModelRenderer();
		mrTerrain->setScale(1.0f, 1.0f, 1.0f);
		mrTerrain->setPos(-20.0f, -7.0f, -40.0f);
		mrTerrain->init();
		mrTerrain->setTexture(terrainTexture);

		//Skybox
		skybox = new SkyboxRenderer("./assets/sfront.jpg", "./assets/sback.jpg", "./assets/sright.jpg", "./assets/sleft.jpg", "./assets/stop.jpg", "./assets/sdown.jpg");

		//Hides Cursor
		SDL_ShowCursor(false);

		return true;
	}

	bool GameEngine::keep_running()
	{
		SDL_PumpEvents();

		SDL_Event evt;

		if (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT)) {
			return false;
		}
		
		return true;
	}

	void GameEngine::update()
	{
		//Rotates models
		mr->setRotation(0.0f, mr->getRotY() + 2.5f, 0.0f);
		cr->setRotation(0.0f, - (mr->getRotY() + 1.5f), 0.0f);

		//Checks keystates & how to move camera
		if (keyStates[UP]) {
			cam->setPos(cam->getPos() + cam->getTarget() * camSpeed);
		}
		if (keyStates[DOWN]) {
			cam->setPos(cam->getPos() - cam->getTarget() * camSpeed);
		}
		if (keyStates[LEFT]) {
			cam->setPos(cam->getPos() - glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed);
		}
		if (keyStates[RIGHT]) {
			cam->setPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed);
		}

		//updates attributes
		cam->updateCamMatrices();
	}

	void GameEngine::draw()
	{
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->draw(cam);
		mr->draw(cam, m);
		cr->draw(cam, cat);
		tr->draw(cam, tower);
		mrTerrain->draw(cam, terrain);

		SDL_GL_SwapWindow(window);
	}

	void GameEngine::shutdown() {
		mr->destroy();
		skybox->destroy();

		delete skybox;
		delete mr;
		delete cr;
		delete tr;
		delete tower;
		delete m;
		delete cat;
		delete cam;

		SDL_DestroyWindow(window);

		window = nullptr;

		SDL_ShowCursor(true);

		SDL_Quit();
	}

	void GameEngine::setwindowtitle(const char* new_title)
	{
		SDL_SetWindowTitle(window, new_title);
	}

	void display_info_message(const char* msg)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Basic Game Engine", msg, nullptr);
	}

	//processes user input (so user can move)
	void GameEngine::processInput() {
	

		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		int diffx = mouse_x - (w / 2);
		int diffy = (h / 2) - mouse_y;

		cam->setYaw(cam->getYaw() + diffx * mouseSens);
		cam->setPitch(cam->getPitch() + diffy * mouseSens);

		SDL_WarpMouseInWindow(window, w / 2, h / 2);

		glm::vec3 direction;

		direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		direction.y = sin(glm::radians(cam->getPitch()));
		direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));

		cam->setTarget(glm::normalize(direction));

		//check pressed keys
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					keyStates[UP] = true;
					break;
				case SDL_SCANCODE_S:
					keyStates[DOWN] = true;
					break;
				case SDL_SCANCODE_A:
					keyStates[LEFT] = true;
					break;
				case SDL_SCANCODE_D:
					keyStates[RIGHT] = true;
					break;
				}
			}

			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					keyStates[UP] = false;
					break;
				case SDL_SCANCODE_S:
					keyStates[DOWN] = false;
					break;
				case SDL_SCANCODE_A:
					keyStates[LEFT] = false;
					break;
				case SDL_SCANCODE_D:
					keyStates[RIGHT] = false;
					break;
				}
			}
		}
	}
}
