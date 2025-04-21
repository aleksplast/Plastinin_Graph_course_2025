#include <deque>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#include <fstream>

struct Edge final {
    int m_src;
    int m_dest;

    bool operator==(const Edge &other) const = default;
};

namespace std {
    // Hash for the edge
    template<>
    struct hash<Edge> {
        size_t operator()(const Edge& p) const {
            uintmax_t hash = std::hash<int>{}(p.m_src);
            hash <<= sizeof(uintmax_t) * 4;
            hash ^= std::hash<int>{}(p.m_dest);
            return std::hash<uintmax_t>{}(hash);
        }
    };
}

using Index = int;
using AdjacencyLists = std::unordered_map<Index, std::unordered_set<Index>>;

// BipartiteGraph struct, all edges are considered undirected
class BipartiteGraph final {
private:
    // Adjacency list for each vertex
    AdjacencyLists m_adjacency_lists;
    // Edges in BipartiteGraph, src is in Left part
    std::unordered_set<Edge> m_edges;

    // Size of Left Part
    int m_left_size = 0;
    // Size of Right Part
    int m_right_size = 0;

    // Is logging enabled
    bool m_log = false;
    // Number of log dumps already done
    uint64_t m_log_cnt = 0;
    // Log file name starting string
    std::string m_log_name = "";

public:
    BipartiteGraph(int left_size, int right_size) : m_left_size(left_size), m_right_size(right_size) {
        m_adjacency_lists.reserve(left_size + right_size);

        for (int i = 1; i < left_size + right_size + 1; ++i) {
            insert_vertice(i);
        }
    };

    // Insert vertex into the BipartiteGraph
    void insert_vertice(const Index vertice) {
        m_adjacency_lists[vertice];
    }

    // Erase vertex by index from the BipartiteGraph
    void erase_vertice(Index idx) {
        if (m_adjacency_lists.find(idx) == m_adjacency_lists.end()) {
            return;
        }

        bool in_left = is_in_left(idx);
        if (in_left) {
            for (auto adj_idx: get_adjacent(idx)) {
                m_adjacency_lists[adj_idx].erase(idx);
                m_edges.erase({idx, adj_idx});
            }
        } else {
           for (auto adj_idx: get_adjacent(idx)) {
                m_adjacency_lists[adj_idx].erase(idx);
                m_edges.erase({adj_idx, idx});
            }
        }

        m_adjacency_lists.erase(idx);
    }

    // Insert edge from src to dest with given weight into the BipartiteGraph
    void insert_edge(const Index src, const Index dest) {
        if (m_adjacency_lists.find(src) == m_adjacency_lists.end() ||
            m_adjacency_lists.find(dest) == m_adjacency_lists.end()) {
            return;
        }

        auto &src_adj_list = m_adjacency_lists[src];
        auto &dest_adj_list = m_adjacency_lists[dest];
        src_adj_list.emplace(dest);
        dest_adj_list.emplace(src);
        m_edges.insert({src, dest});
    }

    // Erase edge from src to dest from the BipartiteGraph
    void erase_edge(const Index src, const Index dest) {
        auto &src_adj_list = m_adjacency_lists[src];
        auto &dest_adj_list = m_adjacency_lists[dest];
        src_adj_list.erase(dest);
        dest_adj_list.erase(src);
        m_edges.erase({src, dest});
    };

    // Number of vertices in BipartiteGraph
    size_t n_vertices() const { return m_adjacency_lists.size(); };
    // Is BipartiteGraph empty
    bool empty() const { return m_adjacency_lists.empty(); };

    // Get Indexes of vertices adjacent to given one
    const std::unordered_set<Index> &get_adjacent(const Index vert_idx) const {
        return m_adjacency_lists.at(vert_idx);
    }

    // Get vertices of the BipartiteGraph
    const AdjacencyLists &get_vertices() const {
        return m_adjacency_lists;
    }

    const std::unordered_set<Edge> &get_edges() const {
        return m_edges;
    }

    // Check if vertex is in left part of Bipartite Graph
    bool is_in_left(const Index idx) const {
        return idx - m_left_size <= 0;
    }

    // Check if vertex is in right part of Bipartite Graph
    bool is_in_right(const Index idx) const {
        return idx - m_left_size > 0;
    }

    // Enable logging
    void enable_log(const std::string &log_name) {
        m_log = true;
        m_log_name = log_name;
    }

    // Dump BipartiteGraph state to Graphviz
    void dump_to_graphviz() {
        if (!m_log) {
            return;
        }

        std::string log_num = m_log_name + std::to_string(m_log_cnt);
        std::string graphviz_name(log_num + ".dot");
        std::string picture_name(log_num + ".png");
        std::ofstream log(graphviz_name);

        log << "diBipartiteGraph dot {\n"    \
               "\trankdir = TB\n"   \
               "\t\"info\" [shape = \"record\", style = \"filled\", fillcolor = \"grey\", label = \"{n_vertices = "
            << n_vertices() << "}\"]\n";

        for (auto &[idx, val]: m_adjacency_lists) {
            log << "\t\"vertex" << idx
                << "\" [shape = \"circle\", style = \"filled\", fillcolor = \""
                << "Grey\", label = \"" << idx << "\"]\n";

        }
        for (auto &[idx, adj_list]: m_adjacency_lists) {
            for (auto &adj_idx: adj_list) {
                log << "\t\"vertex" << idx << "\" -> \"vertex" << adj_idx << "\""
                    << "[ label = \"" << 1 << "\"]\n";
            }
        }

        log << '}';
        std::string src("dot -Tpng " + graphviz_name + " -o " + picture_name);
        log.close();
        m_log_cnt += 1;
        std::system(src.c_str());
    }
};

class BFS {
public:
    const static int INF = -1;
    const static Index NO_PRED = -1;
    enum class BFSColor {
        WHITE,
        GRAY,
    };

    struct BFSInfo {
        BFSColor color = BFSColor::WHITE;
        int distance = INF;
        Index pred = NO_PRED;
    };

    BFS(const BipartiteGraph &graph, AdjacencyLists &adj_lists, const Index src) {
        m_bfs_info.reserve(graph.n_vertices());

        for (auto &[idx, adj_list]: graph.get_vertices()) {
            m_bfs_info.emplace(idx, BFSInfo());
        }

        m_bfs_info[src] = BFSInfo{BFSColor::GRAY, 0, NO_PRED};

        std::deque<Index> work_q;
        work_q.emplace_back(src);

        while (!work_q.empty()) {
            Index idx = work_q.front();
            work_q.pop_front();

            for (auto &adj_idx: adj_lists[idx]) {
                if (m_bfs_info[adj_idx].color == BFSColor::WHITE) {
                    int adj_dist = m_bfs_info[idx].distance + 1;
                    m_bfs_info[adj_idx] = BFSInfo{BFSColor::GRAY, adj_dist, idx};
                    work_q.emplace_back(adj_idx);
                }
            }
        }
    }

private:
    std::unordered_map<Index, BFSInfo> m_bfs_info;
};

class DFS {
public:
    const static int INF = -1;
    const static Index NO_PRED = -1;
    enum class DFSColor {
        WHITE,
        GRAY,
    };

    struct DFSInfo {
        DFSColor color = DFSColor::WHITE;
        int m_start_time = INF;
        int m_end_time = INF;
        Index pred = NO_PRED;
    };

    DFS(const BipartiteGraph &graph, AdjacencyLists &adj_lists, std::function<bool(const Index idx)> should_start) {
        m_dfs_info.reserve(graph.n_vertices());

        for (auto &[idx, adj_list]: graph.get_vertices()) {
            m_dfs_info.emplace(idx, DFSInfo());
        }

        for (auto &[idx, adj_list]: graph.get_vertices()) {
            if (m_dfs_info[idx].color == DFSColor::WHITE && should_start(idx)) {
                DFS_visit(graph, adj_lists, idx);
            }
        }
    }

    bool is_visited(const Index idx) {
        return m_dfs_info[idx].m_start_time != INF;
    }

private:
    void DFS_visit(const BipartiteGraph &graph, AdjacencyLists &adj_lists, const Index idx) {
        m_dfs_time += 1;
        m_dfs_info[idx].color = DFSColor::GRAY;
        m_dfs_info[idx].m_start_time = m_dfs_time;

        for (auto &adj_idx: adj_lists[idx]) {
            if (m_dfs_info[adj_idx].color == DFSColor::WHITE) {
                m_dfs_info[adj_idx].pred = idx;
                DFS_visit(graph, adj_lists, adj_idx);
            }
        }

        m_dfs_time += 1;
        m_dfs_info[idx].m_end_time = m_dfs_time;
    }


private:
    int m_dfs_time = 0;
    std::unordered_map<Index, DFSInfo> m_dfs_info;
};

class FordFulkerson {

};

class MaxMatching {
public:
    MaxMatching(BipartiteGraph &graph) {

    }

    const std::unordered_set<Edge> &get_max_matching_edges() const {
        return m_max_matching_edges;
    }

    const std::unordered_set<Index> &get_saturated_vertices() const {
        return m_saturated_vertices;
    }

private:
    std::unordered_set<Edge> m_max_matching_edges;
    std::unordered_set<Index> m_saturated_vertices;
};

class MinVertCover {
    MinVertCover(BipartiteGraph &graph) {
        MaxMatching max_matching(graph);
        const std::unordered_set<Edge> &max_matching_edges = max_matching.get_max_matching_edges();
        const std::unordered_set<Index> &saturated_vertices = max_matching.get_saturated_vertices();

        OrientEdges(graph, max_matching_edges);

        DFS dfs(graph, m_min_cover_adj_list, [&graph, &saturated_vertices](const Index idx) {
            return graph.is_in_left(idx) && saturated_vertices.find(idx) == saturated_vertices.end();
        });

        for (auto &[idx, adj_list]: graph.get_vertices()) {
            if ((graph.is_in_left(idx) && !dfs.is_visited(idx)) ||
                (graph.is_in_right(idx) && dfs.is_visited(idx))) {
                m_min_cover.emplace(idx);
            }
        }
    }

    void OrientEdges(const BipartiteGraph &graph, const std::unordered_set<Edge> &max_matching_edges) {
        for (auto &edge: graph.get_edges()) {
            if (max_matching_edges.find(edge) != max_matching_edges.end()) {
                m_min_cover_adj_list[edge.m_dest].emplace(edge.m_src);
            } else {
                m_min_cover_adj_list[edge.m_src].emplace(edge.m_dest);
            }
        }
    }

    const std::unordered_set<Index> &get_min_cover_vertices() const {
        return m_min_cover;
    }

private:
    AdjacencyLists m_min_cover_adj_list;
    std::unordered_set<Index> m_min_cover;
};

class Solver {
public:
    void handle_request(BipartiteGraph &graph, int type) {
        if (type == 1) {
            handle_first_type(graph);
        } else {
            handle_second_type(graph);
        }
    }

    void handle_first_type(BipartiteGraph &graph) {
        std::cout << "Handling first type\n";
    }

    void handle_second_type(BipartiteGraph &graph) {
        std::cout << "Handling second type\n";
    }
};

int main() {
    int n1, n2, m, q;

    std::cin >> n1 >> n2 >> m >> q;
    std::cout << "n1 = " << n1 << '\n';
    std::cout << "n2 = " << n2 << '\n';
    std::cout << "m = " << m << '\n';
    std::cout << "q = " << q << '\n';
    BipartiteGraph graph(n1, n2);
    graph.enable_log("BipartiteGraph_log");

    for (int i = 0; i < m; ++i) {
        int src, dest;
        std::cin >> src >> dest;
        graph.insert_edge(src, dest + n1);
        std::cout << "src = " << src << '\n';
        std::cout << "dest = " << dest << '\n';
    }
    graph.dump_to_graphviz();

    Solver solver;
    for (int i = 0; i < q; ++i) {
        int type;

        std::cin >> type;
        solver.handle_request(graph, type);

        std::cout.flush();
    }
}
