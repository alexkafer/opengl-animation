#include "cloth.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static glm::vec3 gravity(0.f, -9.8f, 0.f);

float restLen = .1f;
float k = 500.f; //TRY-IT: How does changing k affect resting length?
float kv = 10.f;

Cloth::Cloth(size_t points) {

    pointMasses = std::vector<PointMass>();
    pointMasses.reserve(points);

    ball = Sphere(0.1f, 36, 18);

    pointMasses.push_back(PointMass(glm::vec3(2.f * (float) rand() / RAND_MAX - 1.f, (points + 1.f) / 5.f, 2.f * (float) rand() / RAND_MAX - 1.f), 0));

    for (size_t i = 1; i <= points; i++) {
        Sphere * sphere = new Sphere(0.1f, 36, 18);

        pointMasses.push_back(PointMass(glm::vec3(0.f, ((float) points - i + 1.f) / 5.f, 0.f), i-1));
    }

    glEnable(GL_PROGRAM_POINT_SIZE); 

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

void Cloth::update(float dt) {

    size_t points = pointMasses.size();
    glm::vec3 forces [points];

    for(size_t i = 0; i < points; i++) forces[i] = glm::vec3(0.f);
 
    for(size_t i = 0; i < points; i++) {
        size_t parent = pointMasses[i].connected_to;

        if (parent == i) continue;

        glm::vec3 edge = pointMasses[i].position - pointMasses[parent].position;

        float string_length = glm::length(edge); 
        float string_force = -k*(string_length - restLen);

        glm::vec3 damp_force = -kv * (pointMasses[i].velocity - pointMasses[parent].velocity);

        forces[i] += (string_force * glm::normalize(edge) + damp_force);

        // Equal and opposite force
        forces[parent] -= forces[i];
    }

    // After we've calculated all the forces at play, apply them;
    for(size_t i = 0; i < points; i++) {
        // A mass connected to itself is an anchor
        if (pointMasses[i].connected_to == i) continue;

        pointMasses[i].velocity += dt * (gravity + forces[i]);
        pointMasses[i].position += dt * pointMasses[i].velocity;

        std::cout << "Position " << i << " " << glm::to_string(pointMasses[i].position) << std::endl;

        if (pointMasses[i].position.y < 0) {
            pointMasses[i].position.y = 0;
            pointMasses[i].velocity.x += 2.f * (((float) rand() / RAND_MAX) - 0.5f);
            pointMasses[i].velocity.y *= -0.5; //bounce factor
            pointMasses[i].velocity.z += 2.f * (float) rand() / RAND_MAX - 1.f;
            pointMasses[i].velocity *= 0.8; //bounce factor
            return;
        }
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

void Cloth::cleanup() {

    glDeleteBuffers(1, &ball_vbo);
    glDeleteBuffers(1, &ball_ibo);
    glDeleteVertexArrays(1, &vao);
}