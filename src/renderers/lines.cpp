#include "lines.h"

#include <glm/gtc/type_ptr.hpp>

#include "../geometry/model.h"

PathRenderer::PathRenderer() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/path.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_milestones);
    glGenBuffers(1, &vbo_edges);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

void PathRenderer::init_graph(const std::vector<orientation_state> & milestones, const std::vector<uint> & edges) {
    // This draw graph is a O(1) draw that uses milestones and their connections (edges). 
    // It draws points at the milestones and lines between milestones that are connected
    if (milestones.size() == 0) return;
    shader.enable();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_milestones);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_milestones);
    glBufferData(GL_ARRAY_BUFFER, milestones.size() * sizeof(glm::vec3), milestones.data(), GL_DYNAMIC_DRAW);

    GLint attribVertexPosition  = shader.attribute("in_position");
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    glGenBuffers(1, &vbo_edges);
}

void PathRenderer::draw_graph(const std::vector<orientation_state> & milestones, const std::vector<uint> & edges, const glm::vec4 & color) {
    shader.enable();

    glBindVertexArray(vao);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    
    GLint attribVertexColor  = shader.attribute("in_color");

    glVertexAttrib4fv(attribVertexColor, glm::value_ptr(color));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_milestones);
    glBufferData(GL_ARRAY_BUFFER, milestones.size() * sizeof(glm::vec3), milestones.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    glDrawArrays(GL_POINTS, 0, milestones.size());

    check_gl_error();

    
    if (edges.size() == 0) return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(uint), edges.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    glDrawElements(GL_LINES, edges.size(), GL_UNSIGNED_INT, (void*)0);

    check_gl_error();

    glBindVertexArray(0);
}

void PathRenderer::draw_edges(const std::vector<glm::vec3> edges, const glm::vec4 color) {
    if (edges.size() == 0) return;

    shader.enable();

    glBindVertexArray(vao);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor  = shader.attribute("in_color");

    glVertexAttrib4fv(attribVertexColor, glm::value_ptr(color));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_edges);
    glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(glm::vec3), edges.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    glDrawArrays(GL_LINES, 0, edges.size());

    glBindVertexArray(0);

    shader.disable();
}

void PathRenderer::cleanup() {
  
}