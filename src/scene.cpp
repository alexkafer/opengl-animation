#include "Scene.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene () {
    	// Initialize the shader (which uses glew, so we need to init that first).
        // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
        // the full path to this project's src/ directory.
        std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/phong.";
        shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

        particles = Particles();
	    particles.init();

        sphere = Sphere(0.75f, 36, 18);

        fire_hydrant = new Model("Firehydrant/", "firehydrant.obj", shader);
        camp_fire = new Model("campfire/", "Campfire.obj", shader);

        // std::stringstream fire_hydrant_ss; fire_hydrant_ss << MY_MODELS_DIR << "Firehydrant/";
        // fire_hydrant_size = load_model(fire_hydrant_vbo, fire_hydrant_ss.str() + "firehydrant.obj", fire_hydrant_ss.str());

        // std::stringstream camp_fire_ss; camp_fire_ss << MY_MODELS_DIR << "campfire/";
        // camp_fire_size = load_model(camp_fire_vbo, camp_fire_ss.str() + "Campfire.obj", camp_fire_ss.str());
        acceleration = glm::vec3(0, -9.8f, 0);
        position = glm::vec3(0, 10.0f, 0);
        velocity = glm::vec3(2.f, 0, 2.5f);

        floorPos = 0;
        wallPos = 5;
}

void Scene::print_stats() {
    particles.print_stats();
}

// Based off of https://frame.42yeah.casa/2019/12/10/model-loading.html
// GLuint Scene::load_model(GLuint & vbo, std::string obj_file, std::string mtl_dir) {
//     tinyobj::attrib_t attributes;
//     std::vector<tinyobj::shape_t> shapes;
//     std::vector<tinyobj::material_t> materials;

//     std::string warn;
//     std::string err;

//     bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, obj_file.c_str(), mtl_dir.c_str());

//     if (!warn.empty()) {
//         std::cout << warn << std::endl;
//     }

//     if (!err.empty()) {
//         std::cerr << err << std::endl;
//     }

//     if (!ret) {
//         exit(1);
//     }

//     std::vector<Vertex> vertices;
//     for (int i = 0; i < shapes.size(); i ++) {
//         tinyobj::shape_t &shape = shapes[i];
//         tinyobj::mesh_t &mesh = shape.mesh;
//         // we could visit the mesh index by using mesh.indices
//         for (int j = 0; j < mesh.indices.size(); j++) {
//             tinyobj::index_t i = mesh.indices[j];
//             glm::vec3 position = {
//                 attributes.vertices[i.vertex_index * 3],
//                 attributes.vertices[i.vertex_index * 3 + 1],
//                 attributes.vertices[i.vertex_index * 3 + 2]
//             };
//             glm::vec3 normal = {
//                 attributes.vertices[i.normal_index * 3],
//                 attributes.vertices[i.normal_index * 3 + 1],
//                 attributes.vertices[i.normal_index * 3 + 2]
//             };
//             glm::vec2 texCoord = {
//                 attributes.vertices[i.texcoord_index * 2],
//                 attributes.vertices[i.texcoord_index * 2 + 1],
//             };
//             // Not gonna care about texCoord right now.
//             Vertex vert = { position, normal, texCoord };
//             vertices.push_back(vert);
//         }
//     }

//     // std::vector<Vertex> vertices;
//     for (const auto& material : materials) {
//         std::cout << "material name: " << material.name << "asking for: " << material.diffuse_texname  << std::endl;
//     }

//     glGenBuffers(1, &vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

//     return vertices.size();
// }

glm::vec3 Scene::get_ball_position() {
    return position;
}

void Scene::add_ball_velocity(glm::vec3 v) {
    velocity += v;
}


void Scene::compute_physics(float dt){
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
    
    particles.update(dt, position, sphere.getRadius());
}

void Scene::check_collisions() {
    if (position.y - sphere.getRadius() < floorPos){
        position.y = floorPos + sphere.getRadius();
        velocity.y *= -.95;
        particles.spawn(impact, glm::vec3(position.x, floorPos, position.z), 0.3f * velocity.y * up, sphere.getRadius(), false);
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

    particles.spawn(water, glm::vec3(0.35f, 1.2f, 0.f), glm::vec3(10.f, 0.f, 0.f), 0.075f, true);
    // particles.spawn(fire, glm::vec3(0.f, 2.f, 0.f), 5.f * up, 0.075f, false);
}

void Scene::init_floor() {
    float vertices[] = {
        // X      Y     Z     R     G      B      U      V
        -5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
        5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 1.0f, 1.0f,
        5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        -5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 0.0f, 0.0f,
        -5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
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

void Scene::init()
{
    shader.enable();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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
    init_static_uniforms();

    shader.disable();
}

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Scene::init_static_uniforms()
{
    GLint uniformLightPosition             = shader.uniform("lightPosition");
    // GLint uniformLightAmbient              = shader.uniform("lightAmbient");
    // GLint uniformLightDiffuse              = shader.uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader.uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    
    // set uniform values
    float lightPosition[]  = {4.f, 3.0f, 0.f, 1.0f};
    // float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    // float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.4f, 0.4f, 0.4f, 1};
    float materialDiffuse[]  = {0.5f, 0.5f, 0.5f, 1};
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

// void Scene::draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size) {
//     GLint attribVertexPosition  = shader.attribute("in_position");
// 	GLint attribVertexNormal    = shader.attribute("in_normal");
//     GLint attribVertexColor     = shader.attribute("in_color");
// 	// GLint attribVertexTexture    = shader.attribute("in_texture");

//     glVertexAttrib3f(attribVertexColor, 0.5, 0.2, 0.2);

//     // activate attribs
//     glEnableVertexAttribArray(attribVertexPosition);
//     glEnableVertexAttribArray(attribVertexNormal);
//     // glEnableVertexAttribArray(attribVertexTexture)

//     // glm::mat4 matrix_normal = glm::mat4(0.1f);
//     // matrix_normal[3] = glm::vec4(0,0,0,1);

//     glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(matrix_model)); // model transformation
//     glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_model)); // projection matrix
		
//     // set attrib arrays using glVertexAttribPointer()
//     // bind vbo for floor
//     glBindBuffer(GL_ARRAY_BUFFER, model_vao);
//     glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
//     glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
//     // glVertexAttribPointer(attribVertexTexture, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));

//     // draw a sphere with VBO
//     glDrawArrays(GL_TRIANGLES, 0, model_size); //(Primitives, Which VBO, Number of vertices)

//     glDisableVertexAttribArray(attribVertexPosition);
//     glDisableVertexAttribArray(attribVertexNormal);
//     // glDisableVertexAttribArray(attribVertexTexture);

//     // glUniform1i(shader.uniform("texture_map"), 0);
//     // glUniform1i(shader.uniform("texture_used"), 1);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

void Scene::draw_ball(float dt) {
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

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

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(ball_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_ball_normal)); // projection matrix
		
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

void Scene::draw_floor() {
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexColor);
    glEnableVertexAttribArray(attribVertexNormal);

    glm::mat4 scale_model = glm::scale(  // Scale first
        glm::mat4( 1.0f ),              // Translate second
        glm::vec3( 10.0f, 10.0f, 10.0f )
    );

    // glm::mat4 matrix_normal = glm::mat4(1.0f);
    // matrix_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(scale_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(scale_model)); // projection matrix
		
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

void Scene::draw(float dt) {

    shader.enable();
    glBindVertexArray(vao); // Bind the globally created VAO to the current context

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
		

    draw_ball(dt);
    draw_floor();

    

    glm::mat4 fire_hydrant_model = glm::scale(  // Scale first
        glm::mat4( 1.0f ),              // Translate second
        glm::vec3( 0.5f, 0.5f, 0.5f )
    );

    glm::mat4 camp_fire_translate = glm::translate(
        glm::mat4( 1.0f ),
        glm::vec3( -2.0f, -0.1f, -4.0f )
    );

    glm::mat4 camp_fire_model = glm::scale(  // Scale first
        camp_fire_translate,              // Translate second
        glm::vec3( 0.2f, 0.2f, 0.2f )
    );
    
    glm::mat4 fire_hydrant_normal = fire_hydrant_model;
    fire_hydrant_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(fire_hydrant_model)  );
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(fire_hydrant_normal)); // projection matrix
    fire_hydrant->draw(shader);

    glm::mat4 camp_fire_normal = camp_fire_model;
    camp_fire_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(camp_fire_model)  );
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(camp_fire_normal)); // projection matrix
    camp_fire->draw(shader);
    // draw_model(fire_hydrant_model, fire_hydrant_vbo, fire_hydrant_size);
    // draw_model(camp_fire_model, camp_fire_vbo, camp_fire_size);


	particles.draw();

    glBindVertexArray(0);

    compute_physics(dt);

    check_collisions();
}

void Scene::cleanup() {
    // Disable the shader, we're done using it
	shader.disable();

    glDeleteBuffers(1, &ball_vbo);
	glDeleteBuffers(1, &ball_ibo);

	glDeleteVertexArrays(1, &vao);
}