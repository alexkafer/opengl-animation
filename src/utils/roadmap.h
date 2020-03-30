#ifndef ROADMAP_H
#define ROADMAP_H

#include <vector>
#include "../common.h"

class Roadmap {
    std::vector<orientation_state> _milestones;
    std::vector<std::pair<int, float>> * _adj;
    int _milestones_count;

public:
    Roadmap(int milestones);
    ~Roadmap();
    void clear();
    void print();

    std::vector<glm::vec3> get_edges();
    const std::vector<orientation_state> & get_milestones();
    int add_milestone(orientation_state state);
    void add_edge(int u, int v, float distance);

    std::vector<orientation_state> dijkstra_path(int src, int dest);
};

#endif // ROADMAP_H