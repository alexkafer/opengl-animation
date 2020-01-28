#include "scene.hpp"

#include <iostream>
#include <glm/vec4.hpp> 
#include <glm/gtc/type_ptr.hpp>

Sphere sphere(1.f, 36, 18);

float position = 0; 
float velocity = 5;
float floorPos = -10;

void computePhysics(float dt){
  float acceleration = -10;//-9.8;
  velocity = velocity + acceleration * dt;
  position = position + velocity * dt;
  printf("pos: %.2f, vel: %.2f\n",position, velocity);
  if (position - radius < floorPos){
    position = floorPos + radius;
    velocity *= -.95;
  }
}

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void init_static_uniforms(mcl::Shader * shader)
{
    GLint uniformLightPosition             = shader->uniform("lightPosition");
    GLint uniformLightAmbient              = shader->uniform("lightAmbient");
    GLint uniformLightDiffuse              = shader->uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader->uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader->uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader->uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader->uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader->uniform("materialShininess");
    
    // set uniform values
    float lightPosition[]  = {1.5f, 0.0f, 1.5f, 1.0f};
    float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.5f, 0.5f, 0.5f, 1};
    float materialDiffuse[]  = {0.7f, 0.7f, 0.7f, 1};
    float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
    float materialShininess  = 4;

    glUniform4fv(uniformLightPosition, 1, lightPosition);
    glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    glUniform1f(uniformMaterialShininess, materialShininess);
}

void init_geometry(mcl::Shader * shader, GLuint & vbo, GLuint & ibo, GLuint & vao)
{

	glGenBuffers(1, &vbo); //Create buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //(Only one buffer can be bound at a time)
	glBufferData(GL_ARRAY_BUFFER, 
        sphere.getInterleavedVertexSize(), 
        sphere.getInterleavedVertices(), 
        GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind the VAO so we don’t accidentally modify it

    // If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.

    // Index Data
    glGenBuffers(1, &ibo); //Create index buffer called ibo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  //Unbind the VAO so we don’t accidentally modify it
   
    glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context
}

void draw_scene(mcl::Shader * shader, GLuint &vbo, GLuint &ibo) {

    GLint attribVertexPosition  = shader->attribute("in_position");
	GLint attribVertexNormal    = shader->attribute("in_normal");

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);

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
}