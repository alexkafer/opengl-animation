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

#define GLM_FORCE_RADIANS //ensure we are using radians
#include <glm/vec3.hpp> 
#include <glm/mat4x4.hpp> 


#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include "scene.hpp"
#include <cstring> // memcpy
#include <iostream> // memcpy

// Constants
#define WIN_WIDTH 500
#define WIN_HEIGHT 500

//
//	Global state variables
//
namespace Globals {
	int screenWidth;
	int screenHeight;
	float aspect;
	GLuint vao, vbo, ibo;

	glm::vec3 eye;
	glm::vec4 light;

	//  Model, view and projection matrices, initialized to the identity
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
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

	//aspect ratio needs update on resize
	Globals::aspect = Globals::screenWidth/ (float)Globals::screenHeight; 

	Globals::projection = glm::perspective(3.14f/4, Globals::aspect, 1.0f, 10.0f);
	
    glViewport(0,0,width,height);
}


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
	init_geometry(&shader, Globals::vbo, Globals::ibo, Globals::vao);
	framebuffer_size_callback(window, int(Globals::screenWidth), int(Globals::screenHeight)); 

	// Enable the shader, this allows us to set uniforms and attributes
	shader.enable();

	init_static_uniforms(&shader);

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Globals::model = glm::mat4(1.0f);

	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 target(0.0f, 0.0f, 0.0f);

	// Game loop
	const float orbit_radius = 5.0f;
	float last_time = glfwGetTime();
	while( !glfwWindowShouldClose(window) ){
	
		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate time delta
		float new_time = glfwGetTime();
		float time_delta = new_time - last_time;

		float camX = sin(new_time) * orbit_radius;
		float camZ = cos(new_time) * orbit_radius;

		// Calculate new camera position
		Globals::eye = glm::vec3(camX, 0.0f, camZ);

		// Calculate new view
		Globals::view = glm::lookAt(Globals::eye, target, up);

		// Globals::light = glm::rotate(Globals::light, time * 3.14f/2, glm::vec3(0.0f, 1.0f, 0.0f));	
		Globals::model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2 * sin(new_time), 0.0f));
		
		// bind vbo for ball
		glBindBuffer(GL_ARRAY_BUFFER, Globals::vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Globals::ibo);

		// Send updated info to the GPU
		glm::mat4 matrix_normal = Globals::model;
    	matrix_normal[3] = glm::vec4(0,0,0,1);

		glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(Globals::model)  ); // model transformation
		glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
		glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
		glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_normal)); // projection matrix
		
		// glUniform3fv( shader.uniform("eye"), 1, glm::value_ptr(Globals::eye)); // used in fragment shader

		draw_scene(&shader, Globals::vbo, Globals::ibo);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Finalize
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // end loop

	// Disable the shader, we're done using it
	shader.disable();

	glDeleteBuffers(1, &Globals::vbo);
	glDeleteBuffers(1, &Globals::ibo);
	glDeleteVertexArrays(1, &Globals::vao);

    
	return EXIT_SUCCESS;
}
