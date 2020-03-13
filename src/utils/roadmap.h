#ifndef ROADMAP_H
#define ROADMAP_H

#include <vector>
#include <glm/vec3.hpp>

class Roadmap {
    std::vector<glm::vec3> _milestones;
    std::vector<std::pair<int, float>> * _adj;
    int _milestones_count;

public:
    Roadmap(int milestones);
    ~Roadmap();
    void clear();
    void print();

    std::vector<glm::vec3> get_edges();
    const std::vector<glm::vec3> & get_milestones();
    int add_milestone(glm::vec3 point);
    void add_edge(int u, int v, float distance);

    std::vector<glm::vec3> find_path(int start, int destination);
    std::vector<glm::vec3> dijkstra_path(int src, int dest);
};

#endif // ROADMAP_H