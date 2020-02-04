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

#include "common.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>


#include <cstring> // memcpy
#include <iostream> // memcpy


// https://github.com/syoyo/tinyobjloader

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

	glm::vec3 eye;
	glm::vec4 light;

	glm::mat4 view;
	glm::mat4 projection;

	bool track_ball;
}

Scene * scene; 
Particles * particles; 

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
			case GLFW_KEY_B: Globals::track_ball = true; break;
			case GLFW_KEY_R: Globals::track_ball = false; break;
			case GLFW_KEY_S: particles->spawn(100000); break;
		}
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	 if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    	scene->add_ball_velocity(glm::vec3(1.f, 5.f, 1.5f));
	 }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	Globals::screenWidth = width;
	Globals::screenHeight = height;

	//aspect ratio needs update on resize
	Globals::aspect = Globals::screenWidth/ (float)Globals::screenHeight; 

	Globals::projection = glm::perspective(3.14f/4, Globals::aspect, 1.0f, 50.0f);
	
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
	glfwSetMouseButtonCallback(window, &mouse_button_callback);
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

	Globals::track_ball = false;
	
	// IMPORTANT: Only call after gl context has been created
	framebuffer_size_callback(window, int(Globals::screenWidth), int(Globals::screenHeight)); 

	// Initialize the scene
	scene = new Scene();
	scene->init();

	particles = new Particles();
	particles->init();

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 target(0.0f, 2.5f, 0.0f);

	// Game loop
	float orbit_radius = 15.0f;
	float last_frame_time = glfwGetTime();
	float last_second_time = glfwGetTime();
	float dt = 0;
	int frame_count = 0;
	while( !glfwWindowShouldClose(window)){
	
		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate time delta
		float current_frame_time = glfwGetTime();
		dt = current_frame_time - last_frame_time;
		if (dt > .1) dt = .1; //Have some max dt

		frame_count++;
		if ( current_frame_time - last_second_time >= 1.0 )
		{
			// Display the frame count here any way you want.
			std::cout << "Framerate: " << frame_count << std::endl;

			frame_count = 0;
			last_second_time = current_frame_time;
		}

		last_frame_time = current_frame_time;

		float camX = sin(last_frame_time / 5) * orbit_radius;
		float camZ = cos(last_frame_time / 5) * orbit_radius;

		

		if (Globals::track_ball) {
			glm::vec3 ball = scene->get_ball_position();
			if (ball.y > 10) {
				Globals::eye = glm::vec3((5 * camX / ball.y) + ball.x, ball.y + 10, (5 *  camZ /  ball.y) + ball.z);
			} else {
				Globals::eye = glm::vec3(camX, 2.5, camZ);
			}
			
			Globals::projection = glm::perspective(3.14f/4, Globals::aspect, 1.0f, 50.0f + ball.y);

		} else {
			// target = glm::vec3(0.0f, 2.5f, 0.0f);
			Globals::eye = glm::vec3(camX, 2.5, camZ);
		}

		particles->update(dt);

		// Calculate new camera position
		// Globals::eye = glm::vec3(camX, 1.0f, camZ);

		// Calculate new view
		Globals::view = glm::lookAt(Globals::eye, target, up);

		// Send updated info to the GPU
		
		// glUniform3fv( shader.uniform("eye"), 1, glm::value_ptr(Globals::eye)); // used in fragment shader

		scene->draw(dt);
		particles->draw();

		// Finalize
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // end loop

	scene->cleanup();

	delete scene;

    
	return EXIT_SUCCESS;
}
