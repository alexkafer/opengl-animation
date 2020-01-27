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
#include <glm/glm.hpp> 
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

// Includes
#include "shader.hpp"
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
	GLuint vao, vbo[2];

	glm::vec3 eye;

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

	std::cout << glm::to_string(Globals::projection) << std::endl;
	
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
	glClearColor(.2f, 0.4f, 0.8f, 1.0f);

	// Enable the shader, this allows us to set uniforms and attributes
	shader.enable();

	Globals::model = glm::mat4(1.0f);
	Globals::eye = glm::vec3(3.0f, 0.0f, 0.0f);
	Globals::view = glm::lookAt(
		Globals::eye, // Cam Position
		glm::vec3(0.0f, 0.0f, 0.0f), // Look at point
		glm::vec3(0.0f, 0.0f, 1.0f)  // Up
	); 

	// Game loop
	while( !glfwWindowShouldClose(window) ){
	
		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float time = glfwGetTime() / 1000.f;

		Globals::model = glm::rotate(Globals::model,time * 3.14f/2, glm::vec3(0.0f, 1.0f, 1.0f));
		Globals::model = glm::rotate(Globals::model, time * 3.14f/4, glm::vec3(1.0f, 0.0f, 0.0f));

		glBindVertexArray(Globals::vao);

		// Send updated info to the GPU
		glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(Globals::model)  ); // model transformation
		glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
		glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
		glUniform3fv( shader.uniform("eye"), 1, glm::value_ptr(Globals::eye)); // used in fragment shader

		// Draw
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Finalize
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // end game loop

	// Disable the shader, we're done using it
	shader.disable();

	glDeleteBuffers(2, Globals::vbo);
	glDeleteVertexArrays(1, &Globals::vao);

    
	return EXIT_SUCCESS;
}


void init_scene(mcl::Shader * shader){

	using namespace Globals;

	GLfloat vertices[] = {
		// X Y Z R G B U V
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //Red face
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, //Green face
		0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, //Yellow face
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //Blue face
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //Black face
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, //White face
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	float normals[] = { //Normals for 36 vertices
		0.f,0.f,-1.f, 0.f,0.f,-1.f, 0.f,0.f,-1.f, 0.f,0.f,-1.f, //1-4
		0.f,0.f,-1.f, 0.f,0.f,-1.f, 0.f,0.f,1.f, 0.f,0.f,1.f, //5-8
		0.f,0.f,1.f, 0.f,0.f,1.f, 0.f,0.f,1.f, 0.f,0.f,1.f, //9-12
		-1.f,0.f,0.f, -1.f,0.f,0.f, -1.f,0.f,0.f, -1.f,0.f,0.f, //13-16
		-1.f,0.f,0.f, -1.f,0.f,0.f, 1.f,0.f,0.f, 1.f,0.f,0.f, //17-20
		1.f,0.f,0.f, 1.f,0.f,0.f, 1.f,0.f,0.f, 1.f,0.f,0.f, //21-24
		0.f,-1.f,0.f, 0.f,-1.f,0.f, 0.f,-1.f,0.f, 0.f,-1.f,0.f, //25-28
		0.f,-1.f,0.f, 0.f,-1.f,0.f, 0.f,1.f,0.f, 0.f,1.f,0.f, //29-32
		0.f,1.f,0.f, 0.f,1.f,0.f, 0.f,1.f,0.f, 0.f,1.f,0.f, //33-36
	};

	glGenBuffers(2, vbo); //Create 2 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //(Only one buffer can be bound at a time)
	// If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context

	// Cube
	GLint posAttrib = shader->attribute("in_position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
	//(above params: Attribute, vals/attrib., type, isNormalized, stride, offset)
	glEnableVertexAttribArray(posAttrib); //Mark the attribute’s location as valid

	GLint colAttrib = shader->attribute("in_color");
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(colAttrib);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW); //upload normals to vbo
	
	GLint normAttrib  = shader->attribute("in_normal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normAttrib);

	//Unbind the VAO so we don’t accidentally modify it
	glBindVertexArray(0); 
}

