#pragma once

#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

class RangeMapping {
public:
  RangeMapping() = default;

  void addRange(const int64_t start, const int64_t end, const int64_t length) {
    mRanges.emplace_back(start, end, length);
  }

  int64_t mapValue(const int64_t value) const;

  std::vector<std::pair<int64_t, int64_t>>
  mapRange(const std::pair<int64_t, int64_t> &range) const;

private:
  // The tuple holds (source, destination, length)
  std::vector<std::tuple<int64_t, int64_t, int64_t>> mRanges{};
};
