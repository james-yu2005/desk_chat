#ifndef FOCUS_PRESENCE_H
#define FOCUS_PRESENCE_H

#include <cstdint>
#include <vector>

namespace focus_presence {

constexpr int kGridCols = 16;
constexpr int kGridRows = 12;

// Mean absolute difference between two luma grids (0 = identical, 255 = max).
float CompareGrids(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

}  // namespace focus_presence

#endif  // FOCUS_PRESENCE_H
