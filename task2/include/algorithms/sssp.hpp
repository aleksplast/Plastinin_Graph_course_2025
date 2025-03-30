#pragma once

#include <cassert>
#include "graph/graph.hpp"
#include "graph/utils.hpp"

namespace Algorithms {

using namespace Graphs;

// Info collected with SSSP algo
struct SSSPVertexInfo {
    Weight estimate;
    Predecessor pred;
};

template <typename GraphT>
class SSSP {};

template <typename T>
class SSSP<DirectedGraph<T>> {
public:
    virtual ~SSSP() = default;

    // SSSP initialization
    SSSP(const DirectedGraph<T> &graph, Index src_idx) {
        for (auto &[idx, val]: graph.get_vertices()) {
            m_sssp_info.insert({idx, SSSPVertexInfo()});
        }
        m_sssp_info[src_idx].estimate = 0;
    }

    // Get path weight, counted by SSSP algo
    Weight get_path_weight(Index dest) {
        return m_sssp_info[dest].estimate;
    }

protected:
    // Try to relax path with edge from first_vert to second_vert
    void relax(const DirectedGraph<T> &graph, Index first_vert, Index second_vert) {
        Weight edge_weight;
        edge_weight = graph.get_weight(first_vert, second_vert);
        SSSPVertexInfo &first_vert_info = m_sssp_info[first_vert];
        SSSPVertexInfo &second_vert_info = m_sssp_info[second_vert];

        if (second_vert_info.estimate > first_vert_info.estimate + edge_weight) {
            second_vert_info.estimate  = first_vert_info.estimate + edge_weight;
            second_vert_info.pred = first_vert;
        }
    }

    std::unordered_map<Index, SSSPVertexInfo> m_sssp_info;
};

} // namespace Algorithms
