#include "Terrain.h"
#include "Model.h"
#include <SDL_image.h>
#include <vector>

namespace GE {
	Terrain::Terrain() {
		//Load the heightmap image from a file.
		SDL_Surface* heightMap = IMG_Load("./assets/terrainheightmap.png");

		//Get the resolution of the heightmap image as it defines the vertices in the terrain.  Each pixel in image corresponds to a vertex
		int height = heightMap->h;
		int width = heightMap->w;

		//Create a pointer variableto get pixels in image.  
		// Pixel colour defines Y of a vertex
		unsigned char* imageData = (unsigned char*)heightMap->pixels;

		//Get the number of channels to work out where red colour is in a specific pixel
		int channels = heightMap->format->BytesPerPixel;

		//Define height and size scales of the terrain - pass in?
		float heightScaling = 0.5f;
		float scaling = 50.0f;

		//Vectors for storing the vertices and indices (of vertices)
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		//create the vertices for the terrain - based on heightmap resolution width and height
		float x, y, z, u, v;
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				unsigned char redPixel = imageData[((row * width) + column) * channels];

				x = (float)column / (float)(width - 1) * scaling;
				z = (float)row / (float)(height - 1) * scaling;
				y = redPixel / 255.0f * heightScaling;

				u = (float)column / (float)(width - 1);
				v = (float)(height - row) / (float)(height - 1);

				vertices.push_back(Vertex(x, y, z, u, v));
			}
		
		}
		//Creates indices for index buffer
		unsigned int c, n;
		for (int row = 0; row < height - 1; row++) {
			for (int column = 0; column < width - 1; column++) {
				c = row * width + column;
				n = (row + 1) * width + column;

				indices.push_back(c);
				indices.push_back(n);
				indices.push_back(c + 1);
				indices.push_back(c + 1);
				indices.push_back(n);
				indices.push_back(n + 1);
			}
		}
		//Creates vertex buffer
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//create index buffer
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//store number of indices to render terrain
		indexCount = indices.size();
		SDL_FreeSurface(heightMap);
	}

}