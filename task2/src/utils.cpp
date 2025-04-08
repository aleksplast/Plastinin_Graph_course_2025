#include "graph/utils.hpp"

namespace Graphs {

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

std::ostream &operator<<(std::ostream &os, const Weight &weight) {
    if (weight.is_inf()) {
        os << "inf";
    } else {
        os << weight.m_val.value();
    }

    return os;
}

} // namespace Graphs
