#include "Scene.hpp"

#include <iostream>
#include <glm/vec4.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

Scene::Scene () {
        sphere = Sphere(1.f, 36, 18);

        acceleration = glm::vec3(0, -9.8f, 0);
        position = glm::vec3(0, 10.0f, 0);
        velocity = glm::vec3(2.f, 0, 2.5f);

        floorPos = 0;
        wallPos = 5;
}

glm::vec3 Scene::get_ball_position() {
    return position;
}

void Scene::add_ball_velocity(glm::vec3 v) {
    velocity += v;
}


void Scene::computePhysics(float dt){
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
    std::cout << "pos: " << glm::to_string(position) << ", vel: " << glm::to_string(velocity) << std::endl;
    
    if (position.y - sphere.getRadius() < floorPos){
        position.y = floorPos + sphere.getRadius();
        velocity.y *= -.95;
    }

    // Positive X wall
    if (position.x + sphere.getRadius() > wallPos){
        position.x = wallPos - sphere.getRadius();
        velocity.x *= -.95;
    }

    // Negative X wall
    if (position.x - sphere.getRadius() < -wallPos){
        position.x = -wallPos + sphere.getRadius();
        velocity.x *= -.95;
    }

    // Positive Z wall
    if (position.z + sphere.getRadius() > wallPos){
        position.z = wallPos - sphere.getRadius();
        velocity.z *= -.95;
    }

    // Negative Z wall
    if (position.z - sphere.getRadius() < -wallPos){
        position.z = -wallPos + sphere.getRadius();
        velocity.z *= -.95;
    }
}

void Scene::init_floor() {
    float vertices[] = {
        // X      Y     Z     R     G      B      U      V
        -5.f, 0.f, -5.f, 0.2f, 0.2f, 0.8f, 0.0f, 1.0f,
        5.f, 0.f, -5.f, 0.2f, 0.2f, 0.8f, 1.0f, 1.0f,
        5.f, 0.f,  5.f, 0.2f, 0.2f, 0.8f, 1.0f, 0.0f,
        5.f, 0.f,  5.f, 0.2f, 0.2f, 0.8f, 1.0f, 0.0f,
        -5.f, 0.f,  5.f, 0.2f, 0.2f, 0.8f, 0.0f, 0.0f,
        -5.f, 0.f, -5.f, 0.2f, 0.2f, 0.8f, 0.0f, 1.0f,
    };

    float normals[] = {
        0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,
    };

	glGenBuffers(2, floor_vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //upload vertices to vbo

    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW); //upload normals to vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//  Model, view and projection matrices, initialized to the identity
glm::mat4 ball_model;

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Scene::init_static_uniforms(mcl::Shader * shader)
{
    GLint uniformLightPosition             = shader->uniform("lightPosition");
    // GLint uniformLightAmbient              = shader->uniform("lightAmbient");
    // GLint uniformLightDiffuse              = shader->uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader->uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader->uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader->uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader->uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader->uniform("materialShininess");
    
    // set uniform values
    float lightPosition[]  = {4.f, 2.0f, 4.f, 1.0f};
    // float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    // float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.5f, 0.5f, 0.5f, 1};
    float materialDiffuse[]  = {0.7f, 0.7f, 0.7f, 1};
    float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
    float materialShininess  = 4;

    glUniform4fv(uniformLightPosition, 1, lightPosition);
    // glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    // glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    glUniform1f(uniformMaterialShininess, materialShininess);
}

void Scene::init_geometry(mcl::Shader * shader, GLuint & vao)
{

	glGenBuffers(1, &ball_vbo); //Create buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo); //(Only one buffer can be bound at a time)
	glBufferData(GL_ARRAY_BUFFER, 
        sphere.getInterleavedVertexSize(), 
        sphere.getInterleavedVertices(), 
        GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind the VAO so we don’t accidentally modify it

    // If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.

    // Index Data
    glGenBuffers(1, &ball_ibo); //Create index buffer called ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  //Unbind the VAO so we don’t accidentally modify it

    init_floor();
   
    glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context
}

void Scene::draw_ball(mcl::Shader * shader, float dt) {
    GLint attribVertexPosition  = shader->attribute("in_position");
    GLint attribVertexColor     = shader->attribute("in_color");
	GLint attribVertexNormal    = shader->attribute("in_normal");

    glVertexAttrib3f(attribVertexColor, 0.7, 0.2, 0.2);

    ball_model = glm::translate(glm::mat4(1.0f), position);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);

    // bind vbo for ball
    glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_ibo);

    glm::mat4 matrix_ball_normal = ball_model;
    matrix_ball_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader->uniform("model"), 1, GL_FALSE, glm::value_ptr(ball_model)  ); // model transformation
    glUniformMatrix4fv( shader->uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_ball_normal)); // projection matrix
		
    // set attrib arrays using glVertexAttribPointer()
    int stride = sphere.getInterleavedStride();
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, stride, (void*) 0);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
   
    // draw a sphere with VBO
    glDrawElements(GL_TRIANGLES,                    // primitive type
                sphere.getIndexCount(),          // # of indices
                GL_UNSIGNED_INT,                 // data type
                (void*)0);                       // offset to indices

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Scene::draw_floor(mcl::Shader * shader) {
    GLint attribVertexPosition  = shader->attribute("in_position");
    GLint attribVertexColor     = shader->attribute("in_color");
	GLint attribVertexNormal    = shader->attribute("in_normal");

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexColor);
    glEnableVertexAttribArray(attribVertexNormal);


    glm::mat4 matrix_normal = glm::mat4(1.0f);
    matrix_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader->uniform("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))  ); // model transformation
    glUniformMatrix4fv( shader->uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_normal)); // projection matrix
		
    // set attrib arrays using glVertexAttribPointer()
    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[0]);
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glVertexAttribPointer(attribVertexColor, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[1]);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // draw a sphere with VBO
    glDrawArrays(GL_TRIANGLES, 0, 6); //(Primitives, Which VBO, Number of vertices)

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexColor);
    glDisableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Scene::draw(mcl::Shader * shader, float dt) {

    draw_ball(shader, dt);
    draw_floor(shader);

    computePhysics(dt);
}

void Scene::cleanup() {
    glDeleteBuffers(1, &ball_vbo);
	glDeleteBuffers(1, &ball_ibo);
}