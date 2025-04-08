#include "graph/graph.hpp"
#include "algorithms/johnson.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>
#include <limits>

using namespace boost;

using namespace Graphs;
using namespace Algorithms;

typedef adjacency_list<vecS, vecS, directedS,
                        property<vertex_name_t, int>,
                        property<edge_weight_t, int> > Graph;
typedef property_map<Graph, edge_weight_t>::type WeightMap;

int main() {
    DirectedGraph<int> graph;
    Index first_idx = graph.insert_vertice(1);
    Index second_idx = graph.insert_vertice(2);
    Index third_idx = graph.insert_vertice(3);
    Index fourth_idx = graph.insert_vertice(4);
    Index fifth_idx = graph.insert_vertice(5);
    Index sixth_idx = graph.insert_vertice(6);

    graph.insert_edge(first_idx, fifth_idx, -1);
    graph.insert_edge(second_idx, first_idx, 1);
    graph.insert_edge(fourth_idx, first_idx, -4);
    graph.insert_edge(second_idx, fourth_idx, 2);
    graph.insert_edge(fourth_idx, fifth_idx, 3);
    graph.insert_edge(fifth_idx, second_idx, 7);
    graph.insert_edge(third_idx, second_idx, 2);
    graph.insert_edge(sixth_idx, second_idx, 5);
    graph.insert_edge(third_idx, sixth_idx, -8);
    graph.insert_edge(sixth_idx, third_idx, 10);

    graph.enable_log("graph");
    graph.dump_to_graphviz();
    Johnson<DirectedGraph<int>> johnson(graph);

    Graph g;
    add_vertex(first_idx, g);
    add_vertex(second_idx, g);
    add_vertex(third_idx, g);
    add_vertex(fourth_idx, g);
    add_vertex(fifth_idx, g);
    add_vertex(sixth_idx, g);

    add_edge(first_idx, fifth_idx, -1, g);
    add_edge(second_idx, first_idx, 1, g);
    add_edge(fourth_idx, first_idx, -4, g);
    add_edge(second_idx, fourth_idx, 2, g);
    add_edge(fourth_idx, fifth_idx, 3, g);
    add_edge(fifth_idx, second_idx, 7, g);
    add_edge(third_idx, second_idx, 2, g);
    add_edge(sixth_idx, second_idx, 5, g);
    add_edge(third_idx, sixth_idx, -8, g);
    add_edge(sixth_idx, third_idx, 10, g);

    std::vector<std::vector<int>> distance_matrix(num_vertices(g), std::vector<int>(num_vertices(g)));
    johnson_all_pairs_shortest_paths(g, distance_matrix);

    for (size_t i = 0; i < num_vertices(g); ++i) {
        for (size_t j = 0; j < num_vertices(g); ++j) {
            std::cout << "Shortest path counted by boost between " << i << " and " << j << " = ";
            if (distance_matrix[i][j] == std::numeric_limits<int>::max()) {
                assert(johnson.get_shortest_path(i, j).is_inf());
                std::cout << "INF\n";
            } else {
                assert(distance_matrix[i][j] == johnson.get_shortest_path(i, j).m_val.value());
                std::cout << distance_matrix[i][j] << '\n';
            }
            std::cout << "Shortest path counted by me between " << i << " and " << j << " = ";
            if (johnson.get_shortest_path(i, j).is_inf()) {
                std::cout << "INF\n";
            } else {
                std::cout << johnson.get_shortest_path(i, j).m_val.value() << '\n';
            }
        }
        std::cout << '\n';
    }
}
