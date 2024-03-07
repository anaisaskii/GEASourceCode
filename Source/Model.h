#pragma once
#include <glew.h>

namespace GE {
	struct Vertex {
		float x, y, z;
		float u, v;

		Vertex(float _x, float _y, float _z, float _u, float _v) {
			x = _x;
			y = _y;
			z = _z;

			u = _u;
			v = _v;
		}

		Vertex() {
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
	};

	class Model {
	public:
		Model() {
			vbo = 0;
			numVertices = 0;
		}

		~Model() {
			glDeleteBuffers(1, &vbo);
		}

		bool loadFromFile(const char* filename);
		GLuint getVertices() {
			return vbo;
		}

		int getNumVertices() {
			return numVertices;
		}

	private: 
		GLuint vbo; //vertex buffer object
		int numVertices; //that models have
	};
}