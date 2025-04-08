#include "graph/graph.hpp"
#include "algorithms/johnson.hpp"

#include <gtest/gtest.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/random.hpp>
#include <random>

using namespace Graphs;
using namespace Algorithms;
using namespace boost;

namespace Tests {

using Graph = adjacency_list<vecS, vecS, directedS,
                    property<vertex_name_t, int>,
                    property<edge_weight_t, int> >;
using Vertex = graph_traits<Graph>::vertex_descriptor;

TEST(Johnson_tests, basic_test) {
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
            if (distance_matrix[i][j] == std::numeric_limits<int>::max()) {
                assert(johnson.get_shortest_path(i, j).is_inf());
            } else {
                assert(distance_matrix[i][j] == johnson.get_shortest_path(i, j).m_val.value());
            }
        }
    }
}

TEST(Johnson_tests, random_test) {
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int num_vertices = 5; num_vertices < 20; num_vertices++) {
        for (int num_edges = num_vertices; num_edges < num_vertices * (num_vertices - 1) / 2; num_edges++) {
            Graph g;
            generate_random_graph(g, num_vertices, num_edges, rng, false, false);

            std::uniform_int_distribution<int> weight_dist(-5, 30);
            property_map<Graph, edge_weight_t>::type edge_weights = get(edge_weight, g);

            graph_traits<Graph>::edge_iterator ei, ei_end;
            for(std::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
                edge_weights[*ei] = weight_dist(rng);
            }

            std::vector<std::vector<int>> distance_matrix(num_vertices, std::vector<int>(num_vertices));
            bool boost_success = johnson_all_pairs_shortest_paths(g, distance_matrix);

            DirectedGraph<int> graph;
            for (int i = 0; i < num_vertices; i++) {
                graph.insert_vertice(i);
            }

            for (int i = 0; i < num_vertices; i++) {
                for (int j = 0; j < num_vertices; j++) {
                    Vertex src = vertex(i, g);
                    Vertex dest = vertex(j, g);

                    auto [edge_desc, exists] = edge(src, dest, g);

                    if (exists) {
                        int weight = get(edge_weights, edge_desc);
                        graph.insert_edge(i, j, weight);
                    }
                }
            }

            Johnson<DirectedGraph<int>> johnson(graph);

            if (johnson.has_negative_cycle()) {
                assert(boost_success == false);
                continue;
            }

            for (int i = 0; i < num_vertices; ++i) {
                for (int j = 0; j < num_vertices; ++j) {
                    if (distance_matrix[i][j] == std::numeric_limits<int>::max()) {
                        assert(johnson.get_shortest_path(i, j).is_inf());
                    } else {
                        assert(distance_matrix[i][j] == johnson.get_shortest_path(i, j).m_val.value());
                    }
                }
            }
        }
    }
}

}
