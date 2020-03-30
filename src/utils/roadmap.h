#ifndef ROADMAP_H
#define ROADMAP_H

#include <vector>
#include <unordered_map>
#include "../common.h"

typedef std::pair<int, float> weighted_edge;

class Roadmap {
    std::vector<orientation_state> _milestones;
    std::vector<std::pair<int, float>> * _adj;
    int _milestones_count;

    std::vector<unsigned int> _renderable_edges;
public:
    Roadmap(int milestones);
    ~Roadmap();
    void clear();
    void print();

    void calculate_edges();
    const std::vector<unsigned int> & get_edges();
    const std::vector<orientation_state> & get_milestones();
    int add_milestone(orientation_state state);
    void add_edge(int u, int v, float distance);

    std::vector<orientation_state> dijkstra_path(int src, int dest);

    float heuristic(int node, int goal);
    std::vector<orientation_state> a_star_path(int src, int dest);
};

#endif // ROADMAP_H