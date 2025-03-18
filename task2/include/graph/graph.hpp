#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "utils.hpp"

namespace Graphs {

template <typename T>
class DirectedGraph final {
public:
    using GraphIterator = typename std::unordered_map<Index, T>::iterator;
    using ConstGraphIterator = typename std::unordered_map<Index, T>::const_iterator;
    using Weight = int;

private:
    std::unordered_map<Index, T> m_vertices;
    std::unordered_map<Index, std::unordered_set<Index>> m_adjacency_lists;
    std::unordered_map<Edge, Weight, EdgeHash> m_edge_weights;

public:
    GraphIterator begin() { return m_vertices.begin(); }
    GraphIterator end() { return m_vertices.end(); }
    ConstGraphIterator begin() const { return m_vertices.cbegin(); }
    ConstGraphIterator end() const { return m_vertices.cend(); }

    DirectedGraph() = default;

    void insert_vertice(const T &vertice) {
        Index vert_idx = m_vertices.size();
        m_vertices[vert_idx] = vertice;
        m_adjacency_lists.try_emplace(vert_idx);
    }

    void erase_vertice(Index idx) {
        auto &node_adj_list = m_adjacency_lists[idx];
        for (auto &dest: node_adj_list) {
            m_edge_weights.erase(Edge(idx, dest));
        }

        for (auto &adj_list: m_adjacency_lists) {
            adj_list.second.erase(idx);
        }

        m_adjacency_lists.erase(idx);
    }

    void insert_edge(const Index src, const Index dest, const Weight weight) {
        auto &src_adj_list = m_adjacency_lists[src];
        src_adj_list.emplace(dest);

        m_edge_weights[Edge(src, dest)] = weight;
    }

    void erase_edge(const Index src, const Index dest) {
        auto &src_adj_list = m_adjacency_lists[src];
        auto edge_it = std::find(src_adj_list.begin(), src_adj_list.end(), dest);
        if (edge_it != src_adj_list.end()) {
            src_adj_list.erase(edge_it);
        }

        auto weight_it = m_edge_weights.find(Edge(src, dest));
        if (weight_it != m_edge_weights.end()) {
            m_edge_weights.erase(weight_it);
        }
    };

    size_t n_vertices() const { return m_vertices.size(); };
    size_t n_edges() const { return m_edge_weights.size(); };
    bool empty() const { return m_vertices.empty(); };

    Weight weight(const Index src, const Index dest) const {
        return m_edge_weights.at(Edge(src, dest));
    };

    const std::unordered_set<Index> &get_adjacent(const Index vert_idx) const {
        return m_adjacency_lists.at(vert_idx);
    }
};

} // namespace Graphs
