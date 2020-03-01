// Copyright 2016 University of Minnesota
// 
// SHADER Uses the BSD 2-Clause License (http://www.opensource.org/licenses/BSD-2-Clause)
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other materials
//    provided with the distribution.
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF MINNESOTA, DULUTH OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Adapted from r3dux (http://r3dux.org).

#ifndef SHADER_HPP
#define SHADER_HPP

#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#ifdef USE_GLEW
#include <GL/glew.h>
#endif

#include <iostream> 
#include <iterator> 
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map> // requires C++11

#include "GLError.h"
//
//	Shader utility class for managing vert/frag shaders.
//	Does not currently handle geometry shaders.
//
//	Example use:
//	Shader myshader;
//	< OpenGL context creation >
	// myshader.init_from_files( "myshader.vert", "myshader.frag" );
	// while( rendering ){
//		< OpenGL view stuff >
//		myshader.enable();
//		glUniform3f( myshader.uniform("color"), 1.f, 0.f, 0.f );
//		< Draw stuff >
//		myshader.disable();
//	}
//

class Shader {
public:
	Shader() : program_id(0) {}

	~Shader(){ glDeleteProgram(program_id); }

	// Init the shader from files (must create OpenGL context first!)
	void init_from_files( std::string vertex_file, std::string frag_file );

	// Init the shader from strings (must create OpenGL context first!)
	void init_from_strings( std::string vertex_source, std::string frag_source ){ init(vertex_source, frag_source); }

	// Be sure to initialize the shader before enabling it
	void enable();

	// Not really needed, but nice for readability
	void disable(){ glUseProgram(0); }

	// Returns the bound location of a named attribute
	GLuint attribute( const std::string name );

	// Returns the bound location of a named uniform
	GLuint uniform( const std::string name );

	void validate( );
 
	GLuint program_id;
private:
	GLuint vertex_id;
	GLuint fragment_id;

	std::unordered_map<std::string, GLuint> attributes;
	std::unordered_map<std::string, GLuint> uniforms;

	// Initialize the shader, called by init_from_*
	void init( std::string vertex_source, std::string frag_source );

	// Compiles the shader, called by init
	GLuint compile( std::string shaderSource, GLenum type );

};

#endif

