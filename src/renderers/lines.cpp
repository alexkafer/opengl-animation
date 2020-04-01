#include "lines.h"

#include <glm/gtc/type_ptr.hpp>

#include "../geometry/model.h"

LineRenderer::LineRenderer() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/path.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

    glEnable(GL_PROGRAM_POINT_SIZE);

    glGenVertexArrays(1, &vao_path);
    glGenBuffers(1, &vbo_path);

    num_milestones = 0;
    num_edges = 0;


    glGenVertexArrays(1, &vao_bounding_box);
    glBindVertexArray(vao_bounding_box);
    

    // Cube 1x1x1, centered on origin
    GLfloat vertices[] = {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
    };

    glGenBuffers(1, &vbo_bounding_box_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_bounding_box_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint attribVertexPosition  = shader.attribute("in_position");

    glVertexAttribPointer(attribVertexPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    check_gl_error();

    GLushort elements[] = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glGenBuffers(1, &ibo_bounding_box_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_bounding_box_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void LineRenderer::init_graph(const std::vector<orientation_state> & milestones, const std::vector<uint> & edges) {
    // This draw graph is a O(1) draw that uses milestones and their connections (edges). 
    // It draws points at the milestones and lines between milestones that are connected
    if (milestones.size() == 0) return;
    num_milestones = milestones.size();
    shader.enable();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_milestones);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_milestones);
    glBufferData(GL_ARRAY_BUFFER, milestones.size() * sizeof(orientation_state), milestones.data(), GL_DYNAMIC_DRAW);

    GLint attribVertexPosition  = shader.attribute("in_position");
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(orientation_state), 0);
    glEnableVertexAttribArray(attribVertexPosition);

    check_gl_error();

    if (edges.size() == 0) return;
    num_edges = edges.size();

    glGenBuffers(1, &vbo_edges);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_edges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(uint), edges.data(), GL_DYNAMIC_DRAW);

    // glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(attribVertexPosition);

    shader.disable();
}

void LineRenderer::draw_graph(const glm::vec4 & color) {
    if (num_milestones == 0) return;
    shader.enable();

    glBindVertexArray(vao);

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    glVertexAttrib4fv(shader.attribute("in_color"), glm::value_ptr(color));

    glDrawArrays(GL_POINTS, 0, num_milestones);
    check_gl_error();

    if (num_edges == 0) {
        glBindVertexArray(0);
        return;
    }

    glDrawElements(GL_LINES, num_edges, GL_UNSIGNED_INT, (void*)0);
    check_gl_error();

    glBindVertexArray(0);
}

void LineRenderer::draw_path(const std::vector<glm::vec3> & edges, const glm::vec4 color) {
    if (edges.size() == 0) return;

    shader.enable();

    glBindVertexArray(vao_path);

    check_gl_error();

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    glm::mat4 model = glm::mat4( 1.0f );
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor  = shader.attribute("in_color");

    glVertexAttrib4fv(attribVertexColor, glm::value_ptr(color));

    check_gl_error();

    glBindBuffer(GL_ARRAY_BUFFER, vbo_path);
    glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(glm::vec3), edges.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribVertexPosition);

    check_gl_error();

    glDrawArrays(GL_LINES, 0, edges.size());

    glBindVertexArray(0);

    shader.disable();
    check_gl_error();
}

void LineRenderer::draw_bounding_box(Renderable * renderable, const glm::vec4 color) {
    check_gl_error();
    bounding_box bbox = renderable->get_bounding_box();

    // Should be max min, so if min is bigger than max we've got nothing
    if (bbox.first.x < bbox.second.x && bbox.first.y < bbox.second.y && bbox.first.z < bbox.second.z) return;

    shader.enable();

    glBindVertexArray(vao_bounding_box);

    // glm::vec3 position = renderable->get_position();

    glm::vec3 size = glm::vec3(bbox.first.x-bbox.second.x, bbox.first.y-bbox.second.y, bbox.first.z-bbox.second.z);
    glm::vec3 center = glm::vec3((bbox.second.x+bbox.first.x)/2, (bbox.second.y+bbox.first.y)/2, (bbox.second.z+bbox.first.z)/2);
    glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
    // glm::mat4 transform = glm::mat4(1);p

    check_gl_error();

    /* Apply object's transformation matrix */
    glm::mat4 m = renderable->get_last_model() * transform;
    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
    glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(m)); // model matrix

    check_gl_error();

    GLint attribVertexColor  = shader.attribute("in_color");
    glVertexAttrib4fv(attribVertexColor, glm::value_ptr(color));
    
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));

    // if (renderable->_children.size() > 0) {
    //     for(auto t=renderable->_children.begin(); t!=renderable->_children.end(); ++t) {
    //         draw_bounding_box(*t, color);
    //     }
    // }
    

    glBindVertexArray(0);

    check_gl_error();
    shader.disable();
}

void LineRenderer::cleanup() {
    glDeleteBuffers(1, &vbo_milestones);
    glDeleteBuffers(1, &vbo_edges);
    glDeleteVertexArrays(1, &vao);
    
    glDeleteBuffers(1, &vbo_path);
    glDeleteVertexArrays(1, &vao_path);
    
    glDeleteVertexArrays(1, &vao_bounding_box);
    glDeleteBuffers(1, &vbo_bounding_box_vertices);
    glDeleteBuffers(1, &ibo_bounding_box_elements);
}