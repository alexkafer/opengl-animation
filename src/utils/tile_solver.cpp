#include "tile_solver.h"
#include  <random>

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

TileSolver::TileSolver(size_t dims) {
    tiles = std::vector<std::vector<TileOrientation>>(dims, std::vector<TileOrientation>(dims));

    for (int i = 0; i < tiles.size(); i++) { 
        for (int j = 0; j < tiles[i].size(); j++) {
            tiles[i][j] = std::make_pair(Undecided, -1); // Undecided
        }
    }

    while (has_undecided()) {
        solve_next();
    }

    // std::cout << "Finished solving. " << std::endl;

    for (int i = 0; i < tiles.size(); i++) { 
        for (int j = 0; j < tiles[i].size(); j++) {
            array<TileEdge, 4> edges = EdgeMap.at(tiles[i][j].first);
            int rotation_offset = tiles[i][j].second;
            // std::cout << "("<<i << "," << j << ") " << tiles[i][j].first << '-' << tiles[i][j].second << ": " << edges[(0 + rotation_offset) % 4] << ' '<< edges[(1 + rotation_offset) % 4] << ' '<< edges[(2 + rotation_offset) % 4] << ' '<< edges[(3 + rotation_offset) % 4] << std::endl;
        }
    }
}

TileOrientation TileSolver::get_type(size_t x, size_t y) {
    return tiles[x][y];
}

bool TileSolver::has_undecided() {
    for (int i = 0; i < tiles.size(); i++) { 
        for (int j = 0; j < tiles[i].size(); j++) {
            if (tiles[i][j].first == Undecided) return true; // Undecided
        }
    }
    return false;
}

void TileSolver::solve_next() {
    valid.clear();

    size_t bottom_bound = tiles.size() - 1;
    for (int i = 0; i <= bottom_bound; i++) { 
        size_t right_bound = tiles[i].size() - 1;
        for (int j = 0; j <= right_bound; j++) {
            if (tiles[i][j].first == Undecided) {
                std::array<TileEdge, 4> neighors = {None, None, None, None};

                std::pair<size_t, size_t> location = std::make_pair(i, j);

                // If a tile exists above, get it's bottom edge
                if (i > 0) {
                    TileOrientation t = tiles[i-1][j];
                    if (t.first != Undecided) {
                        // std::cout << t.first << " is to the right of  (" << i << "," << j <<"). It is rotated " << t.second << std::endl; 
                        neighors[3] = EdgeMap.at(t.first)[(1 + t.second) % 4];
                    } 
                } else {
                    neighors[3] = Land;
                }

                // If a tile exists to the left, get it's right edge
                if (j > 0) {
                    TileOrientation t = tiles[i][j-1];
                    if (t.first != Undecided) {
                        // std::cout << t.first << " is below (" << i << "," << j <<"). It is rotated " << t.second << std::endl; 
                        neighors[2] = EdgeMap.at(t.first)[(0 + t.second) % 4];
                    }
                } else {
                    neighors[2] = Land;
                }

                // If a tile exists below, get it's top edge
                if (i < right_bound) {
                    TileOrientation t = tiles[i+1][j];
                    if (t.first != Undecided) {
                        // std::cout << t.first << " is to the left of  (" << i << "," << j <<"). It is rotated " << t.second << std::endl; 
                        neighors[1] = EdgeMap.at(t.first)[(3 + t.second) % 4];
                    }
                } else {
                    neighors[1] = Land;
                }

                // If a tile exists to the right, get it's left edge
                if (j < bottom_bound) {
                    TileOrientation t = tiles[i][j+1];
                    if (t.first != Undecided) {
                        // std::cout << t.first << " is above (" << i << "," << j <<"). It is rotated " << t.second << std::endl; 
                        neighors[0] = EdgeMap.at(t.first)[(2 + t.second) % 4];
                    }
                } else {
                    neighors[0] = Water;
                }

                // Test all valid pairs
                for ( int tile = Street_3Way_2; tile != Undecided; tile++ )
                {
                    if (tile == Street_Deadend) continue;
                    const TileType type = static_cast<TileType>(tile);
                    std::array<TileEdge, 4> edges = EdgeMap.at(type);

                    for (int rotation_offset = 0; rotation_offset < 4; rotation_offset++) {
                        if (test_tile(edges, neighors, rotation_offset)) {
                            valid.push_back(std::make_pair(location, std::make_pair(type, rotation_offset)));
                        }
                    }
                }
            }
        }
    }

    // std::cout << "Found valid: " << valid.size() << std::endl;
    for (int j = 0; j < valid.size(); j++) {
        array<TileEdge, 4> edges = EdgeMap.at(valid[j].second.first);
        int rotation_offset = valid[j].second.second;
        // std::cout << "V: ("<<valid[j].first.first << "," << valid[j].first.second << ") " << valid[j].second.first << '-' << valid[j].second.second << ": " << edges[(0 + rotation_offset) % 4] << ' '<< edges[(1 + rotation_offset) % 4] << ' '<< edges[(2 + rotation_offset) % 4] << ' '<< edges[(3 + rotation_offset) % 4] << std::endl;
    }

    if (valid.size() > 0) {
        std::pair<std::pair<size_t, size_t>, TileOrientation> placement = *select_randomly(valid.begin(), valid.end());
        tiles[placement.first.first][placement.first.second] = placement.second;
        // std::cout << "Selected: " << placement.first.first << " " << placement.first.second << " to be " << placement.second.first<< " rotated by " << placement.second.second << std::endl;
    } else {
        // No valid selections. Lets backjump
        // std::cout << "Backjumping" << std::endl;
        std::vector<std::pair<size_t, size_t>> undecideds;
        for (int i = 0; i < tiles.size(); i++) { 
            for (int j = 0; j < tiles[i].size(); j++) {
                if (tiles[i][j].first == Undecided) undecideds.push_back(std::make_pair(i, j));
            }
        }

        std::pair<size_t, size_t> backjump = *select_randomly(undecideds.begin(), undecideds.end());

        if (backjump.first > 0)                         tiles[backjump.first-1][backjump.second] = std::make_pair(Undecided, -1);;
        if (backjump.first < tiles.size()-1)            tiles[backjump.first+1][backjump.second] = std::make_pair(Undecided, -1);;
        if (backjump.second > 0)                        tiles[backjump.first][backjump.second-1] = std::make_pair(Undecided, -1);;
        if (backjump.second < tiles[backjump.first].size()-1)   tiles[backjump.first][backjump.second+1] = std::make_pair(Undecided, -1);;
    }
}

bool TileSolver::verify_pair(TileEdge a, TileEdge b) {
    if (a == b) return true;

    if (a == None || b == None) return true;

    if (a == Land && b == Water) return true;

    return false;
}

bool TileSolver::test_tile(std::array<TileEdge, 4> edges, std::array<TileEdge, 4> neighors, int rotation_offset) {
    return (verify_pair(edges[(0 + rotation_offset) % 4], neighors[0]) &&
            verify_pair(edges[(1 + rotation_offset) % 4], neighors[1]) &&
            verify_pair(edges[(2 + rotation_offset) % 4], neighors[2]) &&
            verify_pair(edges[(3 + rotation_offset) % 4], neighors[3]));
}