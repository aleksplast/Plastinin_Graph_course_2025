#pragma once

#include "sssp.hpp"
#include "graph/graph.hpp"

namespace Algorithms {

template<typename GraphT>
class BellmanFord final : public SSSP<GraphT> {};

template<typename T>
class BellmanFord<DirectedGraph<T>> : public SSSP<DirectedGraph<T>> {
    using SSSP<DirectedGraph<T>>::relax;
    using SSSP<DirectedGraph<T>>::m_sssp_info;

public:
    // Bellman-Ford algo, finds shortest path from source to all other vertices,
    // can detect presence of the negative cycles
    BellmanFord(const DirectedGraph<T> &graph, Index source) :
    SSSP<DirectedGraph<T>>(graph, source) {
        for (size_t i = 1; i < graph.n_edges() - 1; i++) {
            for (const auto &[edge, weight]: graph.get_edges()) {
                relax(graph, edge.m_src, edge.m_dest);
            }
        }

        for (const auto &[edge, weight]: graph.get_edges()) {
            SSSPVertexInfo &src_info = m_sssp_info[edge.m_src];
            SSSPVertexInfo &dest_info = m_sssp_info[edge.m_dest];

            if (dest_info.estimate > src_info.estimate + weight) {
                m_has_negative_cycle = true;
                break;
            }
        }
    }

    bool has_negative_cycle() const {
        return m_has_negative_cycle;
    }

private:
    bool m_has_negative_cycle = false;
};

} // namespace Algorithms
