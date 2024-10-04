CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -Iinclude

SRC_DIR = src
TOOLS_DIR = src/tools
BIN_DIR = .
BUILD_DIR = build

$(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/tools)

SOURCES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(TOOLS_DIR)/*.cpp)

OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp)) \
          $(patsubst $(TOOLS_DIR)/%.cpp, $(BUILD_DIR)/tools/%.o, $(wildcard $(TOOLS_DIR)/*.cpp))

TARGET = $(BIN_DIR)/build

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpcap

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/tools/%.o: $(TOOLS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
