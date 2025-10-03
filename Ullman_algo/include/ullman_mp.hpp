#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include <omp.h>
#include <vector>
#include <unordered_map>
#include "graph.hpp"

namespace Algorithms {

using namespace Graphs;

template<typename GraphT>
class UllmanMP final {};

using namespace boost::numeric::ublas;

template<typename T>
class UllmanMP<DirectedGraph<T>> {
private:
    struct SearchState {
        std::vector<int> f;
        std::vector<bool> used;

        SearchState(size_t sub_size, size_t graph_size)
            : f(sub_size, -1), used(graph_size, false) {}
    };

    matrix<int> graph_matrix;
    matrix<int> sub_graph_matrix;
    matrix<bool> M;
    std::unordered_map<size_t, Index> graph_num2idx;
    std::unordered_map<size_t, Index> sub_graph_num2idx;
    size_t graph_vertices_count;
    size_t sub_graph_vertices_count;

    std::vector<std::unordered_map<Index, Index>> m_matchings;

    void prune(SearchState& state, size_t depth,
                   std::vector<std::unordered_map<Index, Index>>& local_matchings) {
        if (depth == sub_graph_vertices_count) {
            bool valid = true;
            for (size_t i = 0; i < sub_graph_vertices_count && valid; ++i) {
                for (size_t j = 0; j < sub_graph_vertices_count && valid; ++j) {
                    if (sub_graph_matrix(i, j) == 1) {
                        if (state.f[i] < 0 || state.f[j] < 0 ||
                            state.f[i] >= static_cast<int>(graph_vertices_count) ||
                            state.f[j] >= static_cast<int>(graph_vertices_count) ||
                            graph_matrix(state.f[i], state.f[j]) != 1) {
                            valid = false;
                        }
                    }
                }
            }

            if (valid) {
                std::unordered_map<Index, Index> matching;
                for (size_t i = 0; i < sub_graph_vertices_count; ++i) {
                    if (state.f[i] >= 0 && state.f[i] < static_cast<int>(graph_vertices_count)) {
                        matching[sub_graph_num2idx[i]] = graph_num2idx[state.f[i]];
                    }
                }
                local_matchings.push_back(matching);
            }
            return;
        }

        for (size_t v = 0; v < graph_vertices_count; ++v) {
            if (M(depth, v) && !state.used[v]) {
                bool consistent = true;

                for (size_t i = 0; i < depth && consistent; ++i) {
                    if (sub_graph_matrix(i, depth) == 1) {
                        if (state.f[i] < 0 || state.f[i] >= static_cast<int>(graph_vertices_count) ||
                            graph_matrix(state.f[i], v) != 1) {
                            consistent = false;
                        }
                    }
                    if (sub_graph_matrix(depth, i) == 1) {
                        if (state.f[i] < 0 || state.f[i] >= static_cast<int>(graph_vertices_count) ||
                            graph_matrix(v, state.f[i]) != 1) {
                            consistent = false;
                        }
                    }
                }

                if (consistent) {
                    state.f[depth] = static_cast<int>(v);
                    state.used[v] = true;
                    prune(state, depth + 1, local_matchings);
                    state.used[v] = false;
                }
            }
        }
    }

public:
    using MatchingStorageTy = std::vector<std::unordered_map<Index, Index>>;

    UllmanMP(const DirectedGraph<T> &graph, const DirectedGraph<T> &sub_graph) {
        if (graph.n_vertices() < sub_graph.n_vertices()) {
            return;
        }

        std::unordered_map<Index, size_t> graph_idx2num;
        graph_num2idx.clear();
        sub_graph_num2idx.clear();

        size_t graph_num = 0;
        for (const auto& [idx, val] : graph.get_vertices()) {
            graph_idx2num[idx] = graph_num;
            graph_num2idx[graph_num] = idx;
            graph_num++;
        }

        size_t sub_graph_num = 0;
        for (const auto& [idx, val] : sub_graph.get_vertices()) {
            sub_graph_num2idx[sub_graph_num] = idx;
            sub_graph_num++;
        }

        graph_vertices_count = graph.n_vertices();
        sub_graph_vertices_count = sub_graph.n_vertices();

        graph_matrix.resize(graph_vertices_count, graph_vertices_count, false);
        sub_graph_matrix.resize(sub_graph_vertices_count, sub_graph_vertices_count, false);

        #pragma omp parallel for collapse(2)
        for (size_t i = 0; i < graph_vertices_count; ++i) {
            for (size_t j = 0; j < graph_vertices_count; ++j) {
                Index i_idx = graph_num2idx[i];
                Index j_idx = graph_num2idx[j];
                const auto& i_adj = graph.get_adjacent(i_idx);
                graph_matrix(i, j) = (i_adj.find(j_idx) != i_adj.end()) ? 1 : 0;
            }
        }

        #pragma omp parallel for collapse(2)
        for (size_t i = 0; i < sub_graph_vertices_count; ++i) {
            for (size_t j = 0; j < sub_graph_vertices_count; ++j) {
                Index i_idx = sub_graph_num2idx[i];
                Index j_idx = sub_graph_num2idx[j];
                const auto& i_adj = sub_graph.get_adjacent(i_idx);
                sub_graph_matrix(i, j) = (i_adj.find(j_idx) != i_adj.end()) ? 1 : 0;
            }
        }

        std::vector<size_t> graph_in_degree(graph_vertices_count, 0);
        std::vector<size_t> graph_out_degree(graph_vertices_count, 0);
        std::vector<size_t> sub_graph_in_degree(sub_graph_vertices_count, 0);
        std::vector<size_t> sub_graph_out_degree(sub_graph_vertices_count, 0);

        #pragma omp parallel for
        for (size_t i = 0; i < graph_vertices_count; ++i) {
            Index idx = graph_num2idx[i];
            graph_out_degree[i] = graph.get_adjacent(idx).size();
        }

        for (const auto& [edge, weight] : graph.get_edges()) {
            auto it = graph_idx2num.find(edge.m_dest);
            if (it != graph_idx2num.end()) {
                graph_in_degree[it->second]++;
            }
        }

        #pragma omp parallel for
        for (size_t i = 0; i < sub_graph_vertices_count; ++i) {
            Index idx = sub_graph_num2idx[i];
            sub_graph_out_degree[i] = sub_graph.get_adjacent(idx).size();
        }

        M.resize(sub_graph_vertices_count, graph_vertices_count, false);

        #pragma omp parallel for collapse(2)
        for (size_t u = 0; u < sub_graph_vertices_count; ++u) {
            for (size_t v = 0; v < graph_vertices_count; ++v) {
                M(u, v) = (sub_graph_out_degree[u] <= graph_out_degree[v]) &&
                          (sub_graph_in_degree[u] <= graph_in_degree[v]);
            }
        }

        #pragma omp parallel
        {
            std::vector<std::unordered_map<Index, Index>> local_matchings;

            #pragma omp for schedule(dynamic) nowait
            for (size_t v = 0; v < graph_vertices_count; ++v) {
                if (M(0, v)) {
                    SearchState state(sub_graph_vertices_count, graph_vertices_count);
                    state.f[0] = static_cast<int>(v);
                    state.used[v] = true;
                    prune(state, 1, local_matchings);
                }
            }

            #pragma omp critical
            {
                m_matchings.insert(m_matchings.end(),
                                 local_matchings.begin(), local_matchings.end());
            }
        }
    }

    const MatchingStorageTy &get_matchings() const { return m_matchings; }
};

} // namespace Algorithms
