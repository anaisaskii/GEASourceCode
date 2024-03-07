#include "ShaderUtils.h"
#include <iostream>

namespace GE {
	void _displayShaderCompilerError(GLuint shaderId) {
		GLint MsgLen = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

		// only display if the error message isn't empty
		if (MsgLen > 1) {
			// create a character buffer to store the error message characters
			GLchar* Msg = new GLchar[MsgLen + 1];

			// Get the error message characters from opengl
			glGetShaderInfoLog(shaderId, MsgLen, nullptr, Msg);

			// Display the error message
			std::cerr << "Error compiling shader" << Msg << std::endl;

			// Release the memory allocated to the string
			delete[] Msg;
		}
	}

	bool compileProgram(const GLchar* v_shader_sourcecode[], const GLchar* f_shader_sourcecode[], GLuint* programId) {
		// Create the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Copy the source to OpenGL ready for compilation
		glShaderSource(vertexShader, 1, v_shader_sourcecode, nullptr);

		// Compile code
		glCompileShader(vertexShader);

		// Check for compiler errors
		// assume shader didn't compile
		GLint isShaderCompiledOK = GL_FALSE;

		// Get the compile status from OpenGL
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile vertex shader" << std::endl;

			_displayShaderCompilerError(vertexShader);

			return false;
		}

		// Do the same for the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Transfer the shader code
		glShaderSource(fragmentShader, 1, f_shader_sourcecode, NULL);

		glCompileShader(fragmentShader);

		isShaderCompiledOK = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile fragment shader" << std::endl;

			_displayShaderCompilerError(fragmentShader);

			return false;
		}

		// Create the program object
		*programId = glCreateProgram();

		// Attach shaders to the program object
		glAttachShader(*programId, vertexShader);
		glAttachShader(*programId, fragmentShader);

		// link the program to create an executable program
		// program executable exists in graphics memory
		glLinkProgram(*programId);

		// Check for linking errors
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(*programId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program" << std::endl;

			return false;
		}

		return true;
	}

}