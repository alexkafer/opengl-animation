#include "floor.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../utils/GLError.h"

// Make it 20x20
Floor::Floor(): Renderable(glm::vec3(10.f)) {}

void Floor::init(Shader & shader) {

    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao);

    float vertices[] = {
        // X      Y     Z     R     G      B      U      V
        -1.f, 0.f, -1.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
        1.f, 0.f, -1.f, 0.376f, 0.502f, 0.22f, 1.0f, 1.0f,
        1.f, 0.f,  1.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        1.f, 0.f,  1.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        -1.f, 0.f,  1.f, 0.376f, 0.502f, 0.22f, 0.0f, 0.0f,
        -1.f, 0.f, -1.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
    };

    float normals[] = {
        0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,
    };

	glGenBuffers(2, vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //upload vertices to vbo

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW); //upload normals to vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    check_gl_error();
}

void Floor::draw(Shader & shader) {
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

    glBindVertexArray(vao);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexColor);
    glEnableVertexAttribArray(attribVertexNormal);

    // set attrib arrays using glVertexAttribPointer()
    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glVertexAttribPointer(attribVertexColor, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // draw a floor with VBO
    glDrawArrays(GL_TRIANGLES, 0, 6); //(Primitives, Which VBO, Number of vertices)

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexColor);
    glDisableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Floor::cleanup() {
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
}