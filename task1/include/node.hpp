#pragma once

namespace Task1 {

// Colour of the TreeNode
enum class Color {
    Red,
    Black
};

// Node of the RedBlackTree
template <typename KeyT>
class TreeNode {
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
    Color m_color;
public:
    // Get value hold in the node
    const KeyT &get_key() const { return m_key; }
    // Set color of the node
    void set_color(Color &color) { m_color = color; }

    // Get right child of the node
    TreeNode *get_right() const { return m_right; }
    // Get left child of the node
    TreeNode *get_left() const { return m_left; }
    // Get parent of the node
    TreeNode *get_parent() const { return m_parent; }
    // Set right child of the node
    void set_right(TreeNode *right) {m_right = right; }
    // Set left child of the node
    void set_left(TreeNode *left) { m_left = left; }
    // Set parent of the node
    void set_parent(TreeNode *parent) { m_parent = parent; }

    // TreeNode constructor
    TreeNode(const KeyT &key = 0, const TreeNode<KeyT> parent = nullptr,
            const TreeNode<KeyT> *right = nullptr, const TreeNode<KeyT> *left = nullptr,
            Color color = Color::Black)
    : m_key(key), m_right(right), m_left(left), m_parent(parent), m_color(color) {}

    TreeNode(const TreeNode *other) {
        m_color = other->m_color;
        m_key = other->m_key;
    }
}; // class TreeNode

} // namespace Task1
