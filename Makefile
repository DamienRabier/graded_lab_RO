# Makefile for TAP Solver project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
INCLUDES = -Isrc -Isrc/loaders -Isrc/solver

# Directories
SRC_DIR = src
BIN_DIR = bin

# Files and structure
TARGET = tap_solver

# Source files
SRCS = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/loaders/TapInstance.cpp \
	$(SRC_DIR)/loaders/InstanceLoader.cpp \
	$(SRC_DIR)/loaders/EvaluationLoader.cpp \
	$(SRC_DIR)/solver/GreedySolver.cpp \

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(BIN_DIR)/$(TARGET)

# Build executable
$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run program
run: all
	./$(BIN_DIR)/$(TARGET)

# Clean build artifacts
clean:
	find $(SRC_DIR) -name "*.o" -type f -delete
	rm -f $(BIN_DIR)/$(TARGET)
