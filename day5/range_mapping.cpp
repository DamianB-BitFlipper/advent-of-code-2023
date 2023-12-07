#include <algorithm>
#include <cassert>
#include <cstdint>
#include <utility>
#include <vector>

#include "range_mapping.h"

int64_t RangeMapping::mapValue(const int64_t value) const {
  // First look if `value` is within any of the ranges
  for (const auto &[source, destination, length] : mRanges) {
    // If `value` is within [source, source + length)
    if (value >= source && value < source + length) {
      return destination + (value - source);
    }
  }

  // If not, then `value` is maps to itself
  return value;
}

std::vector<std::pair<int64_t, int64_t>>
RangeMapping::mapRange(const std::pair<int64_t, int64_t> &range) const {
  auto [rangeStart, rangeLength]{range};

  std::vector<std::pair<int64_t, int64_t>> retRanges{};

  // First look if `range` is within any of the mapping ranges
  for (const auto &[source, destination, mappingLength] : mRanges) {
    // Case 1: `range` is completely inside the mapping range
    if (rangeStart >= source &&
        rangeStart + rangeLength <= source + mappingLength) {
      retRanges.emplace_back(destination + (rangeStart - source), rangeLength);

      // Consume the entire `rangeLength`
      rangeStart += rangeLength;
      rangeLength = 0;

      // Case 2: `range` begins before the mapping range, but ends inside
    } else if (rangeStart < source && rangeStart + rangeLength > source &&
               rangeStart + rangeLength <= source + mappingLength) {
      const int64_t lenBefore{source - rangeStart};

      // The portion before the mapping range maps to itself
      retRanges.emplace_back(rangeStart, lenBefore);

      // The portion inside the mapping range maps to the destination
      retRanges.emplace_back(destination, rangeLength - lenBefore);

      // Consume the entire `rangeLength`
      rangeStart += rangeLength;
      rangeLength = 0;

      // Case 3: `range` begins inside the mapping range, but ends after
    } else if (rangeStart >= source && rangeStart < source + mappingLength &&
               rangeStart + rangeLength > source + mappingLength) {
      const int64_t lenInside{mappingLength - (rangeStart - source)};
      retRanges.emplace_back(destination + (rangeStart - source), lenInside);

      // Move the `rangeStart` to the end of the mapping range
      // since it was just consumed
      rangeStart += lenInside;
      rangeLength -= lenInside;

      // Sanity check
      assert(rangeLength >= 0 && "Unexpected range length");

      // Case 4: `range` begins before the mapping range, and ends after
    } else if (rangeStart < source &&
               rangeStart + rangeLength > source + mappingLength) {
      const int64_t lenBefore{source - rangeStart};
      const int64_t lenInside{mappingLength};

      // The portion before the mapping range maps to itself
      retRanges.emplace_back(rangeStart, lenBefore);

      // The portion inside the mapping range maps to the destination
      retRanges.emplace_back(destination, lenInside);

      // Move the `rangeStart` to the end of the mapping range
      // since it was just consumed
      rangeStart += lenBefore + lenInside;
      rangeLength -= lenBefore + lenInside;

      // Sanity check
      assert(rangeLength >= 0 && "Unexpected range length");
    }
  }

  // If there is any `rangeLength` left, then it maps to itself
  if (rangeLength > 0) {
    retRanges.emplace_back(rangeStart, rangeLength);
  }

  return retRanges;
}
