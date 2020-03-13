#include "ball.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"

Ball::Ball(float radius) {
    _radius = radius;

    _sphere = Sphere(radius, 36, 18);
    t = 0.f;
    step_time = 0.f;

    _current_path = std::vector<glm::vec3>();
}

bool Ball::check_collision(const glm::vec3 & a, const glm::vec3 & b, float radius_offset) {
    // std::cout << "Collision Type: " << (a == b);
    if (a == b) {
        // std::cout << " point" << std::endl;
        float x = (a.x - _origin.x);
        float z = (b.z - _origin.z);
        return (x*x + z*z) < ((_radius + radius_offset) * (_radius + radius_offset));
    } else {
        //  std::cout << " ray" << std::endl;
        float d = glm::length(glm::cross(_origin - a, _origin - b)) / glm::length(b - a);

        // std::cout << "d: " << d << std::endl;
        // std::cout << "_radius: " << _radius << std::endl;
        // std::cout << "radius_offset: " << radius_offset << std::endl;

        return d <= (_radius + radius_offset);
    }
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
    step_time = 0;
    _current_path.clear();
}

void Ball::update(float dt) {
    if (t < step_time ) {
        t += dt;
        _origin = glm::mix(prev_step, next_step, t / step_time);
    } else if (_current_path.size() > 0) {
        // std::cout << " Current Path: ";
        // for (auto it = _current_path.begin(); it != _current_path.end(); ++it) 
        //     std::cout << ' ' << glm::to_string(*it); 
        // std::cout << std::endl;

        animate_position(_current_path.back());
        _current_path.pop_back();
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
