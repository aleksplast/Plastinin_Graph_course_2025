#pragma once

#include "graph/graph.hpp"
#include "graph/utils.hpp"
#include <unordered_map>

namespace Algorithms {

using namespace Graphs;

template <typename GraphT>
class DFS {};

template <typename T>
class DFS<DirectedGraph<T>> final {
    // Time type for dfs, increases by one with each hop
    using Time = uint64_t;

public:
    enum class DFSColor {
        White,
        Gray,
        Black
    };

    struct DFSVertInfo final {
        DFSColor color;
        Time start_time;
        Time end_time;
        Predecessor pred;
    };

    DFS(const DirectedGraph<T> &graph) {
        for (const auto &[idx, val]: graph) {
            m_vertices_info[idx].color = DFSColor::White;
            m_vertices_info[idx].pred = std::nullopt;
        }

        for (auto &[idx, val]: graph) {
            if (m_vertices_info[idx].color == DFSColor::White) {
                DFSVisit(graph, idx);
            }
        }
    }

    void DFSVisit(const DirectedGraph<T> graph, Index idx) {
        DFSVertInfo &vert_info = m_vertices_info[idx];
        vert_info.color = DFSColor::Gray;
        m_time += 1;
        vert_info.start_time = m_time;

        for (auto &adj_idx: graph.get_adjacent(idx)) {
            DFSVertInfo &adj_vert_info = m_vertices_info[adj_idx];
            if (adj_vert_info.color == DFSColor::White) {
                adj_vert_info.pred = idx;
                DFSVisit(graph, adj_idx);
            }
        }

        vert_info.color = DFSColor::Black;
        m_time += 1;
        vert_info.end_time = m_time;
    };

private:
    // Info about vertices collected by dfs
    std::unordered_map<Index, DFSVertInfo> m_vertices_info;
    // DFS working time
    Time m_time = 0;
};

} // namespace Algorithms
