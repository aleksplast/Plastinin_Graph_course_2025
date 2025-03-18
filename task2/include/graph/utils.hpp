#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace Graphs {

using Index = size_t;

struct Edge final {
    Index m_src;
    Index m_dest;

    Edge(Index src, Index dest) : m_src(src), m_dest(dest) {}
};

struct EdgeHash final {
    size_t operator()(const Edge& p) const {
        uintmax_t hash = std::hash<Index>{}(p.m_src);
        hash <<= sizeof(uintmax_t) * 4;
        hash ^= std::hash<Index>{}(p.m_dest);
        return std::hash<uintmax_t>{}(hash);
    }
};

using Distance = std::optional<uint64_t>;

using Predecessor = std::optional<Index>;

} // namespace Graph
