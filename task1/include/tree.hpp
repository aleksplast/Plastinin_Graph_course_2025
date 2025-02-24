#pragma once

#include "node.hpp"

#include <fstream>
#include <functional>

namespace Task1 {

// Class, representing RedBlackTree
template <typename KeyT>
class RedBlackTree {
    using Node = TreeNode<KeyT>;
    using comparator = std::less<KeyT>;

private:
    // Root of the tree
    Node* m_root = nullptr;
    // Size of the tree
    size_t m_size = 0;
    // File with log
    bool m_log;
    // Log file counter
    uint64_t m_log_cnt = 0;

    // Rotate RedBlackTree right around given node
    void rotate_right(Node *node) {
        if (!node->get_left()) {
            return;
        }

        Node *left = node->get_left();
        node->set_left(left->get_right());

        if (left->get_right()) {
            left->get_right()->set_parent(node);
        }
        Node *parent = node->get_parent();
        left->set_parent(parent);

        if (!parent) {
            m_root = left;
        } else if (node == parent->get_left()) {
            parent->set_left(left);
        } else {
            parent->set_right(left);
        }

        left->set_right(node);
        node->set_parent(left);
    }
    // Rotate RedBlackTree left around given node
    void rotate_left(Node *node) {
        if (!node->get_right()) {
            return;
        }
        Node *right = node->get_right();
        node->set_right(right->get_left());

        if (right->get_left()) {
            right->get_left()->set_parent(node);
        }
        Node *parent = node->get_parent();
        right->set_parent(parent);

        if (!parent) {
            m_root = right;
        } else if (node == parent->get_left()) {
            parent->set_left(right);
        } else {
            parent->set_right(right);
        }

        right->set_left(node);
        node->set_parent(right);
    }

    // Find minimum of the tree
    // node - root of the tree
    Node *minimum(Node *node) {
        while (node->get_left()) {
            node = node->get_left();
        }

        return node;
    }

    // Get successor of the given node
    Node *successor(Node *node) {
        if (node->get_right()) {
            return minimum(node->get_right());
        }

        Node *parent = node->get_parent();
        while (parent != nullptr && node == parent->get_right()) {
            node = parent;
            parent = parent->get_parent();
        }

        return parent;
    }

    // Fixup RedBlackTree after fixup
    // inserted_node - node, that was inserted
    void insert_fixup(Node *inserted_node);
    // Fixup after erasing node
    // deleted_node - node, that was dele
    void erase_fixup(Node *deleted_node);

    Node *tree_search(Node *node, KeyT &key) {
        if (node == nullptr || key == node->get_key()) {
            return node;
        } else if (comparator(key, node->get_key())) {
            return tree_search(node->get_right(), key);
        } else {
            return tree_search(node->get_left(), key);
        }
    }
public:
    // Insert value into the RedBlackTree
    void insert(KeyT &value);
    // Erase value from the RedBlackTree
    void erase(KeyT &value);
    // Join another tree into this
    void join(RedBlackTree &other);
    // Check, if tree containts given key
    bool contains(KeyT &key) {
        if (tree_search(m_root, key)) {
            return true;
        } else {
            return false;
        }
    }

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

        if (m_log) {
            dump_to_graphviz();
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

    void enable_log() { m_log = true; }

    void dump_to_graphviz() {
        std::string log_num = std::to_string(m_log_cnt);
        std::string graphviz_name(log_num + ".dot");
        std::string picture_name(log_num + ".png");
        std::ofstream log(graphviz_name);

        log << "strict graph {\n"
            << "\trankdir = TB\n"
            << "\t\"info\" [shape = \"record\", style = \"filled\", fillcolor = \"grey\", label = \"{size = "
            << m_size << "|anchor = "
            << m_root << "}\"]\n";

        uint64_t counter = 1;
        m_root->dump_to_graphviz(counter, log);

        log << '}';
        std::string src("dot -Tpng " + graphviz_name + " -o " + picture_name);
        log.close();
        m_log_cnt += 1;
        std::system(src.c_str());
    }
}; // class RedBlackTree

} //namespace Task1
