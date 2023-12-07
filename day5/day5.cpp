#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "advent_support/fileio.h"
#include "range_mapping.h"

std::vector<int64_t>
parseSeeds(std::string::const_iterator &substring_cbegin,
           const std::string::const_iterator substring_cend) {
  std::vector<int64_t> retSeeds;

  std::smatch matches;
  const std::regex seedHeaderRegex{"^seeds:"};
  const std::regex numberRegex{"^\\s(\\d+)"};

  // Consume the seed header
  if (std::regex_search(substring_cbegin, substring_cend, matches,
                        seedHeaderRegex)) {
    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  } else {
    throw std::runtime_error("Malformed input line");
  }

  // Consume the seed numbers
  while (std::regex_search(substring_cbegin, substring_cend, matches,
                           numberRegex)) {
    assert(matches.size() == 2 && "Unexpected number of matches");

    // Extract the matched grouping
    const int64_t number{std::stoll(matches[1])};
    retSeeds.push_back(number);

    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  }

  // Move semantics makes this OK to do
  return retSeeds;
}

RangeMapping parseMapping(std::string::const_iterator &substring_cbegin,
                          const std::string::const_iterator substring_cend) {
  RangeMapping retMapping;

  std::smatch matches;
  const std::regex mappingHeaderRegex{"^(?:\\n+)\\w+-to-\\w+ map:\\n"};
  const std::regex mappingRegex{"^(\\d+)\\s(\\d+)\\s(\\d+)\\n"};

  // Consume the mapping header
  if (std::regex_search(substring_cbegin, substring_cend, matches,
                        mappingHeaderRegex)) {
    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  } else {
    throw std::runtime_error("Malformed input line");
  }

  // Consume the mapping numbers
  while (std::regex_search(substring_cbegin, substring_cend, matches,
                           mappingRegex)) {
    assert(matches.size() == 4 && "Unexpected number of matches");

    // Extract the matched grouping
    const int64_t source{std::stoll(matches[2])};
    const int64_t destination{std::stoll(matches[1])};
    const int64_t length{std::stoll(matches[3])};

    retMapping.addRange(source, destination, length);

    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  }

  return retMapping;
}

void partA(const std::string &filename) {
  const std::string fileContents{readFileAsString(filename)};
  std::string::const_iterator substring_cbegin{fileContents.cbegin()};
  const std::string::const_iterator substring_cend{fileContents.cend()};

  // Parse the seeds
  const std::vector<int64_t> seeds{
      parseSeeds(substring_cbegin, substring_cend)};

  // Sanity check that there are seeds
  assert(!seeds.empty() && "No seeds found");

  std::vector<RangeMapping> mappings{};

  // Parse the mappings
  while (substring_cbegin != substring_cend) {
    mappings.push_back(parseMapping(substring_cbegin, substring_cend));
  }

  std::unique_ptr<int64_t[]> locations{new int64_t[seeds.size()]};

  // Pass each seed throw each mapping
  for (int32_t i{0}; i < static_cast<int32_t>(seeds.size()); i++) {
    int64_t value{seeds[i]};

    for (const RangeMapping &mapping : mappings) {
      value = mapping.mapValue(value);
    }

    // Save the `value` to the `locations` array
    locations[i] = value;
  }

  int64_t *minLocation{
      std::min_element(locations.get(), locations.get() + seeds.size())};

  std::cout << "The minimum location is: " << *minLocation << std::endl;

  return;
}

int32_t main(int32_t argc, char *argv[]) {
  const std::string filename{argc == 2 ? argv[1] : "input_small.txt"};

  try {
    partA(filename);
    // partB(filename);
  } catch (const std::exception &exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
