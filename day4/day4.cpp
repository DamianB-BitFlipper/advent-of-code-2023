#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "advent_support/fileio.h"

void parseCard(const std::string &cardLine,
               std::set<int32_t> &retWinningNumbers,
               std::set<int32_t> &retTrialNumbers) {
  std::string::const_iterator substring_cbegin{cardLine.cbegin()};
  const std::string::const_iterator substring_cend{cardLine.cend()};

  std::smatch matches;
  const std::regex cardHeaderRegex{"Card\\s+\\d+:"};
  const std::regex numberRegex{"^\\s{1,2}(\\d+)"};
  const std::regex dividerRegex{"^ \\|"};

  // Consume the card header
  if (std::regex_search(substring_cbegin, substring_cend, matches,
                        cardHeaderRegex)) {
    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  } else {
    throw std::runtime_error("Malformed input line: " + cardLine);
  }

  // Consume the winning numbers
  while (std::regex_search(substring_cbegin, substring_cend, matches,
                           numberRegex)) {
    assert(matches.size() == 2 && "Unexpected number of matches");

    // Extract the matched grouping
    const int32_t number{std::stoi(matches[1])};
    retWinningNumbers.insert(number);

    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  }

  // Consume the divider
  if (std::regex_search(substring_cbegin, substring_cend, matches,
                        dividerRegex)) {
    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  } else {
    throw std::runtime_error("Malformed input line: " + cardLine);
  }

  // Consume the trial numbers
  while (std::regex_search(substring_cbegin, substring_cend, matches,
                           numberRegex)) {
    assert(matches.size() == 2 && "Unexpected number of matches");

    // Extract the matched grouping
    const int32_t number{std::stoi(matches[1])};
    retTrialNumbers.insert(number);

    // Advance the iterator
    substring_cbegin = matches.suffix().first;
  }

  // Assert that the entire string was consumed
  assert(substring_cbegin == substring_cend &&
         "Failed to consume entire string");
}

void partA(const std::string &filename) {
  std::vector<std::string> lines{readFileAsLines(filename)};

  int32_t cumulativeScore{0};
  for (const std::string &line : lines) {
    std::set<int32_t> winningNumbers;
    std::set<int32_t> trialNumbers;

    // Parse the card
    parseCard(line, winningNumbers, trialNumbers);

    // Find the intersection of `winningNumbers` and `trialNumbers`
    std::set<int32_t> intersection;
    std::set_intersection(winningNumbers.cbegin(), winningNumbers.cend(),
                          trialNumbers.cbegin(), trialNumbers.cend(),
                          std::inserter(intersection, intersection.begin()));

    // Calculate the score
    if (intersection.size() > 0) {
      int32_t score{0b1 << (intersection.size() - 1)};
      cumulativeScore += score;
    }
  }

  std::cout << "Part A: The cumulative score is: " << cumulativeScore
            << std::endl;

  return;
}

void partB(const std::string &filename) {
  std::vector<std::string> lines{readFileAsLines(filename)};
  std::unique_ptr<int32_t[]> nCardCopies{new int32_t[lines.size()]};

  // Initialize the number of copies of each card to 1
  std::fill(nCardCopies.get(), nCardCopies.get() + lines.size(), 1);

  int32_t cardId{0};
  for (const std::string &line : lines) {
    std::set<int32_t> winningNumbers;
    std::set<int32_t> trialNumbers;

    // Parse the card
    parseCard(line, winningNumbers, trialNumbers);

    // Find the intersection of `winningNumbers` and `trialNumbers`
    std::set<int32_t> intersection;
    std::set_intersection(winningNumbers.cbegin(), winningNumbers.cend(),
                          trialNumbers.cbegin(), trialNumbers.cend(),
                          std::inserter(intersection, intersection.begin()));

    const int32_t nCopies{nCardCopies[cardId]};
    const int32_t nIntersection{static_cast<int32_t>(intersection.size())};

    // Add `nCopies` to `nCardCopies` for each card
    // from [cardId + 1, cardId + nIntersection]
    for (int32_t i{cardId + 1}; i <= cardId + nIntersection; ++i) {
      // Prevent an overflow
      if (cardId + nIntersection >= static_cast<int32_t>(lines.size())) {
        break;
      }

      nCardCopies[i] += nCopies;
    }

    // Increment the card ID
    cardId++;
  }

  const int32_t cumulativeCopies{
      std::accumulate(nCardCopies.get(), nCardCopies.get() + lines.size(), 0)};

  std::cout << "Part B: The cumulative number of copies is: "
            << cumulativeCopies << std::endl;

  return;
}

int32_t main(int argc, char *argv[]) {
  // Check that the filename is provided
  // if (argc != 2) {
  //   std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
  //   return 1;
  // }
  const std::string filename{argc == 2 ? argv[1] : "input_small.txt"};

  try {
    partA(filename);
    partB(filename);
  } catch (const std::exception &exception) {
    std::cerr << exception.what() << std::endl;
    return 1;
  }

  return 0;
}
