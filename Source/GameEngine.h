#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glew.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "ModelRenderer.h"
#include "SkyboxRenderer.h"

namespace GE {

	class GameEngine {
	public:
		GameEngine();
		GameEngine(int _w, int _h);

		virtual ~GameEngine();

		bool init();
		bool keep_running();
		void processInput();
		void update();
		void draw();
		void shutdown();

		void setwindowtitle(const char*);

	private:
		SDL_Window* window;

		SDL_GLContext glContext;

		//Model Renderers
		ModelRenderer* mr;
		ModelRenderer* cr;
		ModelRenderer* tr;
		ModelRenderer* mrTerrain;

		//Models
		Model* m; 
		Model* cat;
		Model* tower;

		//Textures
		Texture* tex;
		Texture* cattex;
		Texture* towertex;

		//Camera
		Camera* cam;

		//Skybox
		SkyboxRenderer* skybox;

		//terrain
		Terrain* terrain;
		Texture* terrainTexture;

		//Camera Settings
		const float camSpeed = 0.2f;
		const float mouseSens = 0.1f;
		
		//User Input
		bool KeyDown[512];
		bool keysPressed[SDL_NUM_SCANCODES];
		bool keyStates[4] = { 0, 0, 0, 0 };
		int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3;
		int w, h;
	};

	void display_info_message(const char*);
}