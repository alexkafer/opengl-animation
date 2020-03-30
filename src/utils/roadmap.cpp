// Adapted from https://www.geeksforgeeks.org/graph-implementation-using-stl-for-competitive-programming-set-2-weighted-graph/
// and https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
#include "roadmap.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <limits>
#include <iostream>
#include <queue>          // std::priority_queue
#include <functional>     // std::greater


Roadmap::Roadmap(int milestones) {
    _milestones_count = milestones;

    _milestones = std::vector<orientation_state>();
    _adj = new std::vector<std::pair<int, float>>[milestones]; 
}

Roadmap::~Roadmap() {
    delete []_adj;
}

const std::vector<orientation_state> & Roadmap::get_milestones() {
    return _milestones;
}

std::vector<glm::vec3> Roadmap::get_edges() {
    std::vector<glm::vec3> edges; 

    for (int u = 0; u < _milestones_count; u++) 
    {
        for (auto it = _adj[u].begin(); it!= _adj[u].end(); it++) 
        { 
            edges.push_back(_milestones.at(u).first);
            edges.push_back(_milestones.at(it->first).first);
        }
    } 

    return edges;
}

void Roadmap::clear() {
    _milestones.clear();

    for (int u = 0; u < _milestones_count; u++) 
    { 
        _adj[u].clear();
    }
}

int Roadmap::add_milestone(orientation_state state) {
    _milestones.push_back(state);

    return _milestones.size() - 1;
}

void Roadmap::add_edge(int u, int v, float distance) {
    _adj[u].push_back(std::make_pair(v, distance)); 
    _adj[v].push_back(std::make_pair(u, distance)); 
}

std::vector<orientation_state> Roadmap::dijkstra_path(int src, int dest) {
    std::priority_queue<std::pair<int, int>, std::vector <std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    std::vector<int> dist(_milestones_count, std::numeric_limits<int>::max()); 
    std::vector<int> parent(_milestones_count, -1); 

    pq.push(std::make_pair(0, src)); 
    dist[src] = 0; 

    while (!pq.empty()) 
    { 
        int u = pq.top().second; 
        pq.pop(); 
  
        for (auto x : _adj[u])
        { 
            int v = x.first; 
            int weight = x.second; 
  
            //  If there is shorted path to v through u. 
            if (dist[v] > dist[u] + weight) 
            { 
                // Updating distance of v 
                dist[v] = dist[u] + weight; 
                parent[v] = u;

                // If we found our target
                if (v == dest) {
                    pq.empty();
                    break;
                }
                pq.push(std::make_pair(dist[v], v)); 
            } 
        } 
    }

    std::vector<orientation_state> path;
   
    int i = dest;
    while(i != -1) {
        path.push_back(_milestones[i]);
        i = parent[i];
    }

    // Smooth the orientations, if we can. This is done by moving backwards through the path 
    for (std::vector<orientation_state>::reverse_iterator i = path.rbegin() + 1; i != path.rend(); ++i ) { 
        glm::vec3 last_pos = i[-1].first;
        glm::vec3 current_pos = i[0].first;
        i->second = glm::normalize(current_pos - last_pos);
    } 

    return path;
}

float Roadmap::heuristic(int node, int goal) {
    return glm::distance(_milestones[node].first, _milestones[goal].first);
}

std::vector<orientation_state> Roadmap::a_star_path(int src, int dest) {
    std::priority_queue<std::pair<int, int>, std::vector <std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    std::vector<int> dist(_milestones_count, std::numeric_limits<int>::max()); 
    std::vector<int> parent(_milestones_count, -1); 

    pq.push(std::make_pair(0, src)); 
    dist[src] = 0; 

    while (!pq.empty()) 
    { 
        int u = pq.top().second; 
        pq.pop(); 
  
        for (auto x : _adj[u])
        { 
            int v = x.first; 
            int weight = x.second; 
  
            //  If there is shorted path to v through u. 
            if (dist[v] > dist[u] + weight) 
            { 
                // Updating distance of v 
                dist[v] = dist[u] + weight; 
                parent[v] = u;

                // If we found our target
                if (v == dest) {
                    pq.empty();
                    break;
                }
                pq.push(std::make_pair(dist[v] + heuristic(v, dest), v)); 
            } 
        } 
    }

    std::vector<orientation_state> path;
   
    int i = dest;
    while(i != -1) {
        path.push_back(_milestones[i]);
        i = parent[i];
    }

    // Smooth the orientations, if we can. This is done by moving backwards through the path 
    for (std::vector<orientation_state>::reverse_iterator i = path.rbegin() + 1; i != path.rend(); ++i ) { 
        glm::vec3 last_pos = i[-1].first;
        glm::vec3 current_pos = i[0].first;
        i->second = glm::normalize(current_pos - last_pos);
    } 

    return path;
}

// Print adjacency list representaion ot graph 
void Roadmap::print() 
{ 
    int v, w; 
    for (int u = 0; u < _milestones_count; u++) 
    { 
        std::cout << "Node " << u << " makes an edge with" << std::endl; 
        for (auto it = _adj[u].begin(); it!= _adj[u].end(); it++) 
        { 
            v = it->first; 
            w = it->second; 
            std::cout << "\tNode " << v << " with edge weight ="
                 << w << std::endl; 
        } 
        std::cout << std::endl; 
    } 
} 