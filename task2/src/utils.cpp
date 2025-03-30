#include "graph/utils.hpp"

namespace Graphs {

bool Edge::operator==(const Edge &other) const {
    return m_src == other.m_src && m_dest == other.m_dest;
}

Weight Weight::operator+(const Weight &other) const {
    if (is_inf() || other.is_inf()) {
        return Weight();
    }

    return Weight(m_val.value() + other.m_val.value());
}

Weight Weight::operator-(const Weight &other) const {
    if (is_inf() || other.is_inf()) {
        return Weight();
    }

    return Weight(m_val.value() - other.m_val.value());
}

bool Weight::operator<(const Weight &other) const {
    if (is_inf()) {
        return false;
    } else if (other.is_inf()) {
        return true;
    }

    return m_val.value() < other.m_val.value();
}

bool Weight::operator>(const Weight &other) const {
    return other < *this;
}

bool Weight::operator==(const Weight &other) const {
    return m_val == other.m_val;
}

bool Weight::operator!=(const Weight &other) const {
    return !(*this == other);
}

} // namespace Graphs
