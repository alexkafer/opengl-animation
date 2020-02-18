#include "cloth.h"

#include <math.h> 
#include <iostream>
#include <utility>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>


static glm::vec3 gravity(0.f, -9.8f, 0.f);
// static glm::vec3 gravity(0.f, -1.f, 0.f);

static const float STEPS = 10;
static const float REST_LENGTH = 0.5f;
// static const float K = 500.f; 
static const float K = 100.f; 
static const float KV = 5.f;
static const float MASS = 0.5f;

static const float GRID_VOLUME = 0.5f;
static const int HASH_TABLE_SIZE = 599;
static const int PRIMES[3] = {73856093, 19349663, 83492791};

static size_t spatial_hashing_func(const glm::vec3 & key) {
    return (
          PRIMES[0] * (int) floor(key.x / GRID_VOLUME) ^ 
          PRIMES[1] * (int) floor(key.y / GRID_VOLUME) ^ 
          PRIMES[2] * (int) floor(key.z / GRID_VOLUME)
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

    // spatial_hash = spatial_hash_map(HASH_TABLE_SIZE); //, spatial_hashing_func);

    // pointMasses.push_back(PointMass(glm::vec3(2.f * (float) rand() / RAND_MAX - 1.f, (points + 15.f) / 5.f, 2.f * (float) rand() / RAND_MAX - 1.f), 0));
    size_t vertex[x_dim][y_dim];

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            vertex[x][y] = pointMasses.size();
            // std::cout << "Generated " << vertex[x][y] << " at (" << x << ", "<< y << ")" << std::endl;
            pointMasses.push_back(PointMass(glm::vec3(x * REST_LENGTH, 5.f, REST_LENGTH * y)));
            forces.push_back(glm::vec3(0.0f));
        }
    }


    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            
            // If not on the x edge
            if (x < x_dim-1) {
                // std::cout << "Paired " << vertex[x][y] << " with " <<vertex[x+1][y] << std::endl;
                edges.push_back(std::make_pair(vertex[x][y], vertex[x+1][y]));
            }
            
            // If not on the bottom row
            if (y < y_dim-1) {
                // std::cout << "Paired " << vertex[x][y] << " with " <<vertex[x][y+1] << std::endl;
                edges.push_back(std::make_pair(vertex[x][y], vertex[x][y+1]));
            }

            // Left down triangle
            if (x < x_dim-1 && y < y_dim-1) {
                indices.push_back(vertex[x][y]);
                indices.push_back(vertex[x+1][y]);
                indices.push_back(vertex[x][y+1]);

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x+1][y] << ", " <<vertex[x][y+1] << std::endl;
            }

            // Up right triangle
            if (x > 0 && y > 0) {
                indices.push_back(vertex[x][y]);
                indices.push_back(vertex[x-1][y]);
                indices.push_back(vertex[x][y-1]);

                // std::cout << "Made triangle " << vertex[x][y] << ", " <<vertex[x-1][y] << ", " <<vertex[x][y-1] << std::endl;
            
            }
        }
    }
}

void Cloth::init(mcl::Shader & shader) {
    shader.enable();

    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, pointMasses.size() * sizeof(PointMass), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  
    glBindVertexArray(0);

    shader.disable();
}

// Algorithm adapted from TinyObj loader
void Cloth::compute_normals() {
    for(size_t i = 0; i < pointMasses.size(); i++) {
        pointMasses[i].normal = glm::vec3(0.f);
    }

    for (size_t f = 0; f < indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        GLushort idx0 = indices[3 * f + 0];
        GLushort idx1 = indices[3 * f + 1];
        GLushort idx2 = indices[3 * f + 2];

        glm::vec3 normal = glm::triangleNormal(pointMasses[idx0].position, pointMasses[idx1].position, pointMasses[idx2].position);

        pointMasses[idx0].normal += normal;
        pointMasses[idx1].normal += normal;
        pointMasses[idx2].normal += normal;

    } 

    for(size_t i = 0; i < pointMasses.size(); i++) {
        pointMasses[i].normal = glm::normalize(pointMasses[i].normal);
    }
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

        edge_vector /= string_length; // Normalize

        float velocity_one = glm::dot(pointMasses[edge.first].velocity, edge_vector);
        float velocity_two = glm::dot(pointMasses[edge.second].velocity, edge_vector);

     

        float damp_force = -KV * STEPS * (velocity_one - velocity_two);

        if (edge.first == Globals::selected || edge.second == Globals::selected) {
            string_force *= 10.f;
        }

        glm::vec3 force_added = ((string_force + damp_force) * glm::normalize(edge_vector)) / 2.f;

        // If the cloth is not being held on to, apply the force
        forces[edge.first] += force_added;
        
        // Equal and opposite force
        forces[edge.second] -= force_added;
    }
 
    // After we've calculated all the forces at play, apply them to the velocity;
    for(size_t i = 0; i < total_points; i++) {
        
    }
}

void Cloth::update_positions(float dt) {
    spatial_hash.clear(); // Clear the hash since we're calculating new positions

    // After we've calculated all the velocities and made modifications, apply them;
    for(size_t i = 0; i < pointMasses.size(); i++) {
        
        if ((i % _y_dim == (_y_dim / 2)) || (i % _y_dim == (_y_dim / 2) + 1)|| (Globals::dragging_object && Globals::selected == i)) {
            // Kill all forces for an object being held
            forces[i] = glm::vec3(0.f);
            pointMasses[i].velocity = glm::vec3(0.f);
        } else {
            pointMasses[i].velocity += dt * forces[i] / MASS;
            pointMasses[i].position += dt * pointMasses[i].velocity;  
        }

        spatial_hash.insert(std::make_pair<size_t, size_t>(spatial_hashing_func(pointMasses[i].position), (size_t) i));
    }
}

void Cloth::check_collisions(float dt) {
    // This code is my own implementation of Matthias Teschner's collision detection algorithm
    // Optimized Spatial Hashing for Collision Detection of Deformable Objects
    // https://matthias-research.github.io/pages/publications/tetraederCollision.pdf

    std::pair <spatial_hash_map::iterator, spatial_hash_map::iterator> ret;
    // spatial_hash.clear();

    // // Populate the hash table
    // for(size_t i = 0; i < pointMasses.size(); i++) {
    //     spatial_hash.insert(std::make_pair<size_t, size_t>(spatial_hashing_func(pointMasses[i].position), (size_t) i));
    // }

    // Check for collisions
    // printUmm(spatial_hash);

    for (size_t f = 0; f < indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        PointMass * triangle_points [3];

        for (size_t i = 0; i < 3; i++) {
            triangle_points[i] = &pointMasses[indices[3 * f + i]];

            if (triangle_points[i]->position.y < 0) {
                triangle_points[i]->position.y = 0.01f; 
                triangle_points[i]->velocity.y *= -0.5; //bounce factor
                triangle_points[i]->velocity.x *= 0.95; //bounce factor
                triangle_points[i]->velocity.z *= 0.95; //bounce factor
            }
        }
        
        // ret = spatial_hash.equal_range(spatial_hashing_func(triangle_points[0]->position));

        // for (spatial_hash_map::iterator it=ret.first; it!=ret.second; ++it) {
        //     PointMass * point = &pointMasses[it->second];

        for (PointMass & mass : pointMasses) {
            PointMass * point = &mass;
            // Make sure the point isn't a member of the triangle
            if (point == triangle_points[0] || point == triangle_points[1] || point == triangle_points[2]) continue;

            glm::vec3 direction = glm::normalize(point->velocity);

            // ray collision
            glm::vec2 barycentric_position; 
            float distance;
            if (glm::intersectRayTriangle(point->position, direction, 
                    triangle_points[0]->position, 
                    triangle_points[1]->position, 
                    triangle_points[2]->position, barycentric_position, distance)) {

                // std::cout << "Found " << distance << " I expect it to travel " << glm::length(dt * point->velocity) << std::endl;

                if (distance < 0 || distance > glm::length(dt * point->velocity)) continue;
                
                std::cout << "Detected to " << distance << " intersection at " << glm::to_string(point->position) << " traveling " << glm::to_string(point->velocity) << std::endl;
                
                // Triangle bounce
                glm::vec3 triangle_normal = glm::normalize(
                    triangle_points[0]->normal 
                    + triangle_points[1]->normal 
                    + triangle_points[2]->normal
                );

                float bounce = 2.9f * glm::dot(point->velocity, triangle_normal);
                point->velocity -= (bounce * triangle_normal);

                glm::vec3 opp_velocity = (bounce * triangle_normal) / 3.f;

                triangle_points[0]->velocity += opp_velocity;
                triangle_points[1]->velocity += opp_velocity;
                triangle_points[2]->velocity += opp_velocity;

                std::cout << "Bounce:" << bounce << std::endl;
                
                float remaining = glm::length(point->velocity) - (distance);
                std::cout << "Remaining:" << remaining << std::endl;
                // point->position += ((0.9f * distance) * direction); // Places it at the impact
                // point->position += (direction *  distance); 
            }
        }
    }
}

void Cloth::update(float dt) {

    for (size_t i = 0; i < STEPS; i++) {
        update_forces(dt / STEPS);
        update_positions(dt / STEPS);
        check_collisions(dt / STEPS);
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

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
   

    glm::mat4 cloth_model = glm::mat4(1.0f);

    glm::mat4 matrix_cloth_normal = cloth_model;
    // matrix_ball_normal[3] = glm::vec4(0,0,0,1);

    compute_normals();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, pointMasses.size() * sizeof(PointMass), &pointMasses[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(cloth_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_cloth_normal)); // projection matrix

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass), (void*) 0);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass), (GLvoid*)offsetof(PointMass, normal));

    // draw the cloth
    glDrawElements(GL_TRIANGLES,                    // primitive type
                indices.size(),          // # of indices
                GL_UNSIGNED_SHORT,                 // data type
                (void*)0);                       // offset to indices

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
    glBindVertexArray(0);

    shader.disable();
}

int Cloth::find_object(glm::vec3 origin, glm::vec3 direction) {
    float radius = REST_LENGTH * REST_LENGTH / 4.0f;
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
    float distance_from_camera = glm::distance(Globals::eye_pos, pointMasses[object].position);
    pointMasses[object].position = Globals::eye_pos + distance_from_camera * direction; 
}

void Cloth::cleanup() {

    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}