// This is based on a homework assignment for CSCI5607

#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#endif

// The loaders are included by glfw3 (glcorearb.h) if we are not using glew.
#ifdef USE_GLEW
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

// Includes
#include "shader.hpp"
#include <cstring> // memcpy

// Constants
#define WIN_WIDTH 500
#define WIN_HEIGHT 500


//
//	Global state variables
//
namespace Globals {
	bool fullscreen = false;
	int screenWidth;
	int screenHeight;
	GLuint vao, vbo;
}


//
//	Callbacks
//
static void error_callback(int error, const char* description){ fprintf(stderr, "Error: %s\n", description); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// Close on escape or Q
	if( action == GLFW_PRESS ){
		switch ( key ) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GL_TRUE); break;
		}
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	Globals::screenWidth = width;
	Globals::screenHeight = height;
	
    glViewport(0,0,width,height);
}


// Function to set up geometry
void init_scene(mcl::Shader * shader);

//
//	Main
//
int main(int argc, char *argv[]){

	// Set up window
	GLFWwindow* window;
	glfwSetErrorCallback(&error_callback);

	// Initialize the window
	if( !glfwInit() ){ return EXIT_FAILURE; }

	// Ask for OpenGL 3.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the glfw window
	Globals::screenWidth = WIN_WIDTH;
	Globals::screenHeight = WIN_HEIGHT;
	window = glfwCreateWindow(int(Globals::screenWidth), int(Globals::screenHeight), "CSCI5611 - Alex Kafer", NULL, NULL);
	
	if( !window ){ glfwTerminate(); return EXIT_FAILURE; }

	// Bind callbacks to the window
	glfwSetKeyCallback(window, &key_callback);
	glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);

	// Make current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize glew AFTER the context creation and before loading the shader.
	// Note we need to use experimental because we're using a modern version of opengl.
	#ifdef USE_GLEW
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	// Initialize the shader (which uses glew, so we need to init that first).
	// MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
	// the full path to this project's src/ directory.
	mcl::Shader shader;
	std::stringstream ss; ss << MY_SRC_DIR << "shader.";
	shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

	// Initialize the scene
	// IMPORTANT: Only call after gl context has been created
	init_scene(&shader);
	framebuffer_size_callback(window, int(Globals::screenWidth), int(Globals::screenHeight)); 

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.f,1.f,1.f,1.f);

	// Enable the shader, this allows us to set uniforms and attributes
	shader.enable();

	// Game loop
	while( !glfwWindowShouldClose(window) ){

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(Globals::vao);

		// Send updated info to the GPU
		// glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, Globals::model.m  ); // model transformation
		// glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, Globals::view.m  ); // viewing transformation
		// glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, Globals::projection.m ); // projection matrix
		// glUniform3f( shader.uniform("eye"), eye[0], eye[1], eye[2] ); // used in fragment shader

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Finalize
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // end game loop

	// Disable the shader, we're done using it
	shader.disable();

	glDeleteBuffers(1, &Globals::vbo);
	glDeleteVertexArrays(1, &Globals::vao);

    
	return EXIT_SUCCESS;
}


void init_scene(mcl::Shader * shader){

	using namespace Globals;

	GLfloat vertices[] = {
		0.0f, 0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1: postion = (0,.5) color = Red
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2: postion = (.5,-.5) color = Green
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f // Vertex 3: postion = (-.5,-.5) color = Blue
	};

	glGenBuffers(1, &vbo); //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //(Only one buffer can be bound at a time)
	// If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context


	GLint posAttrib = shader->attribute("position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	//(above params: Attribute, vals/attrib., type, isNormalized, stride, offset)
	glEnableVertexAttribArray(posAttrib); //Mark the attribute’s location as valid

	GLint colAttrib =  shader->attribute("inColor");
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(colAttrib);

	//Unbind the VAO so we don’t accidentally modify it
	glBindVertexArray(0); 
}

