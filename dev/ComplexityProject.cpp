#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>

struct my_vertex{
    int color;
    std::vector<int> neighbors;
    int uncolored_neighbors;
    my_vertex(int a, int b, int c) : color(a), neighbors(b), uncolored_neighbors(c) {}
};

class Wigderson_coloring {
    public:
        Wigderson_coloring(const std::vector<my_vertex>& other) : G_(other) {}
        int Find_max_degree() const;
        int size() const {
            return G_.size();
        }
        int color_on_index(int i) {
            return G_[i].color;
        }
        int mex_neighbor_color(const my_vertex& vert) {
            std::vector<char> flags;
            flags.assign(G_.size(), 0);
            int mex_val = 0;
            for (const int index: vert.neighbors) {
                int current_color = G_[index].color;
                if (current_color == -1) {
                    continue;
                }
                if (flags[current_color] == 1) {
                    continue;
                } else {
                    flags[current_color] = 1;
                    while(flags[mex_val]){
                        ++mex_val;
                    }
                }
            }
            return mex_val;
        }

        void color_big_neighbors(my_vertex& vert, int min_unused) {
            int first = min_unused;
            int second = min_unused + 1;
            std::vector<int>& neighbors = vert.neighbors;
            for (int i = 0; i < neighbors.size(); ++i) { 
                auto& cur_neig = G_[neighbors[i]];
                if (cur_neig.color != -1) {
                    continue;
                }
                for (int cousin: cur_neig.neighbors) {
                    if (G_[cousin].color == first) {
                        cur_neig.color = second;
                    }
                    if (G_[cousin].color == second) {
                        cur_neig.color = first;
                    }
                    --G_[cousin].uncolored_neighbors;
                }
                if (cur_neig.color == -1) {
                    cur_neig.color = first;
                }
            }
        }

        int wigderson() {
            int max_degree = Find_max_degree();
            int sqr_size = static_cast<int>(std::sqrt(G_.size()));
            if (sqr_size >= max_degree) {
                return greedy_coloring();
            }
            int min_unused = 0;
            for (my_vertex& vert: G_){
                if ((vert.uncolored_neighbors < sqr_size) || (vert.color != -1)) {
                    continue;
                } else {
                    color_big_neighbors(vert, min_unused);
                    min_unused += 2;
                }
            }
            greedy_coloring();
            int total = 0;
            for (my_vertex& vert: G_) {
                total = std::max(vert.color, total);
                vert.uncolored_neighbors = 0;
            }
            return total;
        }

        int greedy_coloring() { // don't hold uncolored neighbors
            int total = 0;
            for(my_vertex& current_coloring: G_) {
                if (current_coloring.color != -1) {
                    continue;
                }
                current_coloring.color = mex_neighbor_color(current_coloring); 
                total = std::max(total, current_coloring.color);
            }
            return total + 1;
        }

        void color_list() const {
            for (int i = 0; i < G_.size(); ++i) {
                std::cout << i << "  " << G_[i].color << std::endl;
            }
        }
    private:
        std::vector<my_vertex> G_;

};

int Wigderson_coloring::Find_max_degree() const {
    size_t max_ = 0;
    for (const my_vertex& vertex: G_){
        max_ = std::max(max_, vertex.neighbors.size());
    }
    return max_;
}

// class Bergler_and_Rompel_coloring {
//     private: 
//         std::vector<my_vertex> G_;
//         std::vector<
// };

int main () {
    int size_;
    std::cin >> size_;
    std::vector<my_vertex> G_;
    for (int i = 0; i < size_; ++i){
        int neighbors;
        std::cin >> neighbors;
        assert(neighbors < size_);
        my_vertex A_(-1, 0, neighbors);
        for (int j = 0; j < neighbors; ++j) {
            int number;
            std::cin >> number;
            assert(number < size_);
            A_.neighbors.emplace_back(number);
        }
        G_.emplace_back(A_);
    }

    Wigderson_coloring colorer(G_);
    colorer.wigderson();
    colorer.color_list();
}
