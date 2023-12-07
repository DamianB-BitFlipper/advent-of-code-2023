#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

class RangeMapping {
public:
  RangeMapping() = default;

  void addRange(const int64_t start, const int64_t end, const int64_t value) {
    mRanges.emplace_back(start, end, value);
  }

  int64_t mapValue(const int64_t value) const;

private:
  // The tuple holds (source, destination, length)
  std::vector<std::tuple<int64_t, int64_t, int64_t>> mRanges{};
};
