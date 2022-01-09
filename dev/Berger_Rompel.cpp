#include <vector>
#include <iostream>
#include <cassert>
#include <unordered_set>
#include <cmath>

struct my_vertex {
    std::vector<int> neighbors;
    int color;
    int uncolored_neighbors;
    my_vertex(int a, int b, int c) : color(a), neighbors(b), uncolored_neighbors(c) {}
};

class BR_coloring {
    public:
        BR_coloring(std::vector<my_vertex> G_) : G_(G_), is_in(G_.size(), 1), size_(G_.size()), n_(G_.size()), uncolored(G_.size()) {};
        int color();
        bool greedy_add(std::vector<char>& subset);
        void color_vertex(int);
        void color_greedy();
        int find_mex(std::vector<int>& set);
        void find_independent_set(std::vector<char>& subset, std::vector<char>& used, std::vector<char>& independed_set);
        bool check_if_independent(std::vector<char>& set);
        void color_neighbors(std::vector<char>& set);
    private:
        std::vector<my_vertex> G_;
        std::vector<char> is_in;
        int size_;
        int n_;
        int max_color = 0;
        int uncolored;
};

void BR_coloring::color_vertex(int index) {
    int first = max_color;
    int second = first + 1;
    int third = first + 2;
    for (int i: G_[index].neighbors) {
        for (int j: G_[i].neighbors){
            if (G_[j].color == first) {
                G_[i].color = second;
                --uncolored;
                break;
            }
            if (G_[j].color == second) {
                G_[i].color = first;
                --uncolored;
                break;
            }
        }
        if (G_[i].color != -1) {
            G_[i].color = first;
            --uncolored;
        }
    }
    G_[index].color = third;
    max_color = third + 1;
}

bool BR_coloring::greedy_add(std::vector<char>& subset) {
    std::vector<char> neighbors;
    for (int i = 0; i < n_; ++i) {
        if (subset[i]) {
            for (int j: G_[i].neighbors) {
                neighbors[j] = 1;
            }
        }
    }
    int max = 0;
    int arg_max = 0;
    for (int i = 0; i < n_; ++i) {
        if (is_in[i]) {
            int cur = 0;
            for (int j: G_[i].neighbors) {
                if (!neighbors[j]) {
                    ++cur;
                }
            }
            if (cur > max) {
                max = cur;
                arg_max = i;
            }
        }
    }
    if (max >= std::sqrt(n_ / std::log(n_))) {
        subset[arg_max] = 1;
        return true;
    } else {
        return false;
    }
}

int BR_coloring::find_mex(std::vector<int>& set) {
    std::vector<char> is_(set.size(), 0);
    int cur = 0;
    for(int i = 0; i < set.size(); +i) {
        int cur_color = G_[set[i]].color;
        if (!is_[cur_color]) {
            is_[cur_color] = 1;
        }
        while (is_[cur]) {
            ++cur;
        }
    }
    return cur;
}

void BR_coloring::color_greedy() {
    for (int i = 0; i < n_; ++i) { 
        if (G_[i].color != -1) {
            continue;
        }
        int mex_ = find_mex(G_[i].neighbors);
        if (mex_ == max_color){
            ++max_color;
        }
        G_[i].color = mex_;
    }
}

std::vector<char> positions_in_number (int a) {
    std::vector<char> ans;
    int i = 0;
    while ((1 << i) < a) {
        if (a & (1 << i)) {
            ans.push_back(i);
        }
    }
    return ans;
}

bool BR_coloring::check_if_independent(std::vector<char>& set) {
    for (char j: set) {
        for (int k: G_[j].neighbors) {
            for (char i: set) {
                if (k == i) {
                    return false;
                }
            }
        }
    }
    return true;
}

void BR_coloring::find_independent_set(std::vector<char>& subset, std::vector<char>& used, std::vector<char>& independed_set) {
    std::vector<char> new_set(n_, 0);
    int new_set_ones = 0;
    for (int i = 0; i < n_; ++i) {
        if (subset[i] && !used[i]) {
            new_set[i] = 1;
            ++new_set_ones;
        }
    }
    for (int i = 0; i < (2 << new_set_ones); ++i) {
        std::vector<char> try_set;
        auto poses = positions_in_number(i);
        if (poses.size() < std::log(n_)) {
            continue;
        }
        int ones = 0;
        for (int j = 0; j < new_set.size(); ++j) {
            if (new_set[j] == 1 && poses[ones]) {
                try_set.push_back(j);
            }
            if (new_set[j] == 1) {
                ++ones;
            }
        }
        if (check_if_independent(try_set)) {
            for (char j: try_set) {
                independed_set[j] = 1;
            }
        }
    }
    for (int i = 0; i < n_; ++i) {
        used[i] = used[i] || independed_set[i];
    }
}

void BR_coloring::color_neighbors(std::vector<char>& set) {
    int first = max_color;
    int second = first + 1;
    int third = first + 2;
    std::vector<char> neighbors;
    for (int i = 0; i < n_; ++i) {
        if (set[i]) {
            G_[i].color = third;
            --uncolored;
            for (int j : G_[i].neighbors) {
                neighbors[j] = 1;
            }
        }
    }
    for (int i = 0; i < n_; ++i) {
        if (neighbors[i]) {
            for (int j: G_[i].neighbors){
                if (G_[j].color == first) {
                    G_[i].color = second;
                    --uncolored;
                    break;
                }
                if (G_[j].color == second) {
                    G_[i].color = first;
                    --uncolored;
                    break;
                }
            }
            if (G_[i].color != -1) {
                G_[i].color = first;
                --uncolored;
            }
        }
    }
}

int BR_coloring::color () {
    while (uncolored) {
        std::vector<char> subset(n_, 0);
        int subset_size = 0;
        bool added = false;
        while (subset_size <= 3 * std::log(n_)) {
            added = greedy_add(subset);
            if (added) {
                ++subset_size; 
            }
            else {
                break;
            }
        }
        if (!added) {
            for (int i = 0; i < n_; ++i) {
                if (subset[i]) {
                    color_vertex(i);
                }
            }
            color_greedy();
        } else {
            std::vector<char> used(n_, 0);
            std::vector<char> independed_set(n_, 0);
            for (int i = 0; i < 3; ++i) {
                find_independent_set(subset, used, independed_set);
                color_neighbors(independed_set);
            }
        }
    }
}

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

    BR_coloring colorer(G_);
    colorer.color();
}