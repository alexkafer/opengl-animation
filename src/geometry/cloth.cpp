#include "cloth.h"

#include <math.h> 
#include <iostream>
#include <utility>
#include <limits>
#include <algorithm>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "../utils/texture.h"

#define IX(i,j) ((i)+(_x_dim)*(j))


static glm::vec3 gravity(0.f, -9.8f, 0.f);
static glm::vec3 air_velocity(-1.f, 0.f, -1.f);
// static glm::vec3 gravity(0.f, -1.f, 0.f);

static const float STEPS = 10;
static const float REST_LENGTH = 0.5f;
// static const float K = 500.f; 
static const float K = 100.f; 
static const float KV = 5.f;
static const float KD = 0.01f;
static const float P = 1.f;
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
    _total = x_dim * y_dim;
    pointMasses = new PointMass[_total];
    forces = new glm::vec3[_total];

    edges = std::vector<std::pair<size_t, size_t>>();
    size_t edge_count = (x_dim-1)*y_dim + (y_dim-1)*x_dim;
    edges.reserve(edge_count);

    // spatial_hash = spatial_hash_map(HASH_TABLE_SIZE); //, spatial_hashing_func);

    // pointMasses.push_back(PointMass(glm::vec3(2.f * (float) rand() / RAND_MAX - 1.f, (points + 15.f) / 5.f, 2.f * (float) rand() / RAND_MAX - 1.f), 0));
    // size_t vertex[x_dim][y_dim];

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            // IX(x,y) = pointMasses.size();
            // std::cout << "Generated " << IX(x,y) << " at (" << x << ", "<< y << ")" << std::endl;
            pointMasses[IX(x,y)] = PointMass(
                glm::vec3(x * REST_LENGTH, 10.f, REST_LENGTH * y), 
                glm::vec2((-1.f * x) / x_dim, (-1.f * y) / y_dim));
            // pointMasses.push_back();
            forces[IX(x,y)] = glm::vec3(0.0f);
        }
    }

    for (size_t x = 0; x < x_dim; x++) {
        for (size_t y = 0; y < y_dim; y++) {
            
            // If not on the x edge
            if (x < x_dim-1) {
                // std::cout << "Paired " << IX(x,y) << " with " <<IX(x+1,y) << std::endl;
                edges.push_back(std::make_pair(IX(x,y), IX(x+1,y)));
            }
            
            // If not on the bottom row
            if (y < y_dim-1) {
                // std::cout << "Paired " << IX(x,y) << " with " <<IX(x,y+1) << std::endl;
                edges.push_back(std::make_pair(IX(x,y), IX(x,y+1)));
            }

            // Left down triangle
            if (x < x_dim-1 && y < y_dim-1) {
                indices.push_back(IX(x,y));
                indices.push_back(IX(x+1,y));
                indices.push_back(IX(x,y+1));

                // std::cout << "Made triangle " << IX(x,y) << ", " <<IX(x+1,y) << ", " <<IX(x,y+1) << std::endl;
            }

            // Up right triangle
            if (x > 0 && y > 0) {
                indices.push_back(IX(x,y));
                indices.push_back(IX(x-1,y));
                indices.push_back(IX(x,y-1));

                // std::cout << "Made triangle " << IX(x,y) << ", " <<vertex[x-1][y] << ", " <<vertex[x][y-1] << std::endl;
            }
        }
    }
}

void Cloth::init(Shader & shader) {
    shader.enable();

    glGenVertexArrays(1, &vao); //Create a VAO
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _total * sizeof(PointMass), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

    
    std::stringstream texture_ss; texture_ss << MY_MODELS_DIR << "/textures/cloth.png";
    texture_id = load_texture(texture_ss.str().c_str());
  
    glBindVertexArray(0);

    shader.disable();
}

// Algorithm adapted from TinyObj loader
void Cloth::compute_normals() {
    #pragma omp parallel for
    for(size_t i = 0; i < _total; i++) {
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

    #pragma omp parallel for
    for(size_t i = 0; i < _total; i++) {
        pointMasses[i].normal = glm::normalize(pointMasses[i].normal);
    }
}


void Cloth::update_forces(float dt) {
    // Clear forces before we get started this round
    #pragma omp parallel for
    for(size_t i = 0; i < _total; i++) forces[i] = gravity;

    for(size_t i = 0; i < edges.size(); i++) 
    {
        std::pair<size_t, size_t> edge = edges.at(i);
        glm::vec3 edge_vector = pointMasses[edge.first].position - pointMasses[edge.second].position;

        float string_length = glm::length(edge_vector); 
        float string_force = -K * STEPS *(string_length - (REST_LENGTH));

        edge_vector /= string_length; // Normalize

        float velocity_one = glm::dot(pointMasses[edge.first].velocity, edge_vector);
        float velocity_two = glm::dot(pointMasses[edge.second].velocity, edge_vector);

     

        float damp_force = -KV * STEPS * (velocity_one - velocity_two);

        if (Globals::mouse_down && (is_selected(edge.first) || is_selected(edge.second))) {
            string_force *= 10.f;
        }

        glm::vec3 force_added = ((string_force + damp_force) * glm::normalize(edge_vector)) / 2.f;

        // If the cloth is not being held on to, apply the force
        // #pragma omp atomic update
        // forces[edge.first].x += force_added.x;
        // #pragma omp atomic update
        // forces[edge.first].y += force_added.y;
        // #pragma omp atomic update
        // forces[edge.first].z += force_added.z;

        // // Equal and opposite force
        // #pragma omp atomic update
        // forces[edge.second].x -= force_added.x;
        // #pragma omp atomic update
        // forces[edge.second].y -= force_added.y;
        // #pragma omp atomic update
        // forces[edge.second].z -= force_added.z;

        forces[edge.first] += force_added;
        forces[edge.second] -= force_added;
    }

    #pragma omp parallel for
    for (size_t f = 0; f < indices.size() / 3; f++) {
        size_t point_a = indices[3 * f + 0];
        size_t point_b = indices[3 * f + 1];
        size_t point_c = indices[3 * f + 2];
        
        glm::vec3 normal = glm::triangleNormal(pointMasses[point_a].position, pointMasses[point_b].position, pointMasses[point_c].position);
        glm::vec3 velocity = ((pointMasses[point_a].velocity + pointMasses[point_b].velocity + pointMasses[point_c].velocity) / 3.f) - air_velocity;

        float constants = -(P * KD) / (2.f * 3.f);

        glm::vec3 aero_force = (glm::length(velocity) * glm::dot(velocity, normal) * normal) / (2.f * glm::length(normal));

        #pragma omp atomic update
        forces[point_a].x -= aero_force.x;
        #pragma omp atomic update
        forces[point_a].y -= aero_force.y;
        #pragma omp atomic update
        forces[point_a].z -= aero_force.z;

        #pragma omp atomic update
        forces[point_b].x -= aero_force.x;
        #pragma omp atomic update
        forces[point_b].y -= aero_force.y;
        #pragma omp atomic update
        forces[point_b].z -= aero_force.z;

        #pragma omp atomic update
        forces[point_c].x -= aero_force.x;
        #pragma omp atomic update
        forces[point_c].y -= aero_force.y;
        #pragma omp atomic update
        forces[point_c].z -= aero_force.z;
    }
}

void Cloth::update_positions(float dt) {
    spatial_hash.clear(); // Clear the hash since we're calculating new positions

    // After we've calculated all the velocities and made modifications, apply them;
    #pragma omp parallel for
    for(size_t i = 0; i < _total; i++) {
        
        // if ((i % _x_dim  == _y_dim-1) ||  (Globals::mouse_down && is_selected(i))) {
        if ((i / _x_dim == (_y_dim / 2)) || (i / _x_dim == (_y_dim / 2) + 1)|| (Globals::mouse_down && is_selected(i))) {
        // if ((i % _y_dim == 0) || (Globals::dragging_object && Globals::selected == i)) {
        // if ((Globals::dragging_object && Globals::selected == i)) {
            // Kill all forces for an object being held
            forces[i] = glm::vec3(0.f);
            pointMasses[i].velocity = glm::vec3(0.f);
        } else {
            pointMasses[i].velocity += dt * forces[i] / MASS;
            //  std::cout << "pre position: " << glm::to_string(pointMasses[i].position) << std::endl;
            pointMasses[i].position += dt * pointMasses[i].velocity;  
            //  std::cout << "post position: " << glm::to_string(pointMasses[i].position) << std::endl;
        }

        // spatial_hash.insert(std::make_pair<size_t, size_t>(spatial_hashing_func(pointMasses[i].position), (size_t) i));
    }
}

// void Cloth::test_triangle_intersection(PointMass * triangle_points [3]) {
//     for (size_t f = 0; f < indices.size() / 3; f++) {
//         // Get the three indexes of the face (all faces are triangular)
//         PointMass * triangle_test_points [3];

//         bool skip = false;

//         for (size_t i = 0; i < 3; i++) {
//             triangle_test_points[i] = &pointMasses[indices[3 * f + i]];

//             if (triangle_points[0] == triangle_test_points[i] ||
//                 triangle_points[1] == triangle_test_points[i] ||
//                 triangle_points[2] == triangle_test_points[i]) {
//                     skip = true;
//             }
//         }

//         if (skip) continue;

//         float V0[3] = {triangle_points[0]->position.x, triangle_points[0]->position.y, triangle_points[0]->position.z};
//         float V1[3] = {triangle_points[1]->position.x, triangle_points[1]->position.y, triangle_points[1]->position.z};
//         float V2[3] = {triangle_points[2]->position.x, triangle_points[2]->position.y, triangle_points[2]->position.z};

//         float U0[3] = {triangle_test_points[0]->position.x, triangle_test_points[0]->position.y, triangle_test_points[0]->position.z};
//         float U1[3] = {triangle_test_points[1]->position.x, triangle_test_points[1]->position.y, triangle_test_points[1]->position.z};
//         float U2[3] = {triangle_test_points[2]->position.x, triangle_test_points[2]->position.y, triangle_test_points[2]->position.z};
        
//         if(tri_tri_intersect(V0, V1, V2, U0, U1, U2)) {
//                 std::cout << "Triangles are intersecting " << std::endl;

//                 triangle_points[0]->velocity *= -0.9;
//                 triangle_points[1]->velocity *= -0.9;
//                 triangle_points[2]->velocity *= -0.9;

//                 triangle_test_points[0]->velocity *= -0.9;
//                 triangle_test_points[1]->velocity *= -0.9;
//                 triangle_test_points[2]->velocity *= -0.9;
//         }
//     }
// }
static const float collision_radius = .59f;
void Cloth::check_collisions(float dt) {
    for(size_t i = 0; i < _total; i++) {

        if (pointMasses[i].position.y < 0) {
            pointMasses[i].position.y = 0.01f; 
            pointMasses[i].velocity.y *= -0.5; //bounce factor
            pointMasses[i].velocity.x *= 0.95; //bounce factor
            pointMasses[i].velocity.z *= 0.95; //bounce factor
        }

        if (pointMasses[i].position.y < 5.3f && pointMasses[i].position.x > 1.2f && pointMasses[i].position.x < 9.1f && pointMasses[i].position.z > 9.1f) {
            pointMasses[i].position.y = 5.3f; 
            pointMasses[i].velocity.y *= -0.5; //bounce factor
            pointMasses[i].velocity.x *= 0.97; //bounce factor
            pointMasses[i].velocity.z *= 0.97; //bounce factor
        }

        // std::cout << "p1.position: " << glm::to_string(pointMasses[i].position) << std::endl;
        // for(size_t j = 0; j < pointMasses.size(); j++) {
        //     if (i == j) continue;
        //     //  std::cout << "p2.position: " << glm::to_string(pointMasses[j].position) << std::endl;

        //     glm::vec3 vecs = pointMasses[i].position - pointMasses[j].position;
        //     glm::vec3 vecv = pointMasses[i].velocity - pointMasses[j].velocity;
            
        //     float distance = glm::length(vecs);

        //     // std::cout << "distance" << distance << std::endl;

        //     if (distance < collision_radius * collision_radius) {

        //         float b = glm::dot(vecv, vecs);

        //         if (b >= 0.0f) continue;

        //         float a = glm::length(vecv); 
        //         float d = b * b - a * distance;

        //         if (d < 0.0f) continue;

        //         float t = (-b - sqrt(d)) / a;

        //         glm::vec3 pos_n = (vecs / distance);
        //         glm::vec3 neg_n = -1.f * (pos_n);

        //         std::cout << "Collision at t: " << t << std::endl;

        //         // std::cout << "Normal" << glm::length(n) << std::endl;
                
        //         glm::vec3 bounce = glm::dot(pointMasses[j].velocity, neg_n) * neg_n;
        //         pointMasses[j].velocity -= (1.5f * bounce);
        //         pointMasses[i].velocity += (1.5f * bounce);

        //         pointMasses[j].position += ((collision_radius-distance) * neg_n);
        //         pointMasses[i].position += ((collision_radius-distance) * pos_n);
        //     }
        // }
    }
}

// void Cloth::check_collisions(float dt) {
//     // This code is my own implementation of Matthias Teschner's collision detection algorithm
//     // Optimized Spatial Hashing for Collision Detection of Deformable Objects
//     // https://matthias-research.github.io/pages/publications/tetraederCollision.pdf

//     std::pair <spatial_hash_map::iterator, spatial_hash_map::iterator> ret;
//     // spatial_hash.clear();

//     // // Populate the hash table
//     // for(size_t i = 0; i < pointMasses.size(); i++) {
//     //     spatial_hash.insert(std::make_pair<size_t, size_t>(spatial_hashing_func(pointMasses[i].position), (size_t) i));
//     // }

//     // Check for collisions
//     // printUmm(spatial_hash);

//     for (size_t f = 0; f < indices.size() / 3; f++) {
//         // Get the three indexes of the face (all faces are triangular)
//         PointMass * triangle_points [3];

//         for (size_t i = 0; i < 3; i++) {
//             triangle_points[i] = &pointMasses[indices[3 * f + i]];

//             if (triangle_points[i]->position.y < 0) {
//                 triangle_points[i]->position.y = 0.01f; 
//                 triangle_points[i]->velocity.y *= -0.5; //bounce factor
//                 triangle_points[i]->velocity.x *= 0.95; //bounce factor
//                 triangle_points[i]->velocity.z *= 0.95; //bounce factor
//             }
//         }

//         // test_triangle_intersection(triangle_points);
        
//         // ret = spatial_hash.equal_range(spatial_hashing_func(triangle_points[0]->position));

//         // for (spatial_hash_map::iterator it=ret.first; it!=ret.second; ++it) {
//         //     PointMass * point = &pointMasses[it->second];

//         for (PointMass & mass : pointMasses) {
//             PointMass * point = &mass;
//             // Make sure the point isn't a member of the triangle
//             if (point == triangle_points[0] || point == triangle_points[1] || point == triangle_points[2]) continue;

//             // glm::vec3 direction = glm::normalize(point->velocity);

//             // ray collision
//             glm::vec2 barycentric_position; 
//             float distance = glm::distance(triangle_points[0]->position, point->position);
//             if (distance < REST_LENGTH) {
            
//             }

//                 if (distance < 0 || distance > glm::length(dt * point->velocity)) continue;
                
//                 std::cout << "Detected to " << distance << " intersection at " << glm::to_string(point->position) << " traveling " << glm::to_string(point->velocity) << std::endl;
                
//                 // Triangle bounce
//                 glm::vec3 triangle_normal = glm::normalize(
//                     triangle_points[0]->normal 
//                     + triangle_points[1]->normal 
//                     + triangle_points[2]->normal
//                 );

//                 float bounce = 1.9f * glm::dot(point->velocity, triangle_normal);
//                 point->velocity -= (bounce * triangle_normal);

//                 glm::vec3 opp_velocity = (bounce * triangle_normal) / 3.f;

//                 triangle_points[0]->velocity += opp_velocity;
//                 triangle_points[1]->velocity += opp_velocity;
//                 triangle_points[2]->velocity += opp_velocity;

//                 std::cout << "Bounce:" << bounce << std::endl;
                
//                 glm::vec3 remaining = 5.f * direction * distance;
//                 std::cout << "Remaining:" << glm::to_string(remaining) << std::endl;
//                 point->position += remaining; // Places it at the impact
//                 triangle_points[0]->position -= remaining;
//                 triangle_points[1]->position -= remaining;
//                 triangle_points[2]->position -= remaining;
                
//                 // point->position += (point->velocity * remaining); // Complete the remaining of the update
//             }
//         }
//     }
// }

void Cloth::update(float dt) {

    for (size_t i = 0; i < STEPS; i++) {
        update_forces(dt / STEPS);
        update_positions(dt / STEPS);
        check_collisions(dt / STEPS);
    }
}

void Cloth::draw(Shader & shader) {
    shader.enable();

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");
    GLuint attribVertexTextureCoord     = shader.attribute("in_texture_coord");

	GLint attribUniformModel    = shader.uniform("model");
	GLint attribUniformNormal    = shader.uniform("normal");
	GLint attribUniformTextureUsed    = shader.uniform("texture_used");

    glBindVertexArray(vao);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(attribUniformTextureUsed, 1);

    glVertexAttrib3f(attribVertexColor, 0.8, 0.8, 0.8);

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);
    glEnableVertexAttribArray(attribVertexTextureCoord);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glm::mat4 cloth_model = glm::mat4(1.0f);

    glm::mat4 matrix_cloth_normal = cloth_model;
    // matrix_ball_normal[3] = glm::vec4(0,0,0,1);

    compute_normals();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _total * sizeof(PointMass), pointMasses, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(cloth_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(matrix_cloth_normal)); // projection matrix

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass), (void*) 0);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(PointMass), (GLvoid*)offsetof(PointMass, normal));
    glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(PointMass), (GLvoid*)offsetof(PointMass, texture_coord));

    // draw the cloth
    glDrawElements(GL_TRIANGLES,                    // primitive type
                indices.size(),          // # of indices
                GL_UNSIGNED_SHORT,                 // data type
                (void*)0);                       // offset to indices

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(attribUniformTextureUsed, 0);
 
    glBindVertexArray(0);

    shader.disable();
}

bool Cloth::is_selected(int i) {
    int y_sel = selected / _x_dim;
    int x_sel = selected % _x_dim;

    int y = i / _x_dim - y_sel;
    int x = i % _x_dim - x_sel;

    float radius = 5.f;

    return (x*x + y*y) < (radius*radius); 
}

void Cloth::interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down) {
    if (mouse_down && selected > 0) {
        drag_selected(direction);
        return;
    }

    float radius = REST_LENGTH * REST_LENGTH / 4.0f;
    float closest_distance = std::numeric_limits<float>::max();
    int closest = -1;

    for(int i = 0; i < _total; i++) {
        float distance; 
        if (glm::intersectRaySphere(origin, direction, pointMasses[i].position, radius, distance)) {
            std::cout << "Found point: " << i << std::endl;
            if (distance < closest_distance) {
                closest = i;
            }
        }
    }

    selected = closest;
}

void Cloth::drag_selected(glm::vec3 direction) {

    float distance_from_camera = glm::distance(Globals::eye_pos, pointMasses[selected].position);
    glm::vec3 new_position = Globals::eye_pos + distance_from_camera * direction;
    glm::vec3 relative_direction = new_position - pointMasses[selected].position;
    // pointMasses[selected].position = new_position; 

    int y_sel = selected / _x_dim;
    int x_sel = selected % _x_dim;

    for (int x = fmax(0, x_sel - 5); x < fmin(_x_dim, x_sel + 5); x++) {
        for (int y = fmax(0, y_sel - 5); y < fmin(_y_dim, y_sel + 5); y++) {
            float x_diff = x - x_sel;
            float y_diff = y - y_sel;
            float smoothing = fmax(0, 1.f - (x_diff * x_diff + y_diff*y_diff) / 100.f);
            // std::cout << "Smoothing: " << smoothing << " " << sqrt((x*x + y*y)) << std::endl;
            pointMasses[IX(x, y)].position += smoothing * relative_direction;
        }
    }

    
}

void Cloth::cleanup() {

    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}