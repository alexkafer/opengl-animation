#ifndef ROADMAP_H
#define ROADMAP_H

#include <vector>
#include <list>
#include <glm/vec3.hpp>

class Roadmap {
    std::vector<glm::vec3> _milestones;
    std::vector<std::pair<int, float>> * _adj;
    int _milestones_count;

    std::list<glm::vec3> dijkstra_path(int src, int dest);
public:
    Roadmap(int milestones);
    ~Roadmap();
    void clear();
    void print();

    const std::vector<glm::vec3> & get_milestones();
    void add_milestone(glm::vec3 point);
    void add_edge(int u, int v, float distance);

    std::list<glm::vec3> find_path(glm::vec3 start, glm::vec3 destination);
};

#endif // ROADMAP_H