#pragma once

#include "node.hpp"

#include <cassert>
#include <cstddef>
#include <fstream>
#include <functional>
#include <tuple>

namespace Task1 {

// Class, representing RedBlackTree
template <typename KeyT, typename Comparator = std::less<KeyT>>
class RedBlackTree final {
    using Node = TreeNode<KeyT>;

public:
    // Default constructor
    RedBlackTree() = default;
    // Copy constructor
    RedBlackTree(const RedBlackTree &rhs);
    // Move constructor
    RedBlackTree(RedBlackTree &&rhs);
    // Copy assignment
    RedBlackTree &operator=(const RedBlackTree &rhs);
    // Move assignment
    RedBlackTree &operator=(RedBlackTree &&rhs);
    // Destructor
    ~RedBlackTree();

private:
    // Root of the tree
    Node* m_root = nullptr;
    // Size of the tree
    size_t m_size = 0;
    // File with log
    bool m_log = false;
    // Log file counter
    uint64_t m_log_cnt;
    // Log file name
    std::string m_log_name = "";

    // Rotate RedBlackTree right around given node
    void rotate_right(Node *node);
    // Rotate RedBlackTree left around given node
    void rotate_left(Node *node);

    // Find minimum of the tree
    // node - root of the tree
    Node *minimum(Node *node) const;

    // Get successor of the given node
    Node *successor(Node *node) const;

    // Finds Node with key in the tree
    Node *find(Node *node, KeyT &key) const;

    // Fixup RedBlackTree after fixup
    // node - node, that was inserted
    void insert_fixup(Node *node);
    // Fixup after erasing node
    // node - child of the successor of the deleted node
    void erase_fixup(Node *node);

    // Split subtree by the root, contained in key_node
    std::tuple<TreeNode<KeyT> *, bool, TreeNode<KeyT> *>
    split(Node *subroot, Node *key_node);

    // Join two subtrees, where height of the left subtree is greater than right subtree
    Node *join_right(Node *left_subroot, Node *key_node, Node *right_subroot);
    // Join two subtrees, where height of the left subtree is less than right subtree
    Node *join_left(Node *left_subroot, Node *key_node, Node *right_subroot);
    // Join two subtrees
    Node *join(Node *left_subroot, Node *key_node, Node *right_subroot);

    // Merge
    Node *merge(Node *left_subroot, Node *right_subroot);

    // Dump subtree to graphviz, where its root is the subroot
    void dump_to_graphviz(Node *sub_root);
    // Get black height of the node
    uint64_t get_black_height(Node *node);

    // Is node red, for nullptr - false
    bool is_red(Node *node);
    // Is node black, for nullptr - true
    bool is_black(Node *node);


public:
    // Insert value into the RedBlackTree
    void insert(const KeyT &key);
    // Erase node containing key from the RedBlackTree
    void erase(KeyT &key);
    // Erase node from the tree
    void erase(Node *node);

    Node *find(KeyT &key) const;
    // Check, if tree containts given key
    bool contains(KeyT &key) const;

    // Join another tree into this
    void join(RedBlackTree &other);
    // Split the tree by the given key
    // Destroyts tree, returning two trees instead
    std::pair<RedBlackTree, RedBlackTree> split(const KeyT &key);
    // Merge one tree into another
    void merge(RedBlackTree &other);

    // Enable logging
    void enable_log() { m_log = true; }
    // Dump current state of the tree to graphviz and generate png
    void dump_to_graphviz();
    // Set log files to start with log_name
    void set_log_name(std::string &log_name) { m_log_name = log_name; }
}; // class RedBlackTree

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::rotate_right(Node *node) {
    if (!node || !node->get_left()) {
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

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::rotate_left(Node *node) {
    if (!node || !node->get_right()) {
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

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::minimum(Node *node) const {
    while (node->get_left()) {
        node = node->get_left();
    }

    return node;
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::successor(Node *node) const {
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

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::find(Node *node, KeyT &key) const {
    if (node == nullptr || key == node->get_key()) {
        return node;
    } else if (Comparator()(key, node->get_key())) {
        return find(node->get_left(), key);
    } else {
        return find(node->get_right(), key);
    }
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::insert_fixup(Node *node) {
    if (node == m_root) {
        node->set_color(Color::Black);
        return;
    }

    Node *parent = node->get_parent();
    while (parent && parent->get_color() == Color::Red) {
        Node *parent_parent = parent->get_parent();

        if (parent_parent->get_left() == parent) {
            Node *parent_parent_right = parent_parent->get_right();

            if (parent_parent_right && parent_parent_right->get_color() == Color::Red) {
                parent->set_color(Color::Black);
                parent_parent_right->set_color(Color::Black);
                parent_parent->set_color(Color::Red);
                node = parent_parent;
            } else if (node == parent->get_right()) {
                node = parent;
                rotate_left(node);
            }

            parent = node->get_parent();
            if (parent) {
                parent->set_color(Color::Black);
                parent_parent = parent->get_parent();
                if (parent_parent) {
                    parent_parent->set_color(Color::Red);
                    rotate_right(parent_parent);
                }
            }
        } else {
            Node *parent_parent_left = parent_parent->get_left();

            if (parent_parent_left && parent_parent_left->get_color() == Color::Red) {
                parent->set_color(Color::Black);
                parent_parent_left->set_color(Color::Black);
                parent_parent->set_color(Color::Red);
                node = parent_parent;
            } else if (node == parent->get_left()) {
                node = parent;
                rotate_right(node);
            }

            parent = node->get_parent();
            if (parent) {
                parent->set_color(Color::Black);
                parent_parent = parent->get_parent();
                if (parent_parent) {
                    parent_parent->set_color(Color::Red);
                    rotate_left(parent_parent);
                }
            }
        }
    }

    m_root->set_color(Color::Black);
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::erase_fixup(Node *node) {
    while (node && node != m_root && node->get_color() == Color::Black) {
        Node *parent = node->get_parent();
        if (node == parent->get_left()) {
            Node *parent_right = parent->get_right();
            if (parent_right->get_color() == Color::Red) {
                parent_right->set_color(Color::Black);
                parent->set_color(Color::Red);
                rotate_left(parent);
                parent_right = parent->get_right();
            }

            if (parent_right->get_left() && parent_right->get_left()->get_color() == Color::Black &&
                parent_right->get_right() && parent_right->get_right()->get_color() == Color::Black) {
                parent_right->set_color(Color::Red);
                rotate_right(parent_right);
                parent_right = parent->get_right();
            } else if (parent_right->get_right() && parent_right->get_right()->get_color() == Color::Black) {
                if (parent_right->get_left()) {
                    parent_right->get_left()->set_color(Color::Black);
                }

                parent_right->set_color(Color::Red);
                rotate_right(parent_right);
                parent_right = parent->get_right();
            }

            parent_right->set_color(parent->get_color());
            parent->set_color(Color::Black);
            if (parent_right->get_right()) {
                parent_right->get_right()->set_color(Color::Black);
            }
            rotate_left(parent);
            node = m_root;
        } else {
            Node *parent_left = parent->get_left();
            if (parent_left->get_color() == Color::Red) {
                parent_left->set_color(Color::Black);
                parent->set_color(Color::Red);
                rotate_right(parent);
                parent_left = parent->get_right();
            }

            if (parent_left->get_left() && parent_left->get_left()->get_color() == Color::Black &&
                parent_left->get_right() && parent_left->get_right()->get_color() == Color::Black) {
                parent_left->set_color(Color::Red);
                rotate_left(parent_left);
                parent_left = parent->get_right();
            } else if (parent_left->get_left() && parent_left->get_left()->get_color() == Color::Black) {
                if (parent_left->get_right()) {
                    parent_left->get_right()->set_color(Color::Black);
                }

                parent_left->set_color(Color::Red);
                rotate_left(parent_left);
                parent_left = parent->get_right();
            }

            parent_left->set_color(parent->get_color());
            parent->set_color(Color::Black);
            if (parent_left->get_left()) {
                parent_left->get_left()->set_color(Color::Black);
            }
            rotate_right(parent);
            node = m_root;
        }
    }
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::insert(const KeyT &key) {
    Node *new_node = new Node(key);
    Node *parent = nullptr;
    Node *curr_node = m_root;

    while (curr_node) {
        parent = curr_node;
        if (Comparator()(new_node->get_key(), curr_node->get_key())) {
            curr_node = curr_node->get_left();
        } else {
            curr_node = curr_node->get_right();
        }
    }

    new_node->set_parent(parent);

    if (!parent) {
        m_root = new_node;
    } else if (Comparator()(new_node->get_key(), parent->get_key())) {
        parent->set_left(new_node);
    } else {
        parent->set_right(new_node);
    }

    m_size += 1;

    dump_to_graphviz();

    insert_fixup(new_node);

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::erase(KeyT &key) {
    Node *node = find(key);

    if (!node) {
        return;
    }
    erase(node);
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::erase(Node *node) {
    Node *succ = nullptr;
    Node *succ_child = nullptr;
    if (!node->get_left() || !node->get_right()) {
        succ = node;
    } else {
        succ = successor(node);
    }

    if (succ->get_left()) {
        succ_child = succ->get_left();
    } else {
        succ_child = succ->get_right();
    }

    Node *succ_parent = succ->get_parent();
    if (succ_child) {
        succ_child->set_parent(succ_parent);
    }

    if (!succ_parent) {
        m_root = succ_child;
    } else if (succ == succ_parent->get_left()) {
        succ_parent->set_left(succ_child);
    } else {
        succ_parent->set_right(succ_child);
    }

    if (succ != node) {
        node->copy_data(succ);
    }

    dump_to_graphviz();

    if (succ->get_color() == Color::Black) {
        erase_fixup(succ_child);
    }

    m_size -= 1;
    delete succ;

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::find(KeyT &key) const {
    return find(m_root, key);
}

template <typename KeyT, typename Comparator>
bool RedBlackTree<KeyT, Comparator>::contains(KeyT &key) const {
    if (find(m_root, key)) {
        return true;
    } else {
        return false;
    }
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join_right(Node *left_subroot, Node *key_node, Node *right_subroot) {
    if (is_black(left_subroot) && get_black_height(left_subroot) == get_black_height(right_subroot)) {
        key_node->set_childs(left_subroot, right_subroot);
        key_node->set_parent(nullptr);
        key_node->set_color(Color::Red);
        return key_node;
    }

    Node *right_join = join_right(left_subroot->get_right(), key_node, right_subroot);
    right_join->set_parent(left_subroot);
    left_subroot->set_right(right_join);
    left_subroot->set_parent(nullptr);
    if (is_black(left_subroot) && is_red(left_subroot->get_right()) && is_red(left_subroot->get_right()->get_right())) {
        left_subroot->get_right()->get_right()->set_color(Color::Black);
        rotate_left(left_subroot);
        return left_subroot->get_parent();
    }

    return left_subroot;
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join_left(Node *left_subroot, Node *key_node, Node *right_subroot) {
    if (is_black(right_subroot) && get_black_height(left_subroot) == get_black_height(right_subroot)) {
        key_node->set_childs(left_subroot, right_subroot);
        key_node->set_parent(nullptr);
        key_node->set_color(Color::Red);
        return key_node;
    }

    Node *left_subroot_join = join_left(left_subroot, key_node, right_subroot->get_left());
    left_subroot_join->set_parent(right_subroot);
    right_subroot->set_left(left_subroot_join);
    right_subroot->set_parent(nullptr);
    if (is_black(right_subroot) && is_red(right_subroot->get_left()) && is_red(right_subroot->get_left()->get_left())) {
        right_subroot->get_left()->get_left()->set_color(Color::Black);
        rotate_right(right_subroot);
        return right_subroot->get_parent();
    }

    return right_subroot;
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join(Node *left_subroot, Node *key_node, Node *right_subroot) {
    if (get_black_height(left_subroot) > get_black_height(right_subroot)) {
        Node *new_sub_root = join_right(left_subroot, key_node, right_subroot);

        if (is_red(new_sub_root) && is_red(new_sub_root->get_right())) {
            new_sub_root->set_color(Color::Black);
        }
        return new_sub_root;
    } else if (get_black_height(left_subroot) < get_black_height(right_subroot)) {
        Node *new_sub_root = join_left(left_subroot, key_node, right_subroot);

        if (is_red(new_sub_root) && is_red(new_sub_root->get_left())) {
            new_sub_root->set_color(Color::Black);
        }
        return new_sub_root;
    } else if (left_subroot && right_subroot && is_black(left_subroot) && is_black(right_subroot)) {
        key_node->set_childs(left_subroot, right_subroot);
        key_node->set_color(Color::Red);
        key_node->set_parent(nullptr);
        return key_node;
    }

    if (left_subroot && right_subroot && left_subroot->get_color() != right_subroot->get_color()) {
        left_subroot->set_color(Color::Black);
        right_subroot->set_color(Color::Black);
    }
    key_node->set_childs(left_subroot, right_subroot);
    key_node->set_color(Color::Black);
    key_node->set_parent(nullptr);
    return key_node;
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::merge(RedBlackTree &other) {
    m_root = merge(m_root, other.m_root);
    m_root->set_color(Color::Black);
    other.m_root = nullptr;

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::merge(Node *left_subroot, Node *right_subroot) {
    if (!left_subroot) {
        return right_subroot;
    } else if (!right_subroot) {
        return left_subroot;
    }

    std::tuple<Node *, bool, Node*> split_res = split(left_subroot, right_subroot);
    dump_to_graphviz(std::get<0>(split_res));
    dump_to_graphviz(std::get<2>(split_res));
    Node *new_left = merge(std::get<0>(split_res), right_subroot->get_left());
    Node *new_right = merge(std::get<2>(split_res), right_subroot->get_right());

    return join(new_left, right_subroot, new_right);
}


template <typename KeyT, typename Comparator>
std::pair<RedBlackTree<KeyT, Comparator>, RedBlackTree<KeyT, Comparator>>
RedBlackTree<KeyT, Comparator>::split(const KeyT &key) {
    Node key_node{key};
    std::tuple<Node *, bool, Node *> result = split(m_root, &key_node);

    RedBlackTree left_tree;
    RedBlackTree right_tree;

    left_tree.m_root = std::get<0>(result);
    right_tree.m_root = std::get<2>(result);

    return std::pair<RedBlackTree, RedBlackTree> (left_tree, right_tree);
}

template <typename KeyT, typename Comparator>
std::tuple<TreeNode<KeyT> *, bool, TreeNode<KeyT> *>
RedBlackTree<KeyT, Comparator>::split(Node *subroot, Node *key_node) {
    const KeyT &key = key_node->get_key();
    if (!subroot) {
        return std::tuple<Node *, bool, Node *> {nullptr, false, nullptr};
    }
    if (key_node->get_key() == subroot->get_key()) {
        return std::tuple<Node *, bool, Node *> {subroot->get_left(), true, subroot->get_right()};
    }

    if (Comparator()(key, subroot->get_key())) {
        std::tuple<Node *, bool, Node *> temp_split = split(subroot->get_left(), key_node);
        Node *left = std::get<0>(temp_split);
        bool success = std::get<1>(temp_split);
        Node *right = std::get<2>(temp_split);
        auto result = std::make_tuple(left, success, join(right, subroot, subroot->get_right()));
        return result;
    }

    std::tuple<Node *, bool, Node *> temp_split = split(subroot->get_right(), key_node);
    Node *left = std::get<0>(temp_split);
    bool success = std::get<1>(temp_split);
    Node *right = std::get<2>(temp_split);
    auto result = std::make_tuple(join(subroot->get_left(), subroot, left), success, right);
    return result;
}

template <typename KeyT, typename Comparator>
uint64_t RedBlackTree<KeyT, Comparator>::get_black_height(Node *node) {
    if (!node) {
        return 0;
    } else {
        return node->get_black_height();
    }
}

template <typename KeyT, typename Comparator>
bool RedBlackTree<KeyT, Comparator>::is_red(Node *node) {
    if (!node) {
        return false;
    } else {
        return node->get_color() == Color::Red;
    }
}

template <typename KeyT, typename Comparator>
bool RedBlackTree<KeyT, Comparator>::is_black(Node *node) {
    if (!node) {
        return true;
    } else {
        return node->get_color() == Color::Black;
    }
}

template <typename KeyT, typename Comparator>
RedBlackTree<KeyT, Comparator>::RedBlackTree(const RedBlackTree &rhs) {
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

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
RedBlackTree<KeyT, Comparator>::RedBlackTree(RedBlackTree &&rhs) {
    std::swap(m_root, rhs.m_root);
    std::swap(m_root, rhs.m_size);
}

template <typename KeyT, typename Comparator>
RedBlackTree<KeyT, Comparator> &RedBlackTree<KeyT, Comparator>::operator=(const RedBlackTree &rhs) {
    RedBlackTree<KeyT, Comparator> temp(rhs);
    *this = std::move(rhs);

    return *this;
}

template <typename KeyT, typename Comparator>
RedBlackTree<KeyT, Comparator> &RedBlackTree<KeyT, Comparator>::operator=(RedBlackTree &&rhs) {
    std::swap(m_root, rhs.m_root);
    std::swap(m_root, rhs.m_size);

    return *this;
}

template <typename KeyT, typename Comparator>
RedBlackTree<KeyT, Comparator>::~RedBlackTree() {
    Node *curr_node = m_root;

    while (curr_node) {
        if (curr_node->get_left()) {
            curr_node = curr_node->get_left();
        } else if (curr_node->get_right()){
            curr_node = curr_node->get_right();
        } else {
            if (curr_node == m_root) {
                delete curr_node;
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

template<typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::dump_to_graphviz() {
    dump_to_graphviz(m_root);
}

template<typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::dump_to_graphviz(Node *sub_root) {
    if (!m_log) {
        return;
    }

    std::string log_num = m_log_name + std::to_string(m_log_cnt);
    std::string graphviz_name(log_num + ".dot");
    std::string picture_name(log_num + ".png");
    std::ofstream log(graphviz_name);

    log << "strict graph {\n"
        << "\trankdir = TB\n"
        << "\t\"info\" [shape = \"record\", style = \"filled\", fillcolor = \"grey\", label = \"{size = "
        << m_size << "|anchor = "
        << m_root << "}\"]\n";

    uint64_t counter = 1;
    if (sub_root) {
        sub_root->dump_to_graphviz(counter, log);
    }

    log << '}';
    std::string src("dot -Tpng " + graphviz_name + " -o " + picture_name);
    log.close();
    m_log_cnt += 1;
    std::system(src.c_str());
}
} //namespace Task1
