#include "cloth.h"

#include <iostream>
#include <utility>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static glm::vec3 gravity(0.f, -9.8f, 0.f);
// static glm::vec3 gravity(0.f, -1.f, 0.f);

float restLen = .1f;
float k = 5000.f; //TRY-IT: How does changing k affect resting length?
float kv = 30.f;
float mass = 0.5f;

Cloth::Cloth(size_t x_dim, size_t y_dim) {
    _x_dim = x_dim;
    _y_dim = y_dim;

    pointMasses = std::vector<PointMass>();
    pointMasses.reserve(x_dim * y_dim);

    edges = std::vector<std::pair<size_t, size_t>>();
    size_t edge_count = (x_dim-1)*y_dim + (y_dim-1)*x_dim;
    edges.reserve(edge_count);

    ball = Sphere(2.f*restLen, 36, 18);

    // pointMasses.push_back(PointMass(glm::vec3(2.f * (float) rand() / RAND_MAX - 1.f, (points + 15.f) / 5.f, 2.f * (float) rand() / RAND_MAX - 1.f), 0));
    size_t vertex[x_dim][y_dim];

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            vertex[x][y] = pointMasses.size();
            // std::cout << "Generated " << vertex[x][y] << " at (" << x << ", "<< y << ")" << std::endl;
            pointMasses.push_back(PointMass(glm::vec3(x * restLen, restLen * (y_dim - y) + 5.f, 0.f)));
        }
    }


    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            
            if (x < x_dim-1) {
                // std::cout << "Paired " << vertex[x][y] << " with " <<vertex[x+1][y] << std::endl;
                edges.push_back(std::make_pair(vertex[x][y], vertex[x+1][y]));
            }
            
            if (y < y_dim-1) {
                // std::cout << "Paired " << vertex[x][y] << " with " <<vertex[x][y+1] << std::endl;
                edges.push_back(std::make_pair(vertex[x][y], vertex[x][y+1]));
            }
        }
    }
}

void Cloth::init(mcl::Shader & shader) {
    shader.enable();

    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao);

    glGenBuffers(1, &ball_vbo);
    glGenBuffers(1, &ball_ibo);

    // The VBO containing the positions and sizes of the spheres representing a point mass
    glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
    glBufferData(GL_ARRAY_BUFFER, ball.getInterleavedVertexSize(), ball.getInterleavedVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  //Unbind the VAO so we don’t accidentally modify it

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ball.getIndexSize(), ball.getIndices(), GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  //Unbind the VAO so we don’t accidentally modify it
    
    glBindVertexArray(0);

    shader.disable();
}

void Cloth::update_physics(float dt) {
    size_t total_points = pointMasses.size();
    glm::vec3 forces [total_points];

    for(size_t i = 0; i < total_points; i++) forces[i] = glm::vec3(0.f);

    for ( const std::pair<size_t, size_t> &edge : edges )
    {
        glm::vec3 edge_vector = pointMasses[edge.first].position - pointMasses[edge.second].position;

        float string_length = glm::length(edge_vector); 
        float string_force = -k*(string_length - restLen);

        // std::cout << "String between" << edge.first << " - " << edge.second << " is length " << string_length; 
        // std::cout << "and produced a force " << string_force << std::endl;


        glm::vec3 damp_force = -kv * (pointMasses[edge.first].velocity - pointMasses[edge.second].velocity);

        glm::vec3 force_added = (string_force * glm::normalize(edge_vector) + damp_force) / 2.f;

        // If the cloth is not being held on to, apply the force
        forces[edge.first] += force_added;
        
        // Equal and opposite force
        forces[edge.second] -= force_added;
    }
 
    // After we've calculated all the forces at play, apply them;
    for(size_t i = 0; i < total_points; i++) {

        // A mass connected to itself is an anchor
        if ((i % _y_dim == 0 && i % 3 == 0) || (Globals::dragging_object && Globals::selected == &pointMasses[i])) {
            pointMasses[i].velocity = glm::vec3(0.f);
        } else {
            pointMasses[i].velocity += dt * (gravity + forces[i]) / mass;
            pointMasses[i].position += dt * pointMasses[i].velocity;
        }

        // if (pointMasses[i].position.y < ball.getRadius()) {
        //     pointMasses[i].position.y = ball.getRadius() + 0.01f;
        //     pointMasses[i].velocity.y *= -0.5; //bounce factor
        //     pointMasses[i].velocity *= 0.8; //bounce factor
        //     return;
        // }
    }
}

void Cloth::update(float dt) {
    size_t steps = 10; 
    for (size_t i = 0; i < steps; i++) {
        update_physics(dt / steps);
    }    
}

void Cloth::draw(mcl::Shader & shader) {
    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

	GLint attribUniformModel    = shader.uniform("model");
	GLint attribUniformNormal    = shader.uniform("normal");

    glBindVertexArray(vao);

    glVertexAttrib3f(attribVertexColor, 0.7, 0.2, 0.2);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);

    glm::mat4 ball_model;

    for (size_t i = 0; i < pointMasses.size(); i++) {

        ball_model = glm::translate(glm::mat4(1.0f), pointMasses[i].position);

        glm::mat4 matrix_ball_normal = ball_model;
        matrix_ball_normal[3] = glm::vec4(0,0,0,1);

        glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_ibo);

        glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(ball_model)  ); // model transformation
        glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_ball_normal)); // projection matrix

        int stride = ball.getInterleavedStride();
        glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, stride, (void*) 0);
        glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        // draw a sphere with VBO
        glDrawElements(GL_TRIANGLES,                    // primitive type
                    ball.getIndexCount(),          // # of indices
                    GL_UNSIGNED_INT,                 // data type
                    (void*)0);                       // offset to indices

    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
    glBindVertexArray(0);

    shader.disable();
}

Draggable * Cloth::find_draggable(glm::vec3 origin, glm::vec3 direction) {
    float radius = pow(ball.getRadius(), 2);
    for(size_t i = 0; i < pointMasses.size(); i++) {
        float distance; 
        if (glm::intersectRaySphere(origin, direction, pointMasses[i].position, radius, distance)) {
            std::cout << "Found point: " << i << std::endl;
            return &pointMasses[i];
        }
    }

    return nullptr;
}

void Cloth::cleanup() {

    glDeleteBuffers(1, &ball_vbo);
    glDeleteBuffers(1, &ball_ibo);
    glDeleteVertexArrays(1, &vao);
}