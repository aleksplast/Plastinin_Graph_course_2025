#include "tree.hpp"
#include <iostream>

int main() {
    Task1::RedBlackTree<int> tree1;
    tree1.enable_log();
    std::string log1 = "tree1_log";
    tree1.set_log_name(log1);
    std::vector vec{-14, 0, 3, 5, 11, 20, 21, 28, 42, 60};

    for (auto &elem: vec) {
        std::cout << "elem = " << elem << '\n';
        tree1.insert(elem);
    }
    int num14 = -14;
    tree1.erase(num14);

    tree1.dump_to_graphviz();
}
