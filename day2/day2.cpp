#include <iostream>
#include <cstdint>
#include <regex>
#include <stdexcept>
#include <cassert>
#include <cctype>
#include <map>

#include "advent_support/fileio.h"

std::string parseGamesLine(const std::string& line) {
  // Remove the `Game` portion of each line
  const std::regex lineRegex{ "Game \\d+:(.*)" };

  std::smatch matches;
  if (std::regex_match(line, matches, lineRegex)) {
    assert(matches.size() == 2 && "Unexpected number of matches");
    
    // Move semantics makes this OK to do
    return matches[1];
  }

  throw std::runtime_error("Malformed input line: " + line);
}

std::string parseSingleGame(const std::string& gamesLine, 
                            std::map<const std::string, int32_t>& retMarblesMap) {
  /** 
   *  Parse a single game from `gamesLine`. Returns the 
   *  unconsumed portion of the string for further processing.
   **/

  // The *? is a lazy matching, attempting to match as few occurrences as possible
  const std::regex gameRegex{ "\\s*(.*?)(?:;|$)" };

  std::smatch matches;
  if (std::regex_search(gamesLine.cbegin(), gamesLine.cend(), matches, gameRegex)) {
    assert(matches.size() == 2 && "Unexpected number of matches");

    const std::string game{ matches[1] };

    const std::regex marbleRegex{ "(\\d+)\\s(\\w+)(?:, )?" };
    std::smatch marbleMatches;

    std::string::const_iterator substring_cbegin{ game.cbegin() };
    const std::string::const_iterator substring_cend{ game.cend() };

    // Iterate each of the marble counts in `game`
    while (std::regex_search(substring_cbegin, substring_cend, marbleMatches, marbleRegex)) {
      assert(marbleMatches.size() == 3 && "Unexpected number of matches");

      // Extract the matched grouping
      const int32_t count{ std::stoi(marbleMatches[1]) };
      const std::string marbleType{ marbleMatches[2] };

      // Check if `marbleType` is already in `retMarblesMap`
      auto marbleIt{ retMarblesMap.find(marbleType) };
      
      // The `marbleType` does not exist, simply insert
      if (marbleIt == retMarblesMap.end()) {
        retMarblesMap.insert(std::make_pair(marbleType, count));
      } else {
        // The `marbleType` does exist, insert the max of the respective `count` value
        marbleIt->second = std::max(marbleIt->second, count);
      }

      // Move the search substring forward
      substring_cbegin = marbleMatches.suffix().first;
    }
    
    // Return the unconsumed portion of the `gamesLine`. Move semantics makes this OK to do
    return matches.suffix();
  }

  throw std::runtime_error("Malformed games line: " + gamesLine);
}

bool isGamePossible(std::map<const std::string, int32_t>& marblesMap) {
  auto redIt{ marblesMap.find("red") };
  auto greenIt{ marblesMap.find("green") };
  auto blueIt{ marblesMap.find("blue") };

  int32_t nRed{ redIt != marblesMap.end() ? redIt->second : 0 };
  int32_t nGreen{ greenIt != marblesMap.end() ? greenIt->second : 0 };
  int32_t nBlue{ blueIt != marblesMap.end() ? blueIt->second : 0 };

  return (nRed <= 12) && (nGreen <= 13) && (nBlue <= 14);
}

void partA(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };

  int32_t possibleGamesSum{ 0 };
  int32_t roundId{ 1 };
  for (auto& line : lines) {
    std::map<const std::string, int32_t> marblesMap{};

    // Consume `games` until it is empty, populating the `marblesMap` on each iteration
    std::string games{ parseGamesLine(line) };
    while (!games.empty()) {
      games = parseSingleGame(games, marblesMap);
    }

    if (isGamePossible(marblesMap)) {
      possibleGamesSum += roundId;
    }

    // Move on to the next round
    roundId++;
  }

  std::cout << "Part A: The possible games sum is: " << possibleGamesSum << std::endl;
}

void partB(const std::string& filename) {
  std::vector<std::string> lines{ readFileAsLines(filename) };

  int32_t powersSum{ 0 };
  for (auto& line : lines) {
    std::map<const std::string, int32_t> marblesMap{
      {"red", 0},
      {"green", 0},
      {"blue", 0},
    };

    // Consume `games` until it is empty, populating the `marblesMap` on each iteration
    std::string games{ parseGamesLine(line) };
    while (!games.empty()) {
      games = parseSingleGame(games, marblesMap);
    }

    powersSum += marblesMap.at("red") * marblesMap.at("green") * marblesMap.at("blue");
  }

  std::cout << "Part B: The powers sum is: " << powersSum << std::endl;
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
