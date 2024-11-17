CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

SRC_DIR = src
TOOLS_DIR = src/tools
BIN_DIR = .
BUILD_DIR = build

$(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/tools)

SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(TOOLS_DIR)/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp)) \
          $(patsubst $(TOOLS_DIR)/%.cpp, $(BUILD_DIR)/tools/%.o, $(wildcard $(TOOLS_DIR)/*.cpp))

TARGET = $(BIN_DIR)/isa-top

all: $(TARGET) clean_objects

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpcap -lncurses

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tools/%.o: $(TOOLS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean_objects:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/tools/*.o  # Clear object files

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
