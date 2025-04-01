#pragma once

#include "algorithms/bellman_ford.hpp"
#include "algorithms/dijkstra.hpp"
#include "graph/utils.hpp"
#include "graph/graph.hpp"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/policies.hpp>
#include <unordered_map>

namespace Algorithms {

template<typename GraphT>
class Johnson final {};

template <typename T>
class Johnson<DirectedGraph<T>> {
    struct JohnsonVertexInfo {
        // TODO: think of meaningfull name
        // Value for calculating new weights
        Weight h;
        // Shortest path weight
        std::unordered_map<Index, Weight> path_weights;

        JohnsonVertexInfo(Weight new_h) : h(new_h) {}
        JohnsonVertexInfo() = default;
    };

private:
    // Info about shortest path collected by Johnson algo
    std::unordered_map<Index, JohnsonVertexInfo> m_johnson_info;
    // Flag indicating if graph has negative cycle
    bool m_has_negative_cycle = false;

public:
    Johnson(const DirectedGraph<T> &graph) {
        for (auto &[idx, val]: graph.get_vertices()) {
            for (auto &[other_idx, other_val]: graph.get_vertices()) {
                m_johnson_info[idx].path_weights[other_idx];
            }
        }

        T fict_vert;
        Index fict_idx = graph.insert_vertice(fict_vert);

        for (auto &[idx, val]: graph.get_vertices()) {
            if (idx != fict_idx) {
                graph.insert_edge(fict_idx, idx, 0);
            }
        }

        graph.dump_to_graphviz();

        BellmanFord<DirectedGraph<T>> bellman_ford(graph, fict_idx);
        if (bellman_ford.has_negative_cycle()) {
            m_has_negative_cycle = true;
            return;
        }

        for (auto &[idx, val]: graph.get_vertices()) {
            m_johnson_info[idx].h = bellman_ford.get_path_weight(idx);
        }

        for (auto &[edge, weight]: graph.get_edges()) {
            graph.set_weight(edge, weight + m_johnson_info[edge.m_src].h - m_johnson_info[edge.m_dest].h);
        }

        graph.dump_to_graphviz();

        for (auto &[idx, val]: graph.get_vertices()) {
            Dijktra<DirectedGraph<T>> dijkstra(graph, idx);

            for (auto &[other_idx, other_val]: graph.get_vertices()) {
                m_johnson_info[idx].path_weights[other_idx] = dijkstra.get_path_weight(other_idx) + m_johnson_info[other_idx].h - m_johnson_info[idx].h;
            }
        }

        graph.erase_vertice(fict_idx);

        graph.dump_to_graphviz();
    }

    Weight get_shortest_path(const Index src, const Index dest) const {
        return m_johnson_info.at(src).path_weights.at(dest);
    }

    bool has_negative_cycle() const {
        return m_has_negative_cycle;
    }
};

} // namespace Algorithms
