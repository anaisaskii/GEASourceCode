#pragma once

#include <glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm.hpp>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Terrain.h"

namespace GE {

	class ModelRenderer {
	public:
		ModelRenderer();
		virtual ~ModelRenderer();

		void init();
		void update();
		void draw(Camera* cam, Model *model);
		void draw(Camera* cam, Terrain* terrain);
		void destroy();

		//accessors
		float getPosX() {
			return pos_x;
		}

		float getPosY() {
			return pos_y;
		}

		float getPosZ() {
			return pos_z;
		}

		float getRotX() {
			return rot_x;
		}

		float getRotY() {
			return rot_y;
		}

		float getRotZ() {
			return rot_z;
		}

		float getScaleX() {
			return scale_x;
		}

		float getScaleY() {
			return scale_y;
		}

		//mutators
		float getScaleZ() {
			return scale_z;
		}

		void setPos(float x, float y, float z) {
			pos_x = x;
			pos_y = y;
			pos_z = z;
		}

		void setRotation(float rx, float ry, float rz) {
			rot_x = rx;
			rot_y = ry;
			rot_z = rz;
		}

		void setScale(float sx, float sy, float sz) {
			scale_x = sx;
			scale_y = sy;
			scale_z = sz;
		}

		void setTexture(Texture* _tex) {
			tex = _tex;
		}

	private:
		GLuint programId;
		GLuint vertexLocation;
		GLuint vertexUVLocation;

		Texture* tex;

		float pos_x, pos_y, pos_z;
		float rot_x, rot_y, rot_z;
		float scale_x, scale_y, scale_z;

		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;
	};
}