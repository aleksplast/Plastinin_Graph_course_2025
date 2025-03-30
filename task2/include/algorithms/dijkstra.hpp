#pragma once

#include "graph/utils.hpp"
#include "sssp.hpp"
#include "graph/graph.hpp"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/policies.hpp>
#include <unordered_map>

namespace Algorithms {

template<typename GraphT>
class Dijktra final : public SSSP<GraphT> {};

template<typename T>
class Dijktra<DirectedGraph<T>> : public SSSP<DirectedGraph<T>> {
    using SSSP<DirectedGraph<T>>::relax;
    using SSSP<DirectedGraph<T>>::m_sssp_info;
    using QueueElem = std::pair<Index, SSSPVertexInfo>;

    // Compare class for fibonacci heap
    struct QueueGreater {
        bool operator()(const QueueElem &lhs, const QueueElem &rhs) const {
            return lhs.second.estimate > rhs.second.estimate;
        }
    };

    using Queue = boost::heap::fibonacci_heap<QueueElem, boost::heap::compare<QueueGreater>>;
    using Handle = typename Queue::handle_type;

public:
    // Dijkstra algo, finds shortest path from source to all other vertices
    Dijktra(const DirectedGraph<T> &graph, Index source) :
    SSSP<DirectedGraph<T>>(graph, source) {
        Queue work_queue;
        std::unordered_map<Index, Handle> handles;
        handles.reserve(graph.n_vertices());

        for (auto &sssp_info: m_sssp_info) {
            Handle handle = work_queue.emplace(sssp_info);
            handles.insert({sssp_info.first, handle});
        }

        while (!work_queue.empty()) {
            const QueueElem min_elem = work_queue.top();
            work_queue.pop();
            const Index min_idx = min_elem.first;
            const SSSPVertexInfo &min_info = min_elem.second;

            for (auto &adj_idx: graph.get_adjacent(min_idx)) {
                Weight edge_weight = graph.get_weight(min_idx, adj_idx);

                if (m_sssp_info[adj_idx].estimate > min_info.estimate + edge_weight) {
                    Weight new_weight = min_info.estimate + edge_weight;
                    m_sssp_info[adj_idx].estimate = new_weight;
                    m_sssp_info[adj_idx].pred = min_idx;

                    Handle adj_handle = handles[adj_idx];
                    work_queue.decrease(adj_handle, {adj_idx, SSSPVertexInfo{new_weight, min_idx}});
                }
            }
        }
    }
};

} // namespace Algorithms
