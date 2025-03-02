#include "tree.hpp"

int main() {
    Task1::RedBlackTree<int> tree1;
    tree1.enable_log();
    std::string log1 = "tree1_log";
    tree1.set_log_name(log1);
    std::vector vec{1, 2, 5, 7, 8, 11, 14, 15};

    for (auto &elem: vec) {
        tree1.insert(elem);
    }

    int num4 = 4;
    tree1.insert(num4);

    tree1.erase(num4);
    int num11 = 11;
    tree1.erase(num11);
    int num7 = 7;
    tree1.erase(num7);

    Task1::RedBlackTree<int> tree2;
    std::string log2 = "tree2_log";
    tree2.set_log_name(log2);
    tree2.enable_log();
    tree2.insert(num7);
    tree2.insert(num4);
    tree2.insert(num11);

    tree1.merge(tree2);
}
