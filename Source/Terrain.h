#pragma once
#include <glew.h>
#include <SDL.h>
#include <sdl_opengl.h>

namespace GE {
	class Terrain
	{
	public:
		Terrain();

		GLuint getVertices() { return vbo; }
		GLuint getIndices() { return ibo; }

		GLuint getIndexCount() { return indexCount; }

	private:
		GLuint vbo, ibo, indexCount;

	};
}