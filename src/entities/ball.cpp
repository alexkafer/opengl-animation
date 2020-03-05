#include "ball.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

static const float SPEED = 10.f; // 2 meters (units) per second

Ball::Ball(float radius) {
    _radius = radius;

    _sphere = Sphere(radius, 36, 18);
    t = 0.f;
    total_time = 0.f;
}

// Animated position
void Ball::animate_position(glm::vec3 pos) {
    start = _origin;
    target = pos;
    t = 0;
    float distance = glm::distance(_origin, target);
    total_time = distance / SPEED;

    std::cout << "Starting animation from " << glm::to_string(start) << " to " << glm::to_string(target) << " for " << total_time << " seconds covering " << distance << std::endl;
}

bool Ball::check_collision(glm::vec3 position) {
    float x = (position.x - _origin.x);
    float y = (position.y - _origin.y);
    return (x*x + y*y) < (_radius*_radius);
}

void Ball::init(Shader & shader) {
    glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context

    glGenBuffers(1, &vbo); //Create buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //(Only one buffer can be bound at a time)
	glBufferData(GL_ARRAY_BUFFER, 
        _sphere.getInterleavedVertexSize(), 
        _sphere.getInterleavedVertices(), 
        GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind the VAO so we don’t accidentally modify it

    // If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.

    // Index Data
    glGenBuffers(1, &ibo); //Create index buffer called ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sphere.getIndexSize(), _sphere.getIndices(), GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  //Unbind the VAO so we don’t accidentally modify it
}

void Ball::reset() {
    t = 0;
}

void Ball::update(float dt) {
    if (t < total_time ) {
        t += dt;
        _origin = glm::mix(start, target, t / total_time);
    }
}

void Ball::draw(Shader & shader) {
    GLint attribVertexPosition     = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

    glBindVertexArray(vao); // Bind the globally created VAO to the current context
    glVertexAttrib3f(attribVertexColor, 0.7, 0.2, 0.2);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);

    // bind vbo for ball
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // set attrib arrays using glVertexAttribPointer()
    int stride = _sphere.getInterleavedStride();
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, stride, (void*) 0);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
   
    // draw a _sphere with VBO
    glDrawElements(GL_TRIANGLES,                    // primitive type
                _sphere.getIndexCount(),          // # of indices
                GL_UNSIGNED_INT,                 // data type
                (void*)0);                       // offset to indices

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ball::cleanup() {
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
