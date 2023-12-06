CXX = clang++
CXXFLAGS = -I.. -Wall -Wextra -Wstrict-aliasing -std=c++2a -Weffc++ -g

SRC_FILES = $(wildcard *.cpp)
SRC_FILES_SUPPORT = $(wildcard ../advent_support/*.cpp)

OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_FILES_SUPPORT = $(SRC_FILES_SUPPORT:.cpp=.o)

# Target binary name is the same as the parent directory
TARGET = $(notdir $(patsubst %/,%,$(CURDIR)))

$(TARGET): $(OBJ_FILES) $(OBJ_FILES_SUPPORT)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(OBJ_FILES_SUPPORT) $(TARGET)
