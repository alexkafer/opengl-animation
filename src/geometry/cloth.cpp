#include "cloth.h"

#include <math.h> 
#include <iostream>
#include <utility>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static glm::vec3 gravity(0.f, -9.8f, 0.f);
// static glm::vec3 gravity(0.f, -1.f, 0.f);

static const float STEPS = 10;
static const float REST_LENGTH = .2f;
// static const float K = 500.f; 
static const float K = 300.f; 
static const float KV = 3.f;
static const float MASS = 0.5f;

static const int GRID_SIZE = 2;
static const int HASH_TABLE_SIZE = 599;
static const int PRIMES[3] = {73856093, 19349663, 83492791};

static size_t spatial_hashing_func(const glm::vec3 & key) {
    return (
          PRIMES[0] * (int) floor(key.x / GRID_SIZE) ^ 
          PRIMES[1] * (int) floor(key.y / GRID_SIZE) ^ 
          PRIMES[2] * (int) floor(key.z / GRID_SIZE)
    ) % HASH_TABLE_SIZE;
}

Cloth::Cloth(size_t x_dim, size_t y_dim) {
    _x_dim = x_dim;
    _y_dim = y_dim;

    pointMasses = std::vector<PointMass>();
    pointMasses.reserve(x_dim * y_dim);

    forces = std::vector<glm::vec3>();
    forces.reserve(x_dim*y_dim);

    edges = std::vector<std::pair<size_t, size_t>>();
    size_t edge_count = (x_dim-1)*y_dim + (y_dim-1)*x_dim;
    edges.reserve(edge_count);

    ball = Sphere(2 * REST_LENGTH, 36, 18);

    // spatial_hash = spatial_hash_map(HASH_TABLE_SIZE); //, spatial_hashing_func);

    // pointMasses.push_back(PointMass(glm::vec3(2.f * (float) rand() / RAND_MAX - 1.f, (points + 15.f) / 5.f, 2.f * (float) rand() / RAND_MAX - 1.f), 0));
    size_t vertex[x_dim][y_dim];

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            vertex[x][y] = pointMasses.size();
            // std::cout << "Generated " << vertex[x][y] << " at (" << x << ", "<< y << ")" << std::endl;
            pointMasses.push_back(PointMass(glm::vec3(x * REST_LENGTH, REST_LENGTH * (y_dim - y) + 5.f, 0.f)));
            forces.push_back(glm::vec3(0.0f));
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

void Cloth::update_forces(float dt) {
    size_t total_points = pointMasses.size();
    
    // Clear forces before we get started this round
    for(size_t i = 0; i < total_points; i++) forces[i] = gravity;

    for ( const std::pair<size_t, size_t> &edge : edges )
    {
        glm::vec3 edge_vector = pointMasses[edge.first].position - pointMasses[edge.second].position;

        float string_length = glm::length(edge_vector); 
        float string_force = -K * STEPS *(string_length - (REST_LENGTH));

        // std::cout << "String between" << edge.first << " - " << edge.second << " is length " << string_length; 
        // std::cout << "and produced a force " << string_force << std::endl;


        glm::vec3 damp_force = -KV * STEPS * (pointMasses[edge.first].velocity - pointMasses[edge.second].velocity);

        if (edge.first == Globals::selected || edge.second == Globals::selected) {
            string_force *= 10.f;
            damp_force = glm::vec3(0.f);
        }

        glm::vec3 force_added = (string_force * glm::normalize(edge_vector) + damp_force) / 2.f;

        // If the cloth is not being held on to, apply the force
        forces[edge.first] += force_added;
        
        // Equal and opposite force
        forces[edge.second] -= force_added;
    }
 
    // After we've calculated all the forces at play, apply them to the velocity;
    for(size_t i = 0; i < total_points; i++) {
        //  if (i == _y_dim-1) {
        //     std::cout << "Position: " << glm::to_string(pointMasses[i].position) << std::endl;
        //     std::cout << "Velocity: " << glm::to_string(pointMasses[i].velocity) << std::endl;
        //     std::cout << "Forces: " << glm::to_string(forces[i]) << std::endl;
        //     std::cout << "----- " << std::endl;
        // }

        // 
        if ((i % _y_dim == 0 && i % 3 == 0) || (Globals::dragging_object && Globals::selected == i)) {
            // Kill all forces for an object being held
            forces[i] = glm::vec3(0.f);
        }
    }
}

void Cloth::update_positions(float dt) {
    // After we've calculated all the velocities and made modifications, apply them;
    for(size_t i = 0; i < pointMasses.size(); i++) {
        pointMasses[i].velocity += dt * forces[i] / MASS;
        pointMasses[i].position += dt * pointMasses[i].velocity;   
    }
}

void Cloth::check_collisions() {
    // This code is my own implementation of Matthias Teschner's collision detection algorithm
    // Optimized Spatial Hashing for Collision Detection of Deformable Objects
    // https://matthias-research.github.io/pages/publications/tetraederCollision.pdf

    // std::pair <spatial_hash_map::iterator, spatial_hash_map::iterator> ret;
    // spatial_hash.clear();

    // // Populate the hash table
    // for(size_t i = 0; i < pointMasses.size(); i++) {
    //     spatial_hash.insert(std::make_pair<size_t, size_t>(spatial_hashing_func(pointMasses[i].position), (size_t) i));
    // }

    // Check for collisions
    // printUmm(spatial_hash);

    for(size_t i = 0; i < pointMasses.size(); i++) {
        
        // ret = spatial_hash.equal_range(spatial_hashing_func(pointMasses[i].position));

        // std::cout << "near " << i << " => ";
        // for (spatial_hash_map::iterator it=ret.first; it!=ret.second; ++it) {
        // for(size_t nearby_point_index = 0; nearby_point_index < pointMasses.size(); nearby_point_index++) {
        //     // std::cout << ' ' << it->second;

        //     // size_t nearby_point_index = it->second;

        //     if (nearby_point_index == i) continue;

        //     glm::vec3 diff = pointMasses[i].position - pointMasses[nearby_point_index].position;

        //     // std::cout << "Distance between " << i << " and " << nearby_point_index << " is " << glm::length(diff) << std::endl;
        //     float len = glm::length(diff);
        //     if (len < (REST_LENGTH)) {
        //         // std::cout << "We have a collision " << len << " between " << i << " and " << nearby_point_index <<std::endl;
        //         // diff /= len;

        //         // glm::vec3 nv1 = pointMasses[i].velocity; // new velocity for sphere 1
        //         // glm::vec3 nv2 = pointMasses[nearby_point_index].velocity; // new velocity for sphere 2
                
        //         // std::cout << "Initial Velocities: " << glm::to_string(nv1) << " and " << glm::to_string(nv2) <<std::endl;

        //         // nv1 += glm::proj(pointMasses[nearby_point_index].velocity, -1.f * diff);
        //         // nv1 -= glm::proj(pointMasses[i].velocity, diff);
                
        //         // nv2 += glm::proj(pointMasses[i].velocity, -1.f * diff);
        //         // nv2 -= glm::proj(pointMasses[nearby_point_index].velocity, diff);

        //         // std::cout << "Final Velocities: " << glm::to_string(nv1) << " and " << glm::to_string(nv2) <<std::endl;
        //         // pointMasses[i].velocity = nv1;
        //         // pointMasses[nearby_point_index].velocity = nv2;

        //         // forces[i] = glm::vec3(0.f);
        //         // forces[nearby_point_index] = glm::vec3(0.f);

        //         // pointMasses[i].velocity = glm::vec3(0.f);
        //         // pointMasses[nearby_point_index].velocity = glm::vec3(0.f);
        //         // std::cout << "Initial Positions: " << glm::to_string(pointMasses[i].position) << " and " << glm::to_string(pointMasses[nearby_point_index].position) <<std::endl;
        //         // std::cout << " diff: " << glm::to_string(diff) <<std::endl;

        //         pointMasses[i].position = pointMasses[nearby_point_index].position + (REST_LENGTH) * (diff / len);

        //         // std::cout << "Final Positions: " << glm::to_string(pointMasses[i].position) << " and " << glm::to_string(pointMasses[nearby_point_index].position) <<std::endl;
                
        //     }
        // }

        // std::cout << '\n';



        if (pointMasses[i].position.y < ball.getRadius()) {
            // if (i == _y_dim-1) {
                // std::cout << "COLLISION!" << glm::to_string(pointMasses[i].position) << std::endl;
            // }
            // pointMasses[i].position.y = ball.getRadius(); 
            // pointMasses[i].position.y = 0.1f;
            pointMasses[i].velocity.y *= -0.8; //bounce factor
            pointMasses[i].velocity.x *= 0.8; //bounce factor
            pointMasses[i].velocity.z *= 0.8; //bounce factor
        }
    }
}

void Cloth::update(float dt) {
    for (size_t i = 0; i < STEPS; i++) {
        update_forces(dt / STEPS);
        update_positions(dt / STEPS);
        check_collisions();
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

int Cloth::find_object(glm::vec3 origin, glm::vec3 direction) {
    float radius = pow(ball.getRadius(), 2);
    for(int i = 0; i < pointMasses.size(); i++) {
        float distance; 
        if (glm::intersectRaySphere(origin, direction, pointMasses[i].position, radius, distance)) {
            std::cout << "Found point: " << i << std::endl;
            return i;
        }
    }

    return -1;
}

void Cloth::drag_object(int object, glm::vec3 direction) {
    // std::pair <spatial_hash_map::iterator, spatial_hash_map::iterator> ret;

    // ret = spatial_hash.equal_range(spatial_hashing_func(pointMasses[object].position));
    
    // for (spatial_hash_map::iterator it=ret.first; it!=ret.second; ++it) {
        float distance_from_camera = glm::distance(Globals::eye_pos, pointMasses[object].position);
        // std::cout << "Point at " << glm::to_string(pointMasses[object].position) << " is " << distance_from_camera << " units from the camera" << std::endl;
        pointMasses[object].position = Globals::eye_pos + distance_from_camera * direction;   
    // }
}

void Cloth::cleanup() {

    glDeleteBuffers(1, &ball_vbo);
    glDeleteBuffers(1, &ball_ibo);
    glDeleteVertexArrays(1, &vao);
}