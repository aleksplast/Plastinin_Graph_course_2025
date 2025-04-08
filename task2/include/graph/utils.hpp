#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>

namespace Graphs {

// Index of vertex in graph
using Index = size_t;

// Edge of the graph
struct Edge final {
    Index m_src;
    Index m_dest;

    Edge(Index src, Index dest) : m_src(src), m_dest(dest) {}

    bool operator==(const Edge &other) const = default;
};

// Predecessor of the vertex, used in some algorithms
using Predecessor = std::optional<Index>;

// Weight of the edge
struct Weight final {
    std::optional<int> m_val;

    Weight(int val) : m_val(val) {}
    Weight() = default;

    bool is_inf() const { return !m_val.has_value(); }

    Weight operator+(const Weight &other) const;
    Weight operator-(const Weight &other) const;
    bool operator<(const Weight &other) const;
    bool operator>(const Weight &other) const;
    bool operator==(const Weight &other) const;
    bool operator!=(const Weight &other) const;
};

std::ostream &operator<<(std::ostream &os, const Weight &weight);

} // namespace Graph

namespace std {
    // Hash for the edge
    template<>
    struct hash<Graphs::Edge> {
        size_t operator()(const Graphs::Edge& p) const {
            uintmax_t hash = std::hash<Graphs::Index>{}(p.m_src);
            hash <<= sizeof(uintmax_t) * 4;
            hash ^= std::hash<Graphs::Index>{}(p.m_dest);
            return std::hash<uintmax_t>{}(hash);
        }
    };
}
