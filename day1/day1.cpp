#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cctype>
#include <map>

#include "advent_support/fileio.h"

template <typename T>
char firstNumber(T it, const T end) {
  for (; it != end; it++) {
    char c{ *it };
    if (isdigit(c)) {
      return c;
    }
  }

  throw std::runtime_error("No first digit found!");
}

void partA(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };

  int32_t calibration_value{ 0 };
  for (const auto& line : lines) {
    const char firstChar{ firstNumber(line.cbegin(), line.cend()) };
    const char lastChar{ firstNumber(line.crbegin(), line.crend()) };
    
    const std::string number{ firstChar, lastChar };
    
    calibration_value += std::stoi(number);
  }

  std::cout << "Part A: The calibration value is: " << calibration_value << std::endl;
}

template <typename T>
bool lookFor(T it, const T end, std::string_view target) {
  for (std::string_view::iterator target_it = target.begin(); 
       target_it != target.end();
       target_it++, it++) {
    // We ran into the end of `it` before exhausting `target`, must be `false`
    if (it == end) {
      return false;
    } else if (*it != *target_it) {
      // The strings differ
      return false;
    }
  }

  // Everything matches up
  return true;
}

template <typename T>
int32_t firstNumberWithWords(T it, const T end, bool reverse) {
  const std::map<std::string_view, int32_t> digitMap{
    {!reverse ? "one" : "eno", 1},
    {!reverse ? "two" : "owt", 2},
    {!reverse ? "three" : "eerht", 3},
    {!reverse ? "four" : "ruof", 4},
    {!reverse ? "five" : "evif", 5},
    {!reverse ? "six" : "xis", 6},
    {!reverse ? "seven" : "neves", 7},
    {!reverse ? "eight" : "thgie", 8},
    {!reverse ? "nine" : "enin", 9},
  };

  for (; it != end; it++) {
    // First check for a simple digit
    if (isdigit(*it)) {
      // Convert the char to its int representation
      return *it - '0';
    }

    // Try each of the digits in `digitMap`
    for (const auto& pair : digitMap) {
      if (lookFor(it, end, pair.first)) {
        return pair.second;
      }
    }
  }

  throw std::runtime_error("No first digit found!");
}

void partB(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };

  int32_t calibration_value{ 0 };
  for (const auto& line : lines) {
    const int32_t firstDigit{ firstNumberWithWords(line.cbegin(), line.cend(), false) };
    const int32_t lastDigit{ firstNumberWithWords(line.crbegin(), line.crend(), true) };
    calibration_value += firstDigit * 10 + lastDigit;
  }

  std::cout << "Part B: The calibration value is: " << calibration_value << std::endl;
}

int32_t main(int argc, char* argv[]) {
  // Check that the filename is provided
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  try {
    partA(argv[1]);
    partB(argv[1]);
  } catch(const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
