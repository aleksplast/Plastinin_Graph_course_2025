#include <iostream>
#include <chrono>
#include <omp.h>
#include "ullman_mp.hpp"

using namespace Graphs;
using namespace Algorithms;

DirectedGraph<std::string> create_grid_graph(int rows, int cols) {
    DirectedGraph<std::string> graph;

    std::vector<std::vector<Index>> vertex_indices(rows, std::vector<Index>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            vertex_indices[i][j] = graph.insert_vertice(
                "(" + std::to_string(i) + "," + std::to_string(j) + ")"
            );
        }
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (j < cols - 1) {
                graph.insert_edge(vertex_indices[i][j], vertex_indices[i][j + 1], 1.0);
            }
            if (i < rows - 1) {
                graph.insert_edge(vertex_indices[i][j], vertex_indices[i +1 ][j], 1.0);
            }
            if (j > 0) {
                graph.insert_edge(vertex_indices[i][j], vertex_indices[i][j - 1], 1.0);
            }
            if (i > 0) {
                graph.insert_edge(vertex_indices[i][j], vertex_indices[i - 1][j], 1.0);
            }
        }
    }

    return graph;
}

DirectedGraph<std::string> create_complete_graph(int size) {
    DirectedGraph<std::string> graph;

    std::vector<Index> vertices;
    for (int i = 0; i < size; ++i) {
        vertices.push_back(graph.insert_vertice("V" + std::to_string(i)));
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (i != j) {
                graph.insert_edge(vertices[i], vertices[j], 1.0);
            }
        }
    }

    return graph;
}

void run_performance_test() {
    std::cout << "=== Performance Test: Sequential vs Parallel Ullman Algorithm ===\n\n";

    {
        std::cout << "Test 1: Grid Graphs (6x6 vs 3x3)\n";
        auto G = create_grid_graph(6, 6);
        auto H = create_grid_graph(3, 3);

        std::cout << "G: " << G.n_vertices() << " vertices, " << G.n_edges() << " edges\n";
        std::cout << "H: " << H.n_vertices() << " vertices, " << H.n_edges() << " edgess\n";

        omp_set_num_threads(1);
        auto start_seq = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_seq(G, H);
        auto end_seq = std::chrono::high_resolution_clock::now();
        auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();

        omp_set_num_threads(4);
        auto start_par = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_par(G, H);
        auto end_par = std::chrono::high_resolution_clock::now();
        auto duration_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();

        std::cout << "Sequential time: " << duration_seq << " ms\n";
        std::cout << "Parallel time (4 threads): " << duration_par << " ms\n";
        std::cout << "Speedup: " << static_cast<double>(duration_seq) / duration_par << "x\n";
        std::cout << "Found " << ullman_par.get_matchings().size() << " isomorphisms\n\n";
    }

    {
        std::cout << "Test 2: Larger Grid Graphs (8x8 vs 4x4)\n";
        auto G = create_grid_graph(8, 8);
        auto H = create_grid_graph(4, 4);

        std::cout << "G: " << G.n_vertices() << " vertices, " << G.n_edges() << " edges\n";
        std::cout << "H: " << H.n_vertices() << " vertices, " << H.n_edges() << " edges\n";

        omp_set_num_threads(1);
        auto start_seq = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_seq(G, H);
        auto end_seq = std::chrono::high_resolution_clock::now();
        auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();

        omp_set_num_threads(4);
        auto start_par = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_par(G, H);
        auto end_par = std::chrono::high_resolution_clock::now();
        auto duration_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();

        std::cout << "Sequential time: " << duration_seq << " ms\n";
        std::cout << "Parallel time (4 threads): " << duration_par << " ms\n";
        std::cout << "Speedup: " << static_cast<double>(duration_seq) / duration_par << "x\n";
        std::cout << "Found " << ullman_par.get_matchings().size() << " isomorphisms\n\n";
    }

    {
        std::cout << "Test 3: Complete Graphs (K8 vs K4)\n";
        auto G = create_complete_graph(8);
        auto H = create_complete_graph(4);

        std::cout << "G: " << G.n_vertices() << " vertices, " << G.n_edges() << " edges\n";
        std::cout << "H: " << H.n_vertices() << " vertices, " << H.n_edges() << " edges\n";

        omp_set_num_threads(1);
        auto start_seq = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_seq(G, H);
        auto end_seq = std::chrono::high_resolution_clock::now();
        auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();

        omp_set_num_threads(4);
        auto start_par = std::chrono::high_resolution_clock::now();
        UllmanMP<DirectedGraph<std::string>> ullman_par(G, H);
        auto end_par = std::chrono::high_resolution_clock::now();
        auto duration_par = std::chrono::duration_cast<std::chrono::milliseconds>(end_par - start_par).count();

        std::cout << "Sequential time: " << duration_seq << " ms\n";
        std::cout << "Parallel time (4 threads): " << duration_par << " ms\n";
        std::cout << "Speedup: " << static_cast<double>(duration_seq) / duration_par << "x\n";
        std::cout << "Found " << ullman_par.get_matchings().size() << " isomorphisms\n\n";
    }

    {
        std::cout << "Test 4: Thread Scalability Analysis (7x7 vs 3x3 grid)\n";
        auto G = create_grid_graph(7, 7);
        auto H = create_grid_graph(3, 3);

        std::cout << "G: " << G.n_vertices() << " vertices, " << G.n_edges() << " edges\n";
        std::cout << "H: " << H.n_vertices() << " vertices, " << H.n_edges() << " edges\n";

        for (int threads : {1, 2, 4, 8}) {
            omp_set_num_threads(threads);
            auto start = std::chrono::high_resolution_clock::now();
            UllmanMP<DirectedGraph<std::string>> ullman(G, H);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            std::cout << threads << " thread(s): " << duration << " ms, "
                      << ullman.get_matchings().size() << " isomorphisms\n";
        }
        std::cout << "\n";
    }
}

int main() {
    omp_set_dynamic(0);

    std::cout << "OpenMP max threads: " << omp_get_max_threads() << "\n";
    std::cout << "OpenMP available processors: " << omp_get_num_procs() << "\n\n";

    run_performance_test();

    return 0;
}
