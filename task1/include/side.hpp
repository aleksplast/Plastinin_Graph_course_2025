#pragma once

namespace Task1 {

// On which side node is located relative to its parent
enum class Side { Left, Right , None};

Side opposite(Side side);
}
