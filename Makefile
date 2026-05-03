# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -funroll-loops -Wall -Wextra -pedantic
CPPFLAGS = -Iinclude

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source and Object Files
SOURCES = $(SRC_DIR)/transforms/polar.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executables / Tests
TEST_QJL = $(BUILD_DIR)/test_qjl
TEST_INTEGRATION = $(BUILD_DIR)/test_turboquant

.PHONY: all clean test dirs

all: dirs $(TEST_QJL) $(TEST_INTEGRATION)

# Create build directory structure
dirs:
	@mkdir -p $(BUILD_DIR)/transforms

# Rule for building core objects
$(BUILD_DIR)/transforms/%.o: $(SRC_DIR)/transforms/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Build QJL Unit Tests
$(TEST_QJL): $(TEST_DIR)/test_qjl.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

# Build Integration Tests
$(TEST_INTEGRATION): $(TEST_DIR)/test_turboquant.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

# Run the test suite
test: all
	@echo "--- Running QJL Tests ---"
	./$(TEST_QJL)
	@echo "--- Running Integration Tests ---"
	./$(TEST_INTEGRATION)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)