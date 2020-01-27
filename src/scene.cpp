#include "scene.hpp"

#include <iostream>
#include <glm/vec4.hpp> 
#include <glm/gtc/type_ptr.hpp>

Sphere sphere(1.f, 36, 18);


///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void init_uniforms(mcl::Shader * shader)
{
    GLint uniformLightAmbient              = shader->uniform("lightAmbient");
    GLint uniformLightDiffuse              = shader->uniform("lightDiffuse");
    // GLint uniformLightSpecular             = shader->uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader->uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader->uniform("materialDiffuse");
    // GLint uniformMaterialSpecular          = shader->uniform("materialSpecular");
    // GLint uniformMaterialShininess         = shader->uniform("materialShininess");
    
    // set uniform values
    float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    // float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.5f, 0.5f, 0.5f, 1};
    float materialDiffuse[]  = {0.7f, 0.7f, 0.7f, 1};
    // float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
    // float materialShininess  = 16;

    glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    // glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    // glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    // glUniform1f(uniformMaterialShininess, materialShininess);
}

void init_geometry(mcl::Shader * shader, GLuint * vbo, GLuint & vao)
{

	glGenBuffers(2, vbo); //Create 2 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //(Only one buffer can be bound at a time)
    
    sphere.printSelf();
	glBufferData(GL_ARRAY_BUFFER, 
        sphere.getInterleavedVertexSize(), 
        sphere.getInterleavedVertices(), 
        GL_STATIC_DRAW); 

    // If data is changing infrequently GL DYNAMIC DRAW may be better, 
	// and GL STREAM DRAW is best used when the data changes frequently.

    // Index Data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.getIndexSize(), sphere.getIndices(), GL_STATIC_DRAW); 
    //upload normals to vbo

	glGenVertexArrays(1, &vao); //Create a VAO
	glBindVertexArray(vao); // Bind the globally created VAO to the current context


	// Sphere
    int stride = sphere.getInterleavedStride();
	GLint posAttrib = shader->attribute("in_position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	//(above params: Attribute, vals/attrib., type, isNormalized, stride, offset)
	glEnableVertexAttribArray(posAttrib); //Mark the attribute’s location as valid

    GLint normAttrib  = shader->attribute("in_normal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*6));
	glEnableVertexAttribArray(normAttrib);

	//Unbind the VAO so we don’t accidentally modify it
	glBindVertexArray(0); 
}

void draw_scene() {

    // draw a sphere with VBO
    glDrawElements(GL_TRIANGLES,                    // primitive type
                sphere.getIndexCount(),          // # of indices
                GL_UNSIGNED_INT,                 // data type
                (void*)0);                       // offset to indices

}
