#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace Graphs {

// Index of vertex in graph
using Index = size_t;

// Edge of the graph
struct Edge final {
    Index m_src;
    Index m_dest;

    Edge(Index src, Index dest) : m_src(src), m_dest(dest) {}

    bool operator==(const Edge &other) const;
};

// Hash for the edge
struct EdgeHash final {
    size_t operator()(const Edge& p) const {
        uintmax_t hash = std::hash<Index>{}(p.m_src);
        hash <<= sizeof(uintmax_t) * 4;
        hash ^= std::hash<Index>{}(p.m_dest);
        return std::hash<uintmax_t>{}(hash);
    }
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

} // namespace Graph
