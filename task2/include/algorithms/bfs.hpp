#pragma once

#include <cassert>
#include <deque>
#include <optional>
#include <unordered_map>
#include "graph/graph.hpp"

namespace Algorithms {

using namespace Graphs;

template <typename GraphT>
class BFS {};

template <typename T>
class BFS<DirectedGraph<T>> final {
public:
    enum class BFSColor {
        White,
        Gray,
        Black
    };

    BFS(const DirectedGraph<T> &graph, Index start_idx) {
        for (const auto &[idx, val]: graph) {
            m_vert_colors[idx] = BFSColor::White;
            m_vert_distance[idx] = std::nullopt;
            m_vert_preds[idx] = std::nullopt;
        }

        m_vert_colors[start_idx] = BFSColor::Gray;
        m_vert_distance[start_idx] = 0;
        std::deque<Index> bfs_queue;
        bfs_queue.emplace_back(start_idx);

        while (!bfs_queue.empty()) {
            Index vert = bfs_queue.front();
            bfs_queue.pop_front();

            for (const auto &adj_vert: graph.get_adjacent(vert)) {
                if (m_vert_colors[adj_vert] == BFSColor::White) {
                    m_vert_colors[adj_vert] = BFSColor::Gray;
                    // TODO: change this for something meaningfull
                    assert(m_vert_distance[vert].has_value());
                    m_vert_distance[adj_vert] = m_vert_distance[vert].value() + 1;
                    m_vert_preds[adj_vert] = vert;
                    bfs_queue.emplace_back(adj_vert);
                }
            }

            m_vert_colors[vert] = BFSColor::Black;
        }
    }

private:
    // Color of each vertex
    std::unordered_map<Index, BFSColor> m_vert_colors;
    // Distance to each vertex from given vertex
    std::unordered_map<Index, Distance> m_vert_distance;
    // Predecessor of each vertex
    std::unordered_map<Index, Predecessor> m_vert_preds;
};

} // namespace Algorithms
