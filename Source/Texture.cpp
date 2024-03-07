#include "Texture.h"
#include <iostream>

namespace GE {
	void Texture::loadTexture(std::string filename) {
		//load texture data from file
		SDL_Surface* surfaceImage = IMG_Load(filename.c_str());

		if (surfaceImage == nullptr) {
			std::cerr << "Failed to load image: " << SDL_GetError() << std::endl;
			return;
		}

		//Dimensions of image
		width = surfaceImage->w;
		height = surfaceImage->h;

		//Format of image
		format = surfaceImage->format->format;

		switch (format) {
		case SDL_PIXELFORMAT_RGBA32: format = GL_RGBA;
			break;
		case SDL_PIXELFORMAT_RGB24: format = GL_RGB;
			break;
		default:
			format = GL_RGB;
			break;
		}
		//Creates a OpenGL texture from loaded image
		glGenTextures(1, &textureName);
		//Select created texture
		glBindTexture(GL_TEXTURE_2D, textureName);
		//Copy pixel data from SDL_surface to OpenGL texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);
		//configure how texture will be manipulated
		//gl_linear is weighted average of colours around texture coords
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SDL_FreeSurface(surfaceImage);

		return;
	}
}