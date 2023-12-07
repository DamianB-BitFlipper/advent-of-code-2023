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
