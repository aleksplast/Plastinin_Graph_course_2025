#pragma once

#include <inttypes.h>
#include <iostream>

namespace Task1 {

// Colour of the TreeNode
enum class Color { Red, Black };

// Node of the RedBlackTree
template <typename KeyT> class TreeNode final {
private:
  // Value hold in the node
  KeyT m_key;

  // Right child of the node
  TreeNode *m_right = nullptr;
  // Left child of the node
  TreeNode *m_left = nullptr;
  // Parent of the node
  TreeNode *m_parent = nullptr;

  // Colour of the node
  Color m_color = Color::Red;

public:
  // Get value hold in the node
  const KeyT &get_key() const { return m_key; }
  // Get color of the node
  Color get_color() const { return m_color; }
  // Set color of the node
  void set_color(Color color) { m_color = color; }

  // Get right child of the node
  TreeNode *get_right() const { return m_right; }
  // Get left child of the node
  TreeNode *get_left() const { return m_left; }
  // Get parent of the node
  TreeNode *get_parent() const { return m_parent; }
  // Set right child of the node
  void set_right(TreeNode *right) { m_right = right; }
  // Set left child of the node
  void set_left(TreeNode *left) { m_left = left; }
  // Set parent of the node
  void set_parent(TreeNode *parent) { m_parent = parent; }

  // TreeNode constructor
  TreeNode(const KeyT &key, TreeNode<KeyT> *parent = nullptr,
           TreeNode<KeyT> *left = nullptr, TreeNode<KeyT> *right = nullptr,
           Color color = Color::Red)
      : m_key(key), m_right(right), m_left(left), m_parent(parent),
        m_color(color) {
    if (m_right) {
      m_right->m_parent = this;
    }
    if (m_left) {
      m_left->m_parent = this;
    }
  }

  // Constructor, with data from other node
  TreeNode(const TreeNode *other)
      : m_color(other->m_color), m_key(other->m_key) {}

  TreeNode(TreeNode *left, TreeNode *right, TreeNode *parent)
      : m_right(right), m_left(left), m_parent(parent), m_color(Color::Black) {}

  // Get height of the node
  // TODO: change this to constant time computations (class member)
  uint64_t get_black_height() {
    uint64_t height = 1;
    TreeNode *curr_node = m_left;
    while (curr_node) {
      if (curr_node->m_color == Color::Black) {
        height += 1;
      }
      curr_node = curr_node->get_left();
    }

    return height;
  }
  // Set childs of the node
  void set_childs(TreeNode *left, TreeNode *right) {
    m_left = left;
    m_right = right;
    if (m_left) {
      m_left->m_parent = this;
    }
    if (m_right) {
      m_right->m_parent = this;
    }
  }
  // Copy data from other node
  void copy_data(const TreeNode *other) {
    m_color = other->m_color;
    m_key = other->m_key;
  }

  // Dump Node to graphviz
  // log - log ostream
  // counter - number visited nodes counter
  // returns number of the node in reverse post order
  uint64_t dump_to_graphviz(uint64_t &counter, std::ostream &log) {
    uint64_t num_left = 0;
    uint64_t num_right = 0;
    if (m_left) {
      num_left = m_left->dump_to_graphviz(counter, log);
    }
    if (m_right) {
      num_right = m_right->dump_to_graphviz(counter, log);
    }

    std::string color_str = m_color == Color::Red ? "Red" : "Grey";
    log << "\t\"node" << counter
        << "\" [shape = \"circle\", style = \"filled\", fillcolor = \""
        << color_str << "\", label = \"" << m_key << "\"]\n";

    if (m_left) {
      log << "\t\"node" << counter << "\" -- \"node" << num_left << "\"\n";
    }
    if (m_right) {
      log << "\t\"node" << counter << "\" -- \"node" << num_right << "\"\n";
    }

    counter += 1;
    return counter - 1;
  }
}; // class TreeNode

} // namespace Task1
