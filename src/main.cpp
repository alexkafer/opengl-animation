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

static const glm::vec3 up(0.0f, 1.0f, 0.0f);

//
//	Global state variables
//
namespace Globals {
	int screen_width;
	int screen_height;
	float aspect;

	float mouse_x;
	float mouse_y;
	bool first_mouse;

	float yaw;
	float pitch;

	glm::vec3 eye_pos;
	glm::vec3 eye_dir;
	glm::vec4 light;

	glm::mat4 view;
	glm::mat4 projection;
}

Scene * scene; 
Particles * particles; 

//
//	Callbacks
//
static void error_callback(int error, const char* description){ fprintf(stderr, "Error: %s\n", description); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// Close on escape or Q
	const float cameraSpeed = 0.5f;

	if( action == GLFW_PRESS || action == GLFW_REPEAT){
		switch ( key ) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GL_TRUE); break;
			case GLFW_KEY_SPACE: particles->spawn(100000); break;
			case GLFW_KEY_W: Globals::eye_pos += cameraSpeed * Globals::eye_dir; break;
			case GLFW_KEY_S: Globals::eye_pos -= cameraSpeed * Globals::eye_dir; break;
			case GLFW_KEY_A: Globals::eye_pos -= glm::normalize(glm::cross( Globals::eye_dir, up)) * cameraSpeed; break;
			case GLFW_KEY_D: Globals::eye_pos += glm::normalize(glm::cross( Globals::eye_dir, up)) * cameraSpeed; break;
		}
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	 if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    	scene->add_ball_velocity(glm::vec3(1.f, 5.f, 1.5f));
	 }
}

void calculate_eye_direction() {
	glm::vec3 direction;
	direction.x = cos(Globals::yaw)* cos(Globals::pitch);
	direction.y = sin(Globals::pitch);
	direction.z = sin(Globals::yaw)* cos(Globals::pitch);
	Globals::eye_dir = glm::normalize(direction);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if(Globals::first_mouse)
    {
        Globals::mouse_x = xpos;
        Globals::mouse_y = ypos;
        Globals::first_mouse = false;
    }

	float xoffset = xpos - Globals::mouse_x;
	float yoffset = Globals::mouse_y - ypos; // reversed since y-coordinates range from bottom to top
	
	Globals::mouse_x = xpos;
	Globals::mouse_y = ypos;

	const float sensitivity = 0.001f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Globals::yaw   += xoffset;
	Globals::pitch += yoffset;  

	if(Globals::pitch > 89.0f)
		Globals::pitch =  89.0f;
	if(Globals::pitch < -89.0f)
		Globals::pitch = -89.0f;	

	calculate_eye_direction();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	
	Globals::screen_width = width;
	Globals::screen_height = height;

	//aspect ratio needs update on resize
	Globals::aspect = Globals::screen_width/ (float)Globals::screen_height; 

	Globals::projection = glm::perspective(3.14f/4, Globals::aspect, 1.0f, 50.0f);
	
    glViewport(0,0,width,height);
}

void lookAt(glm::vec3 center)
{
	glm::vec3 direction = glm::normalize(center - Globals::eye_pos);
	Globals::pitch = asin(direction.y);
	Globals::yaw = atan2(direction.x, direction.z);

	calculate_eye_direction();
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
	Globals::first_mouse = true;
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CSCI5611 - Alex Kafer", NULL, NULL);

	glfwGetFramebufferSize(window, &Globals::screen_width, &Globals::screen_height);
	
	if( !window ){ glfwTerminate(); return EXIT_FAILURE; }

	// Bind callbacks to the window
	glfwSetKeyCallback(window, &key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetMouseButtonCallback(window, &mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
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

	
	// IMPORTANT: Only call after gl context has been created
	framebuffer_size_callback(window, int(Globals::screen_width), int(Globals::screen_height)); 

	// Initialize the scene
	scene = new Scene();
	scene->init();

	particles = new Particles();
	particles->init();

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Initialize camera
	Globals::eye_pos = glm::vec3(15.f, 2.5f, 15.f);
	
	lookAt(glm::vec3(0.f, 0.f, 0.f));

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

		// float camX = sin(last_frame_time / 5) * orbit_radius;
		// float camZ = cos(last_frame_time / 5) * orbit_radius;


		particles->update(dt);

		// Calculate new camera position
		// Globals::eye = glm::vec3(camX, 1.0f, camZ);

		// Calculate new view
		Globals::view = glm::lookAt(Globals::eye_pos, Globals::eye_pos + Globals::eye_dir, up);

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
