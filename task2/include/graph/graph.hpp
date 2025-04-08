#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <fstream>

#include "utils.hpp"

namespace Graphs {

template <typename T>
class DirectedGraph final {
public:
    using GraphIterator = typename std::unordered_map<Index, T>::iterator;
    using ConstGraphIterator = typename std::unordered_map<Index, T>::const_iterator;

private:
    // Vertices of the graph
    std::unordered_map<Index, T> m_vertices;
    // Adjacency list for each vertex
    std::unordered_map<Index, std::unordered_set<Index>> m_adjacency_lists;
    // Weight map of the graph
    std::unordered_map<Edge, Weight> m_edge_weights;
    // Index to be assigned to the next inserted vertex
    Index next_idx = 0;

    // Is logging enabled
    bool m_log = false;
    // Number of log dumps already done
    uint64_t m_log_cnt = 0;
    // Log file name starting string
    std::string m_log_name = "";

public:
    GraphIterator begin() { return m_vertices.begin(); }
    GraphIterator end() { return m_vertices.end(); }
    ConstGraphIterator begin() const { return m_vertices.cbegin(); }
    ConstGraphIterator end() const { return m_vertices.cend(); }

    DirectedGraph() = default;

    // Insert vertex into the graph
    Index insert_vertice(const T &vertice) {
        m_vertices.emplace(next_idx, vertice);
        m_adjacency_lists.try_emplace(next_idx);

        return next_idx++;
    }

    // Erase vertex by index from the graph
    void erase_vertice(Index idx) {
        if (m_vertices.find(idx) == m_vertices.end()) {
            return;
        }

        auto &node_adj_list = m_adjacency_lists[idx];
        for (auto &dest: node_adj_list) {
            m_edge_weights.erase(Edge(idx, dest));
        }

        for (auto &adj_list: m_adjacency_lists) {
            adj_list.second.erase(idx);
        }

        m_adjacency_lists.erase(idx);
        m_vertices.erase(idx);
    }

    // Insert edge from src to dest with given weight into the graph
    void insert_edge(const Index src, const Index dest, const Weight weight) {
        if (m_vertices.find(src) == m_vertices.end() ||
            m_vertices.find(dest) == m_vertices.end()) {
            return;
        }

        auto &src_adj_list = m_adjacency_lists[src];
        src_adj_list.emplace(dest);

        m_edge_weights.emplace(Edge(src, dest), weight);
    }

    // Erase edge from src to dest from the graph
    void erase_edge(const Index src, const Index dest) {
        if (m_edge_weights.find(Edge{src, dest}) == m_edge_weights.end()) {
            return;
        }
        auto &src_adj_list = m_adjacency_lists[src];
        src_adj_list.erase(dest);
        m_edge_weights.erase(Edge{src,dest});
    };

    // Number of vertices in graph
    size_t n_vertices() const { return m_vertices.size(); };
    // Number of edges in graph
    size_t n_edges() const { return m_edge_weights.size(); };
    // Is graph empty
    bool empty() const { return m_vertices.empty(); };

    // Get weight of an edge from src to dest
    Weight get_weight(const Index src, const Index dest) const {
        return m_edge_weights.at(Edge(src, dest));
    };

    // Get Indexes of vertices adjacent to given one
    const std::unordered_set<Index> &get_adjacent(const Index vert_idx) const {
        return m_adjacency_lists.at(vert_idx);
    }

    // Get edges of the graph
    const std::unordered_map<Edge, Weight> &get_edges() const {
        return m_edge_weights;
    }

    // Get vertices of the graph
    const std::unordered_map<Index, T> &get_vertices() const {
        return m_vertices;
    }

    // Set weight of the edge
    void set_weight(const Edge edge, Weight weight) {
        auto edge_it = m_edge_weights.find(edge);
        if (edge_it == m_edge_weights.end()) {
            return;
        }

        edge_it->second = weight;
    }

    // Enable logging
    void enable_log(const std::string &log_name) {
        m_log = true;
        m_log_name = log_name;
    }

    // Dump graph state to graphviz
    void dump_to_graphviz() {
        if (!m_log) {
            return;
        }

        std::string log_num = m_log_name + std::to_string(m_log_cnt);
        std::string graphviz_name(log_num + ".dot");
        std::string picture_name(log_num + ".png");
        std::ofstream log(graphviz_name);

        log << "digraph dot {\n"    \
               "\trankdir = TB\n"   \
               "\t\"info\" [shape = \"record\", style = \"filled\", fillcolor = \"grey\", label = \"{n_vertices = "
            << n_vertices() << "| n_edges = "
            << m_edge_weights.size() << "}\"]\n";

        for (auto &[idx, val]: m_vertices) {
            log << "\t\"vertex" << idx
                << "\" [shape = \"circle\", style = \"filled\", fillcolor = \""
                << "Grey\", label = \"" << val
                << "\"]\n";

        }
        for (auto &[idx, val]: m_vertices) {
            for (auto &adj_idx: m_adjacency_lists[idx]) {
                log << "\t\"vertex" << idx << "\" -> \"vertex" << adj_idx << "\""
                    << "[ label = \"" << m_edge_weights[Edge(idx, adj_idx)] << "\"]\n";
            }
        }

        log << '}';
        std::string src("dot -Tpng " + graphviz_name + " -o " + picture_name);
        log.close();
        m_log_cnt += 1;
        std::system(src.c_str());
    }
};

} // namespace Graphs
