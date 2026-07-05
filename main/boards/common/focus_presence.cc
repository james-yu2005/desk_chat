#include "focus_presence.h"

#include <algorithm>
#include <cmath>

namespace focus_presence {

float CompareGrids(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    if (a.empty() || a.size() != b.size()) {
        return 255.0f;
    }

    uint64_t total = 0;
    for (size_t i = 0; i < a.size(); i++) {
        total += static_cast<uint32_t>(std::abs(static_cast<int>(a[i]) - static_cast<int>(b[i])));
    }
    return static_cast<float>(total) / static_cast<float>(a.size());
}

}  // namespace focus_presence
