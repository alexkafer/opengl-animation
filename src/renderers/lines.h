#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <vector>
#include <array>
#include <glm/mat4x4.hpp>

#include "../common.h"
#include "../utils/shader.h"
#include "../utils/obb.h"

#include "renderable.h"


class LineRenderer
{
    Shader shader;
    GLuint vao;
    GLuint vao_path;

    GLuint vao_bounding_box;
    GLuint vbo_bounding_box_vertices;
    GLuint ibo_bounding_box_elements;

    GLuint vbo_milestones;
    GLuint vbo_edges;
    GLuint vbo_path;

    size_t num_milestones;
    size_t num_edges;

    public:
        LineRenderer();

        void init_graph(const std::vector<orientation_state> & milestones, const std::vector<uint> & edges);
        void draw_graph(const glm::vec4 & color);

        void draw_path(const std::vector<glm::vec3> & edges, const glm::vec4 color);
        void draw_bounding_box(Renderable * box, const glm::vec4 color);
        void draw_bounding_box(const OBB & bbox, const glm::vec4 color);

        void cleanup();
};
#endif // LINE_RENDERER_H