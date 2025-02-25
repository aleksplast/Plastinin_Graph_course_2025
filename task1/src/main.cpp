#include "tree.hpp"
#include <map>

int main() {
    Task1::RedBlackTree<int> tree;
    tree.enable_log();
    std::vector vec{1, 2, 5, 7, 8, 11, 14, 15};

    for (auto &elem: vec) {
        tree.insert(elem);
    }

    int num4 = 4;
    tree.insert(num4);

    tree.erase(num4);
    int num11 = 11;
    tree.erase(num11);
    int num7 = 7;
    tree.erase(num7);
}
