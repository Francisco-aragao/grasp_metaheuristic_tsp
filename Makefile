# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = object
BIN_DIR = bin

# Source files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
# Object files
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Name of the executable
TARGET = $(BIN_DIR)/tsp_constructive_heuristic

# Build target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR) # Ensure the bin directory exists
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) # Link all object files to create the executable

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR) # Ensure the obj directory exists
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to run the program
run: $(TARGET)
	@echo "Running: ./$(TARGET) $(ARGS)"
	./$(TARGET) $(ARGS)

# Clean the build
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all run clean