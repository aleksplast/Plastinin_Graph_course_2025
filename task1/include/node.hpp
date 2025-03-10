#pragma once

#include <cassert>
#include <inttypes.h>
#include <iostream>

#include "side.hpp"

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
  // On which side node is located relative to its parent
  Side m_side = Side::None;
  // Black height of the node
  uint64_t m_height = 0;

public:
  // Get value hold in the node
  const KeyT &get_key() const { return m_key; }
  // Get color of the node
  Color get_color() const { return m_color; }
  // Set color of the node
  void set_color(Color color) { m_color = color; }

  TreeNode *get_child(Side side) {
    TreeNode *child = side == Side::Left ? m_left : m_right;
    return child;
  }
  // Get right child of the node
  TreeNode *get_right() const { return m_right; }
  // Get left child of the node
  TreeNode *get_left() const { return m_left; }
  // Get parent of the node
  TreeNode *get_parent() const { return m_parent; }
  // Set right child of the node
  void set_right(TreeNode *right) {
    m_right = right;
    if (m_right) {
      m_right->set_parent(this);
      m_right->set_side(Side::Right);
    }
  }
  // Set left child of the node
  void set_left(TreeNode *left) {
    m_left = left;
    if (m_left) {
      m_left->set_parent(this);
      m_left->set_side(Side::Left);
    }
  }
  // Set parent of the node
  void set_parent(TreeNode *parent) { m_parent = parent; }
  // Set side of the node
  void set_side(Side side) { m_side = side; }
  // Get side of the node
  Side get_side() const { return m_side; }
  // Set child of the given side of the node
  void set_child(TreeNode *node, Side side) {
    if (side == Side::None) {
      return;
    }

    side == Side::Left ? set_left(node) : set_right(node);
  }
  // Is node located on the right side of the parent
  bool is_right() const { return m_side == Side::Right; }
  // Is node located on the left side of the parent
  bool is_left() const { return m_side == Side::Left; }
  // Get black height of the node
  uint64_t get_height() const { return m_height; }
  // Set black height of the node
  void set_height(uint64_t height) { m_height = height; }

  // TreeNode constructor
  TreeNode(const KeyT &key, TreeNode<KeyT> *parent = nullptr,
           TreeNode<KeyT> *left = nullptr, TreeNode<KeyT> *right = nullptr,
           Color color = Color::Red, Side side = Side::None)
      : m_key(key), m_right(right), m_left(left), m_parent(parent),
        m_color(color), m_side(side) {
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
  // Set childs of the node
  void set_childs(TreeNode *left, TreeNode *right) {
    set_left(left);
    set_right(right);
  }
  // Copy data from other node
  void copy_data(const TreeNode *other) {
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
    std::string side_str;
    if (m_side == Side::None) {
      side_str = "None";
    } else {
      side_str = m_side == Side::Left ? "Left" : "Right";
    }

    log << "\t\"node" << counter
        << "\" [shape = \"circle\", style = \"filled\", fillcolor = \""
        << color_str << "\", label = \""
        << "key = " << m_key << '\n'
        << "height = " << m_height  << '\n'
        << "side = " << side_str << "\"]\n";

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
