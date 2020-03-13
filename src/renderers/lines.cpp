#include "lines.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common.h"
#include "../geometry/model.h"

PathRenderer::PathRenderer() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/path.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo_milestones);
    glGenBuffers(1, &vbo_edges);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

void PathRenderer::draw_milestones(const std::vector<glm::vec3> milestones) {
    if (milestones.size() == 0) return;

    shader.enable();

    glBindVertexArray(vao);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    GLint attribVertexPosition  = shader.attribute("in_position");

    glBindBuffer(GL_ARRAY_BUFFER, vbo_milestones);
    glBufferData(GL_ARRAY_BUFFER, milestones.size() * sizeof(glm::vec3), milestones.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    glDrawArrays(GL_POINTS, 0, milestones.size());

    glBindVertexArray(0);

    shader.disable();
}

void PathRenderer::draw_edges(const std::vector<glm::vec3> edges) {
    if (edges.size() == 0) return;

    shader.enable();

    glBindVertexArray(vao);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    GLint attribVertexPosition  = shader.attribute("in_position");

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