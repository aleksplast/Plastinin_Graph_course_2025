#pragma once

#include <boost/numeric/ublas/matrix.hpp>
#include "graph.hpp"

namespace Algorithms {

using namespace Graphs;

template<typename GraphT>
class Ullman final {};

using namespace boost::numeric::ublas;
template<typename T>
class Ullman<DirectedGraph<T>> {
public:
    using MatchingStorageTy = std::vector<std::unordered_map<Index, Index>>;

    Ullman(const DirectedGraph<T> &graph, const DirectedGraph<T> &sub_graph) {
        if (graph.n_vertices() < sub_graph.n_vertices()) {
            return;
        }

        std::unordered_map<Index, size_t> graph_idx2num;
        std::unordered_map<Index, size_t> graph_num2idx;
        std::unordered_map<Index, size_t> sub_graph_idx2num;
        std::unordered_map<Index, size_t> sub_graph_num2idx;

        size_t graph_num = 0;
        for (const auto& [idx, val] : graph.get_vertices()) {
            graph_idx2num[idx] = graph_num;
            graph_num2idx[graph_num++] = idx;
        }

        size_t sub_graph_num = 0;
        for (const auto& [idx, val] : sub_graph.get_vertices()) {
            sub_graph_idx2num[idx] = sub_graph_num;
            sub_graph_num2idx[sub_graph_num++] = idx;
        }

        matrix<int> graph_matrix(graph.n_vertices(), graph.n_vertices());
        matrix<int> sub_graph_matrix(sub_graph.n_vertices(), sub_graph.n_vertices());

        for (size_t i = 0; i < graph_matrix.size1(); ++i) {
            for (size_t j = 0; j < graph_matrix.size2(); ++j) {
                Index i_idx = graph_num2idx[i];
                Index j_idx = graph_num2idx[j];

                auto &i_adj = graph.get_adjacent(i_idx);
                if (i_adj.find(j_idx) != i_adj.end()) {
                    graph_matrix(i, j) = 1;
                } else {
                    graph_matrix(i, j) = 0;
                }
            }
        }

        for (size_t i = 0; i < sub_graph_matrix.size1(); ++i) {
            for (size_t j = 0; j < sub_graph_matrix.size2(); ++j) {
                Index i_idx = sub_graph_num2idx[i];
                Index j_idx = sub_graph_num2idx[j];

                auto &i_adj = sub_graph.get_adjacent(i_idx);
                if (i_adj.find(j_idx) != i_adj.end()) {
                    sub_graph_matrix(i, j) = 1;
                } else {
                    sub_graph_matrix(i, j) = 0;
                }
            }
        }

        std::vector<size_t> graph_in_degree(graph.n_vertices(), 0);
        std::vector<size_t> graph_out_degree(graph.n_vertices(), 0);
        std::vector<size_t> sub_graph_in_degree(sub_graph.n_vertices(), 0);
        std::vector<size_t> sub_graph_out_degree(sub_graph.n_vertices(), 0);

        for (const auto& [edge, weight] : graph.get_edges()) {
            graph_in_degree[graph_idx2num[edge.m_dest]]++;
            graph_out_degree[graph_idx2num[edge.m_src]]++;
        }

        for (const auto& [edge, weight] : sub_graph.get_edges()) {
            sub_graph_in_degree[sub_graph_idx2num[edge.m_dest]]++;
            sub_graph_out_degree[sub_graph_idx2num[edge.m_src]]++;
        }

        matrix<bool> M(sub_graph.n_vertices(), graph.n_vertices());
        for (size_t u = 0; u < sub_graph.n_vertices(); ++u) {
            for (size_t v = 0; v < graph.n_vertices(); ++v) {
                M(u, v) = (sub_graph_out_degree[u] <= graph_out_degree[v]) &&
                        (sub_graph_in_degree[u] <= graph_in_degree[v]);
            }
        }

        std::vector<int> f(sub_graph.n_vertices(), -1);
        std::vector<bool> used(graph.n_vertices(), false);

        std::function<void(size_t)> backtrack;
        backtrack = [&](size_t depth) {
            if (depth == sub_graph.n_vertices()) {
                for (size_t i = 0; i < graph_matrix.size1(); ++i) {
                    for (size_t j = 0; j < graph_matrix.size2(); ++j) {
                        if (graph_matrix(i, j) == 1 && sub_graph_matrix(f[i], f[j]) != 1) {
                            return;
                        }
                    }
                }

                std::unordered_map<Index, Index> matching;
                for (size_t i = 0; i < graph.n_vertices(); ++i) {
                    matching[graph_num2idx[i]] = sub_graph_num2idx[f[i]];
                }
                m_matchings.push_back(matching);
                return;
            }

            for (size_t v = 0; v < graph.n_vertices(); ++v) {
                if (M(depth, v) && !used[v]) {
                    bool consistent = true;
                    for (size_t i = 0; i < depth; ++i) {
                        if (graph_matrix(i, depth) == 1 && sub_graph_matrix(f[i], v) != 1) {
                            consistent = false;
                            break;
                        }
                        if (graph_matrix(depth, i) == 1 && sub_graph_matrix(v, f[i]) != 1) {
                            consistent = false;
                            break;
                        }
                    }

                    if (consistent) {
                        f[depth] = v;
                        used[v] = true;
                        backtrack(depth + 1);
                        used[v] = false;
                    }
                }
            }
        };

        backtrack(0);
    }

    const MatchingStorageTy &get_matchings() const { return m_matchings; }

private:
    MatchingStorageTy m_matchings;
};

} // namespace Algorithms
