// Basic OpenGL program
// Based on example code from: Interactive Computer Graphics: A Top-Down Approach with Shader-Based OpenGL (6th Edition), by Ed Angel

//#define GLFW_INCLUDE_GLCOREARB

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

// This file contains the code that reads the shaders from their files and compiles them
#include "ShaderStuff.hpp"

// function that is called whenever an error occurs
static void
error_callback(int error, const char* description){
    fputs(description, stderr);  // write the error description to stderr
}

// function that is called whenever a keyboard event occurs; defines how keyboard input will be handled
static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // checks to see if the escape key was pressed
        glfwSetWindowShouldClose(window, GL_TRUE);  // closes the window
}

// initialize some basic structures and geometry
typedef struct {
    float x, y;
} FloatType2D;
const int nvertices = 4;

//----------------------------------------------------------------------------

void init( void )
{
    FloatType2D vertices[nvertices];
    GLuint vao[1], buffer, location, program;
    
    vertices[0].x = -0.9;  vertices[0].y = -0.9;
    vertices[1].x =  0.9;  vertices[1].y = -0.6;
    vertices[2].x =  0.9;  vertices[2].y =  0.6;
    vertices[3].x = -0.9;  vertices[3].y =  0.9;
    
    // Create a vertex array object
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    
    // Define the names of the shader files
    std::stringstream vshader, fshader;
    vshader << SRC_DIR << "/vshader.glsl";
    fshader << SRC_DIR << "/fshader.glsl";
    
    // Load the shaders and use the resulting shader program
    program = InitShader( vshader.str().c_str(), fshader.str().c_str() );
    
    // Initialize the vertex position attribute from the vertex shader
    location = glGetAttribLocation( program, "vertex_position" );
    glEnableVertexAttribArray( location );
    glVertexAttribPointer( location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    // Define OpenGL state variables
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
    //glEnable(GL_POINT_SMOOTH);
    //glEnable(GL_LINE_SMOOTH);
    //glEnable (GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(10.0);  // huge points
    //glLineWidth(5.0);  // fat lines
}

//----------------------------------------------------------------------------

int main(void){

	GLFWwindow* window;
    
    // Define the error callback function (one of the few things that can precede glfwInit)
    glfwSetErrorCallback(error_callback);
    
    // Initialize GLFW (performs platform-specific initialization)
    if (!glfwInit()) exit(EXIT_FAILURE);
    
    // Ask for OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Use GLFW to open a window within which to display your graphics
	window = glfwCreateWindow(640, 480, "Basic Test", NULL, NULL);
	
    // Verify that the window was successfully created; if not, print error message and terminate
    if (!window)
	{
        printf("GLFW failed to create window; terminating\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
	}
    
	glfwMakeContextCurrent(window); // makes the newly-created context current

    // load opengl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to gladLoadGLLoader" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwSwapInterval(1);  // tells the system to wait to swap buffers until monitor refresh has completed; necessary to avoid tearing

    // Define the keyboard callback function
    glfwSetKeyCallback(window, key_callback);

	// Create the shaders
	init();

	while (!glfwWindowShouldClose(window)) {

        // draw graphics
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // fills the window with the background color
		glDrawArrays( GL_POINTS, 0, nvertices );    // draw a point at each vertex location
		//glDrawArrays( GL_LINES, 0, nvertices );    // draw a line between each vertex pair
		//glDrawArrays( GL_LINE_LOOP, 0, nvertices );    // draw a line between each successive vertex
		//glDrawArrays( GL_TRIANGLES, 0, nvertices );    // draw a triangle for each vertex triple
		//glDrawArrays( GL_TRIANGLE_FAN, 0, nvertices );    // draw a triangle between the first vertex and each successive vertex pair
		glFlush();	

        glfwSwapBuffers(window);  // swap buffers
        glfwPollEvents();  // check for events

	} // end graphics loop

	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();  // destroys any remaining objects, frees resources allocated by GLFW
	exit(EXIT_SUCCESS);

} // end main


