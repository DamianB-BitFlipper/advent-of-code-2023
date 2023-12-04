#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <cctype>
#include <charconv>
#include <system_error>
#include <cassert>
#include <utility>

#include "advent_support/fileio.h"

int32_t parseNumber(const std::string& row, int32_t j, int32_t& retParsedValue) {
  const int32_t width{ static_cast<int32_t>(row.length()) };

  // Parse the first number in `row` from `j` to the end
  auto [ptr, ec] = std::from_chars(row.data() + j, row.data() + width, retParsedValue);
  
  // Indicates a success
  if (ec != std::errc()) {
    std::cerr << "Failed to parse " << row << " starting from position " << j << std::endl;
    std::cerr << "Input: " << row.substr(j) << std::endl;
    assert(false);
  }

  // Calculate how many chars were parsed based off of how offset `ptr` is
  int32_t nDigits{ static_cast<int32_t>(ptr - (row.data() + j)) };

  return nDigits;
}

void partA(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };
  const int32_t height{ static_cast<int32_t>(lines.size()) };
  const int32_t width{ static_cast<int32_t>(lines.at(0).length()) };

  int32_t schematicSum{ 0 };

  // Iterate through the schematic
  for (int32_t i{ 0 }; i < height; i++) {
    const std::string& row = lines[i];
    for (int32_t j{ 0 }; j < width;) {
      // This is the start of a number
      if (isdigit(row[j])) {
        int32_t parsedResult{};
        // Parse the first number in `row` from `j` to the end
        const int32_t nDigits{ parseNumber(row, j, parsedResult) };

        // Look at the chars around this number
        bool foundSymbol{ false };
        for (int32_t inspectI{ i - 1}; inspectI <= i + 1 && !foundSymbol; inspectI++) {
          for (int32_t inspectJ{ j - 1 }; inspectJ <= j + nDigits; inspectJ++) {
            // Check for in-bounds of (inspectI, inspectJ)
            if (inspectI < 0 || inspectJ < 0 || inspectI >= height || inspectJ >= width) {
              continue;
            }

            // Look up the value at (inspectI, inspectJ)
            const char spotValue = lines[inspectI][inspectJ];

            // If the `spotValue` is neither a digit nor a '.', it is a symbol which
            // neighbors the current number. Assign `foundSymbol` accordingly
            if (spotValue != '.' && !isdigit(spotValue)) {
              foundSymbol = true;
              break;
            }
          }
        }

        // Add the `parsedResult` to the accumulator `schematicSum` if a `foundSymbol`
        if (foundSymbol) {
          schematicSum += parsedResult;
        }

        // Skip `j` forward accordingly
        j += nDigits;
      } else {
        j++;
      }
    }
  }

  std::cout << "Part A: The schematic sum is: " << schematicSum << std::endl;
}

std::pair<int32_t, int32_t> parseNumberOnIJ(const std::vector<std::string>& lines,
                                            int32_t i, 
                                            int32_t j) {
  /**
   * Parse the number contained at point (i, j).
   * 
   * Returns a pair with the parsed number and how far this number extends
   * to the right of point (i, j).
   **/
  // Sanity check
  assert(isdigit(lines[i][j]));

  const std::string& row{ lines[i] };

  // Look to the left for the first non-digit or edge of the schema
  int32_t leftJ{ j };
  while (true) {
    int32_t peekJ{ leftJ - 1 };

    // We hit the edge
    if (peekJ < 0) {
      break;
    }

    const char peekC{ lines[i][peekJ] };

    // The value at `peekJ` is no longer a digit
    if (!isdigit(peekC)) {
      break;
    }

    // Move `leftJ` leftward by 1
    leftJ--;
  }

  int32_t parsedResult{};

  // Parse the first number in `row` from `leftJ` to the end
  const int32_t nDigits{ parseNumber(row, leftJ, parsedResult) };

  // Calculate how far this number extends to the right of point (i, j). Subtract 1 to
  // account for 1 indexing of `nDigits`
  const int32_t extends{ (leftJ + nDigits - 1) - j };

  return std::make_pair(parsedResult, extends);
}

std::pair<int32_t, bool> calculateGearRatio(const std::vector<std::string>& lines, 
                                            int32_t asteriskI, 
                                            int32_t asteriskJ) {
  /**
   * Calculate the gear ratio for an asterisk at `(askteriskI, asteriskJ)`.
   * 
   * Returns a pair of the calculated gear ratio and whether this gear ratio
   * is value (`true`) or not (`false`).
   **/
  // Sanity check
  assert(lines[asteriskI][asteriskJ] == '*');

  const int32_t height{ static_cast<int32_t>(lines.size()) };
  const int32_t width{ static_cast<int32_t>(lines.at(0).length()) };

  const int32_t EMPTY{ -1 };
  std::array<std::array<int32_t, 3>, 3> surroundingValues{{
      {EMPTY, EMPTY, EMPTY},
      {EMPTY, EMPTY, EMPTY},
      {EMPTY, EMPTY, EMPTY},
    }};

  // Offsets to make (-1, -1) the top-left point in `surroundingValues`
  const int32_t REALIGN_I{ 1 };
  const int32_t REALIGN_J{ 1 };

  // Iterate over the surroundingValues
  for (int32_t offsetI{ -1 }; offsetI <= 1; offsetI++) {
    for (int32_t offsetJ{ -1 }; offsetJ <= 1; offsetJ++) {
      const int32_t inspectI{ asteriskI + offsetI };
      const int32_t inspectJ{ asteriskJ + offsetJ };

      // Check for in-bounds of (inspectI, inspectJ)
      if (inspectI < 0 || inspectJ < 0 || inspectI >= height || inspectJ >= width) {
        continue;
      }

      // Look up the value at (inspectI, inspectJ)
      const char spotValue = lines[inspectI][inspectJ];

      // We found a number!
      if (isdigit(spotValue)) {
        auto [parsedValue, extends] = parseNumberOnIJ(lines, inspectI, inspectJ);

        // Assign the `parsedValue` to its respective spot in `surroundingValues`
        surroundingValues[offsetI + REALIGN_I][offsetJ + REALIGN_J] = parsedValue;

        // Fast-forward the `offsetJ` by `extends` since those values are
        // still a part of the number at `(inspectI, inspectJ)`
        offsetJ += extends;
      }
    }
  }

  // Scan through the `surroundingValues` array. It only counts as a gear ratio
  // if there are exactly two non-EMPTY values
  int32_t nGears{ 0 };
  int32_t gearRatio{ 1 };
  for (int32_t i{ 0 }; i < static_cast<int32_t>(surroundingValues.size()); i++) {
    for (int32_t j{ 0 }; j < static_cast<int32_t>(surroundingValues[0].size()); j++) {
      const int32_t value{ surroundingValues[i][j] };

      // Continue on `EMPTY`
      if (value == EMPTY) {
        continue;
      }

      // Multiply the `value` into the `gearRatio` and mark it as counted
      gearRatio *= value;
      nGears++;
    }
  }

  // Only return successfully if `nGears` is exactly 2
  if (nGears == 2){
    return std::make_pair(gearRatio, true);
  } else {
    return std::make_pair(-1, false);
  }
}

void partB(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };
  const int32_t height{ static_cast<int32_t>(lines.size()) };
  const int32_t width{ static_cast<int32_t>(lines.at(0).length()) };

  int32_t cumulativeGearRatios{ 0 };

  // Iterate through the schematic
  for (int32_t i{ 0 }; i < height; i++) {
    for (int32_t j{ 0 }; j < width; j++) {
      // Calculate the gear ratio only if `(i, j)` is an asterisk
      if (lines[i][j] == '*') {
        auto [gearRatio, isValid] = calculateGearRatio(lines, i, j);

        // Only consider this `gearRatio` if it is valid
        if (isValid) {
          cumulativeGearRatios += gearRatio;
        }
      }
    }
  }

  std::cout << "Part B: The cumulative gear ratios are: " << cumulativeGearRatios << std::endl;
}

int32_t main(int argc, char* argv[]) {
  // Check that the filenames are provided
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <filename A> <filename B>" << std::endl;
    return 1;
  }

  try {
    partA(argv[1]);
    partB(argv[2]);
  } catch(const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
