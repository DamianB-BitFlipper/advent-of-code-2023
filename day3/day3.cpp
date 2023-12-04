#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <cctype>
#include <charconv>
#include <system_error>
#include <cassert>

#include "advent_support/fileio.h"

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
        auto [ptr, ec] = std::from_chars(row.data() + j, row.data() + width, parsedResult);

        // Indicates a success
        if (ec != std::errc()) {
          std::cerr << "Failed to parse " << row << " starting from position " << j << std::endl;
          std::cerr << "Input: " << row.substr(j) << std::endl;
          assert(false);
        }

        // Calculate how many chars were parsed based off of how offset `ptr` is
        int32_t nDigits{ static_cast<int32_t>(ptr - (row.data() + j)) };

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

int32_t main(int argc, char* argv[]) {
  // Check that the filenames are provided
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <filename A> <filename B>" << std::endl;
    return 1;
  }

  try {
    partA(argv[1]);
    // partB(argv[2]);
  } catch(const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
