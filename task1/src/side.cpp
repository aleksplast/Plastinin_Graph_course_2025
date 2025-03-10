#include "side.hpp"

namespace Task1 {

Side opposite(Side side) {
    if (side == Side::None) {
        return side;
    }

    return side == Side::Left ? Side::Right : Side::Left;
}

}
