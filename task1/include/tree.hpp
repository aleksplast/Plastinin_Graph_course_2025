#pragma once

#include "node.hpp"

#include <functional>

namespace Task1 {

// Class, representing RedBlackTree
template <typename KeyT>
class RedBlackTree {
    using Node = TreeNode<KeyT>;
    using comparator = std::less<KeyT>;

private:
    // Root of the tree
    Node *m_root = nullptr;
    // Size of the tree
    size_t m_size = 0;

    // Rotate RedBlackTree right around given node
    void rotate_right(Node node);
    // Rotate RedBlackTree left around given node
    void rotate_left(Node node);

    // Get successor of the given node
    Node *successor(Node *node);

    // Fixup RedBlackTree after fixup
    // inserted_node - node, that was inserted
    void insert_fixup(Node *inserted_node);
    // Fixup after erasing node
    // deleted_node - node, that was dele
    void erase_fixup(Node *deleted_node);
public:
    // Insert value into the RedBlackTree
    void insert(KeyT value);
    // Erase value from the RedBlackTree
    void erase(KeyT value);
    // Join another tree into this
    void join(RedBlackTree &other);

    // Default constructor
    RedBlackTree(){};
    // Copy constructor
    RedBlackTree(const RedBlackTree &rhs);
    // Move constructor
    RedBlackTree(const RedBlackTree &&rhs);
    // Copy assignment
    RedBlackTree &operator=(const RedBlackTree &rhs);
    // Move assignment
    RedBlackTree &operator=(const RedBlackTree &&rhs);
    // Destructor
    ~RedBlackTree();
};

} //namespace Task1
