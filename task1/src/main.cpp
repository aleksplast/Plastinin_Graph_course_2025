#include "tree.hpp"
#include <vector>

int main() {
    Task1::RedBlackTree<int> tree1;
    tree1.enable_log();
    std::string log1 = "tree1_log";
    tree1.set_log_name(log1);
    std::vector vec{-14, 0, 3, 5, 11, 20, 21, 28, 42, 60};

    for (auto &elem: vec) {
        tree1.insert(elem);
    }
    int num14 = -14;
    tree1.erase(num14);
    int num0 = 0;
    tree1.erase(num0);
    int num28 = 28;
    tree1.erase(num28);

    tree1.dump_to_graphviz();

    Task1::RedBlackTree<int> tree2;
    tree2.enable_log();
    std::string log2 = "tree2_log";
    tree2.set_log_name(log2);
    std::vector vec2{-15, 1, 2, 4, 12, 22, 26, 27, 45, 80};

    for (auto &elem: vec2) {
        tree2.insert(elem);
    }

    tree1.merge(tree2);
    tree1.dump_to_graphviz();
}
