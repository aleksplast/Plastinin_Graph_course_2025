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
    void insert(KeyT &value);
    // Erase value from the RedBlackTree
    void erase(KeyT &value);
    // Join another tree into this
    void join(RedBlackTree &other);

    // Default constructor
    RedBlackTree(){};
    // Copy constructor
    RedBlackTree(const RedBlackTree &rhs) {
        m_root = new Node(rhs.m_root);
        m_size = rhs.m_size;

        Node *curr = m_root;
        Node *rhs_curr = rhs.m_root;

        while (rhs_curr) {
            if (curr->get_left() == nullptr && rhs_curr->get_left()) {
                Node *parent = curr;
                Node *rhs_left = rhs_curr->get_left();
                Node *curr_left = new Node(rhs_left);

                rhs_curr = rhs_left;
                curr->set_left(curr_left);

                curr = curr->get_left();
                curr->set_parent(parent);
            } else if (curr->get_right() == nullptr && rhs_curr->get_right()) {
                Node *parent = curr;
                Node *rhs_right = rhs_curr->get_right();
                Node *curr_right = new Node(rhs_right);

                rhs_curr = rhs_right;
                curr->set_right(curr_right);

                curr = curr->get_right();
                curr->set_parent(parent);
            } else {
                if (curr == m_root) {
                    break;
                } else {
                    curr = curr->get_parent();
                    rhs_curr = rhs_curr->get_parent();
                }
            }
        }
    }
    // Move constructor
    RedBlackTree(RedBlackTree &&rhs) {
        std::swap(m_root, rhs.m_root);
        std::swap(m_root, rhs.m_size);
    }
    // Copy assignment
    RedBlackTree &operator=(const RedBlackTree &rhs) {
        RedBlackTree<KeyT> temp(rhs);
        *this = std::move(rhs);

        return *this;
    }
    // Move assignment
    RedBlackTree &operator=(const RedBlackTree &&rhs) {
        std::swap(m_root, rhs.m_root);
        std::swap(m_root, rhs.m_size);

        return *this;
    }
    // Destructor
    ~RedBlackTree() {
        Node *curr_node = m_root;

        while (curr_node) {
            if (curr_node->get_left()) {
                curr_node = curr_node->get_left();
            } else if (curr_node->get_right()){
                curr_node = curr_node->get_right();
            } else {
                if (curr_node == m_root) {
                    break;
                } else {
                    Node *parent = curr_node->get_parent();
                    if (parent->get_right() == curr_node) {
                        parent->set_right(nullptr);
                    } else if (parent->get_left() == curr_node) {
                        parent->set_left(nullptr);
                    }

                    delete curr_node;
                    curr_node = parent;
                }
            }
        }
    }
}; // class RedBlackTree

} //namespace Task1
