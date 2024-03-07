#include "ModelRenderer.h"
#include <iostream>
#include <gtc/type_ptr.hpp>

namespace GE {
	GLfloat vertexData[] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	ModelRenderer::ModelRenderer() //Could take in pos/rot/scale as parameter?
	{
		//set pos/rot/scale to default
		pos_x = pos_y = pos_z = 0.0f;
		rot_x = rot_y = rot_z = 0.0f;
		scale_x = scale_y = scale_z = 0.0f;

		programId = 0;

		vertexLocation = 0;

		transformUniformId = 0;
		viewUniformId = 0;
		projectionUniformId = 0;
	}

	ModelRenderer::~ModelRenderer()
	{

	}

	void displayShaderCompilerError(GLuint shaderId)
	{
		GLint MsgLen = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

		if (MsgLen > 1) {
			GLchar* Msg = new GLchar[MsgLen + 1];
			glGetShaderInfoLog(shaderId, MsgLen, NULL, Msg);
			std::cerr << "Error compiling shader" << Msg << std::endl;
			delete[] Msg;
		}
	}

	void ModelRenderer::init()
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"in vec2 vUV;\n"
			"out vec2 uv;\n"
			"uniform mat4 transformMat;\n"
			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projMat * viewMat * transformMat * v;\n"
			"gl_Position = v;\n"
			"uv = vUV;\n"
			"}\n" };

		glShaderSource(vertexShader, 1, V_ShaderCode, NULL);

		glCompileShader(vertexShader);

		GLint isShaderCompiledOK = GL_FALSE;

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile vertex shader" << std::endl;
			displayShaderCompilerError(vertexShader);
			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"in vec2 uv;\n"
			"uniform sampler2D sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = texture(sampler, uv).rgba;\n" //colour of the triangle
			"}\n"
		};

		glShaderSource(fragmentShader, 1, F_ShaderCode, NULL);

		glCompileShader(fragmentShader);

		isShaderCompiledOK = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile fragment shader" << std::endl;

			displayShaderCompilerError(fragmentShader);

			return;
		}

		programId = glCreateProgram();

		glAttachShader(programId, vertexShader);
		glAttachShader(programId, fragmentShader);

		glLinkProgram(programId);
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(programId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE)
		{
			std::cerr << "Failed to link program" << std::endl;
		}

		vertexUVLocation = glGetAttribLocation(programId, "vUV"); // move this back to modelrenderer::modelrenderer if errors

		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl; //and this
		}
		
		viewUniformId = glGetUniformLocation(programId, "viewMat");
		projectionUniformId = glGetUniformLocation(programId, "projMat");
		transformUniformId = glGetUniformLocation(programId, "transformMat");
		samplerId = glGetUniformLocation(programId, "sampler");

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void ModelRenderer::update()
	{

	}

	void ModelRenderer::draw(Camera* cam, Model *model)
	{
		glEnable(GL_CULL_FACE);

		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		glUseProgram(programId);

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		glBindBuffer(GL_ARRAY_BUFFER, model->getVertices());

		glEnableVertexAttribArray(vertexLocation);

		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		glEnableVertexAttribArray(vertexUVLocation);

		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		//Texture stuff
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());

		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0);

		glDisable(GL_CULL_FACE);
	}

	void ModelRenderer::draw(Camera* cam, Terrain* terrain)
	{
		glEnable(GL_CULL_FACE);

		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		glUseProgram(programId);

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertices());

		glEnableVertexAttribArray(vertexLocation);

		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		glEnableVertexAttribArray(vertexUVLocation);

		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getIndices());
		glDrawElements(GL_TRIANGLES, terrain->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0);

		glDisable(GL_CULL_FACE);
	}

	void ModelRenderer::destroy()
	{
		glDeleteProgram(programId);
	}
}