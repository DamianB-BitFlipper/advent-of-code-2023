CXX = g++-8
CXXFLAGS = -I.. -Wall -Wextra -Wstrict-aliasing -std=c++2a -Weffc++

SRC_FILES = $(wildcard *.cpp)
SRC_FILES_SUPPORT = $(wildcard ../advent_support/*.cpp)

OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_FILES_SUPPORT = $(patsubst %.cpp, ./%.o, $(notdir $(SRC_FILES_SUPPORT)))

TARGET = solution

$(TARGET): $(OBJ_FILES) $(OBJ_FILES_SUPPORT)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: ../advent_support/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_FILES) $(OBJ_FILES_SUPPORT) $(TARGET)
