#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> readFileAsLines(const std::string &filename) {
  std::ifstream file{filename};

  // Check if the file was opened correctly
  if (!file.is_open()) {
    throw std::runtime_error("Error opening file: " + filename);
  }

  std::vector<std::string> lines;

  // Read each line from the string
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }

  // Clean up
  file.close();

  // Move semantics makes this an OK thing to do
  return lines;
}
