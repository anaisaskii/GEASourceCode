#include "SkyboxRenderer.h"
#include "ShaderUtils.h"
#include <SDL_image.h>
#include <iostream>
#include <gtc/type_ptr.hpp>

namespace GE {
	struct CubeVertex {
		float x, y, z; //location

		CubeVertex() {
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		CubeVertex(float _x, float _y, float _z) {
			x = _x;
			y = _y;
			z = _z;
		}
	};

	const float SIDE = 1.0f;

	CubeVertex cube[] = {
		// Front face
		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),

		// Back face
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(SIDE, -SIDE, SIDE),

		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, SIDE, SIDE),

		// Left face
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, -SIDE),

		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),

		// Right face
		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),

		// Top face
		CubeVertex(-SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE,  SIDE),

		// Bottom face
		CubeVertex(-SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE)
	};

	//create sky cube and apply texture
	void SkyboxRenderer::createCubemap(std::vector<std::string> filenames) {
		glGenTextures(1, &skyboxCubeMapName);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		for (int faceNum = 0; faceNum < 6; faceNum++) {
			SDL_Surface * surfaceImage = IMG_Load(filenames[faceNum].c_str());

			if (surfaceImage == nullptr) {
				return;
			}

			GLenum format = surfaceImage->format->format;

			switch (format) {
			case SDL_PIXELFORMAT_RGBA32:
				format = GL_RGBA;
				break;

			case SDL_PIXELFORMAT_RGB24:
				format = GL_RGB;
				break;
			
			default:
				format = GL_RGB;
				break;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceNum, 0, format, surfaceImage->w, surfaceImage->h, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

			SDL_FreeSurface(surfaceImage);
		}

		//parameters for cube map
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void SkyboxRenderer::createCubeVBO() {
		glGenBuffers(1, &vboSkybox); // 1 because one buffer object stored in vboskybox
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SkyboxRenderer::createSkyboxProgram() {
		//prepares vertices for rendering
		// applys transforms to vertexes
		//assigns vertex position as texture coordinates to be passed to fragment shader
		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"out vec3 texCoord;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projection * view * v;\n"
			"gl_Position = v;\n"
			"texCoord = vertexPos3D;\n"
			"}\n"
		};

		const GLchar* F_ShaderCode[] = {
			//computes colour of each pixel by sampling cube map
			"#version 140\n"
			"in vec3 texCoord;\n"
			"uniform samplerCube sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = vec4(texture(sampler, texCoord).rgb, 1.0f);\n"
			"}\n"
		};

		//error checking
		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &skyboxProgramId);

		if (!result) {
			std::cerr << "Failed to create SKyboxRenderer program" << std::endl;
			return;
		}

		vertexLocation = glGetAttribLocation(skyboxProgramId, "vertexPos3D");

		if (vertexLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		viewUniformId = glGetUniformLocation(skyboxProgramId, "view");
		projectionUniformId = glGetUniformLocation(skyboxProgramId, "projection");
		samplerId = glGetUniformLocation(skyboxProgramId, "sampler");
		
	}

	void SkyboxRenderer::draw(Camera* cam) {
		bool isDepthTestEnable = glIsEnabled(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);

		glm::mat4 camView = cam->getViewMatrix();
		glm::mat4 projection = cam->getProjectionMatrix();

		//setting camera position to 0,0,0
		camView[3][0] = 0.0f;
		camView[3][1] = 0.0f;
		camView[3][2] = 0.0f;

		//uses compiled skybox shader code
		glUseProgram(skyboxProgramId);
		//binds skybox data to array buffer
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		//to transform vertices from world space to screen space
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(camView));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projection));
		
		//allows vertex data to be passed to shader
		glEnableVertexAttribArray(vertexLocation);

		//vertices have xyz
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, x));

		//texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		//draw faces 
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(CubeVertex));

		//clean up (unbind)
		glDisableVertexAttribArray(vertexLocation);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (isDepthTestEnable) {
			glEnable(GL_DEPTH_TEST);
		}
	}

	//clean up
	void SkyboxRenderer::destroy() {
		glDeleteProgram(skyboxProgramId);
		glDeleteBuffers(1, &vboSkybox);
		glDeleteTextures(1, &skyboxCubeMapName);
	}
}
