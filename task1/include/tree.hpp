#pragma once

#include "node.hpp"

#include <cstddef>
#include <fstream>
#include <functional>
#include <thread>
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

    std::tuple<TreeNode<KeyT> *, bool, TreeNode<KeyT> *>
    split(Node *node, const KeyT &key);

    Node *join_right(Node *left_root, const KeyT &key, Node *right);

    Node *join_left(Node *left_root, const KeyT &key, Node *right);

    Node *join(Node *left, const KeyT &key, Node *right);

    Node *merge(Node *left, Node *right);

    void dump_to_graphviz(Node *node);

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
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join_right(Node *left, const KeyT &key, Node *right) {
    if (!left) {
        return new Node (key, nullptr, left, right, Color::Red);
    }
    if (left->get_color() == Color::Black && left->get_black_height() == right->get_black_height()) {
        return new Node(key, left->get_parent(), left, right, Color::Red);
    }

    Node *new_node = new Node(left->get_key(), left->get_parent(), left->get_left(), join_right(left->get_right(), key, right), left->get_color());
    if (!new_node->get_right() || !new_node->get_right()->get_right()) {
        return new_node;
    }
    if (left->get_color() == Color::Black && new_node->get_right()->get_color() == new_node->get_right()->get_right()->get_color()
        && new_node->get_right()->get_color() == Color::Red) {
        new_node->get_right()->get_right()->set_color(Color::Black);
        rotate_left(new_node);
        return new_node->get_parent();
    }

    return new_node;
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join_left(Node *left, const KeyT &key, Node *right) {
    if (!left) {
        return new Node (key, nullptr, left, right, Color::Red);
    }
    if (left->get_color() == Color::Black && left->get_black_height() == right->get_black_height()) {
        return new Node(key, left->get_parent(), left, right, Color::Red);
    }

    Node *new_node = new Node(left->get_key(), left->get_parent(), join_left(left->get_left(), key, right), left->get_right(), left->get_color());
    if (!new_node->get_right() || !new_node->get_left()->get_left()) {
        return new_node;
    }
    if (left->get_color() == Color::Black && new_node->get_left()->get_color() == new_node->get_left()->get_left()->get_color()
        && new_node->get_left()->get_color() == Color::Red) {
        new_node->get_left()->get_left()->set_color(Color::Black);
        rotate_right(new_node);
        return new_node->get_parent();
    }

    return new_node;
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::join(Node *left, const KeyT &key, Node *right) {
    if (!left && !right) {
        return new Node (key, nullptr, left, right, Color::Red);
    } else if (!left) {
        return new Node (key, nullptr, left, right, Color::Red);
    }
    if (right && left->get_black_height() > right->get_black_height()) {
        Node *new_node = join_right(left, key, right);

        if (new_node->get_color() == Color::Red && new_node->get_right()->get_color() == Color::Red) {
            new_node->set_color(Color::Black);
        }

        Node *parent = left->get_parent();
        if (!parent) {
        } else if (left->get_parent()->get_left() == left) {
            left->get_parent()->set_left(new_node);
        } else if (left->get_parent()->get_right() == left) {
            left->get_parent()->set_right(new_node);
        }
        left->set_parent(new_node);
        right->set_parent(new_node);
        return new_node;
    } else if (right && left->get_black_height() < right->get_black_height()) {
        Node *new_node = join_left(left, key, right);

        if (new_node->get_color() == Color::Red && new_node->get_left()->get_color() == Color::Red) {
            new_node->set_color(Color::Black);
        }

        Node *parent = left->get_parent();
        if (!parent) {
        } else if (left->get_parent()->get_left() == left) {
            left->get_parent()->set_left(new_node);
        } else if (left->get_parent()->get_right() == left) {
            left->get_parent()->set_right(new_node);
        }
        left->set_parent(new_node);
        right->set_parent(new_node);
        return new_node;
    } else if (right && left->get_color() == Color::Black && right->get_color() == Color::Black) {
        Node *new_node = new Node(key, left->get_parent(), left, right, Color::Black);
        Node *parent = left->get_parent();
        if (!parent) {
        } else if (parent->get_left() == left) {
            parent->set_left(new_node);
        } else if (left->get_parent()->get_right() == left) {
            parent->set_right(new_node);
        }
        left->set_parent(new_node);
        right->set_parent(new_node);
        return new_node;
    }
    Node *new_node = new Node(key, left->get_parent(), left, right, Color::Black);
    Node *parent = left->get_parent();
    if (!parent) {
    } else if (left->get_parent()->get_left() == left) {
        left->get_parent()->set_left(new_node);
    } else if (left->get_parent()->get_right() == left) {
        left->get_parent()->set_right(new_node);
    }
    left->set_parent(new_node);
    if (right) {
        right->set_parent(new_node);
    }

    return new_node;
}

template <typename KeyT, typename Comparator>
void RedBlackTree<KeyT, Comparator>::merge(RedBlackTree &other) {
    m_root = merge(m_root, other.m_root);

    dump_to_graphviz();
}

template <typename KeyT, typename Comparator>
TreeNode<KeyT> *RedBlackTree<KeyT, Comparator>::merge(Node *left, Node *right) {
    if (!left) {
        return right;
    } else if (!right) {
        return left;
    }

    std::tuple<Node *, bool, Node*> split_res = split(left, right->get_key());
    dump_to_graphviz(std::get<0>(split_res));
    dump_to_graphviz(std::get<2>(split_res));
    Node *new_left = merge(std::get<0>(split_res), right->get_left());
    Node *new_right = merge(std::get<2>(split_res), right->get_right());

    return join(new_left, right->get_key(), new_right);
}

template <typename KeyT, typename Comparator>
std::pair<RedBlackTree<KeyT, Comparator>, RedBlackTree<KeyT, Comparator>>
RedBlackTree<KeyT, Comparator>::split(const KeyT &key) {
    std::tuple<Node *, bool, Node *> result = split(m_root, key);
    // TODO: add split error handling

    RedBlackTree left_tree;
    RedBlackTree right_tree;

    left_tree.m_root = std::get<0>(result);
    right_tree.m_root = std::get<2>(result);
    // TODO: implement size and log and height update

    return std::pair<RedBlackTree, RedBlackTree> (left_tree, right_tree);
}

template <typename KeyT, typename Comparator>
std::tuple<TreeNode<KeyT> *, bool, TreeNode<KeyT> *>
RedBlackTree<KeyT, Comparator>::split(Node *node, const KeyT &key) {
    if (!node) {
        return std::tuple<Node *, bool, Node *> {nullptr, false, nullptr};
    }
    if (key == node->get_key()) {
        return std::tuple<Node *, bool, Node *> {node->get_left(), true, node->get_right()};
    }

    if (Comparator()(key, node->get_key())) {
        std::tuple<Node *, bool, Node *> temp_split = split(node->get_left(), key);
        Node *left = std::get<0>(temp_split);
        bool success = std::get<1>(temp_split);
        Node *right = std::get<2>(temp_split);
        auto result = std::make_tuple(left, success, join(right, node->get_key(), node->get_right()));
        return result;
    }

    std::tuple<Node *, bool, Node *> temp_split = split(node->get_right(), key);
    Node *left = std::get<0>(temp_split);
    bool success = std::get<1>(temp_split);
    Node *right = std::get<2>(temp_split);
    auto result = std::make_tuple(join(node->get_left(), node->get_key(), left), success, right);
    return result;
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
void RedBlackTree<KeyT, Comparator>::dump_to_graphviz(Node *node) {
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
    if (node) {
        node->dump_to_graphviz(counter, log);
    }

    log << '}';
    std::string src("dot -Tpng " + graphviz_name + " -o " + picture_name);
    log.close();
    m_log_cnt += 1;
    std::system(src.c_str());
}
} //namespace Task1
