
# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

# Source directory
SRC_DIR = src
OBJ_DIR = obj

# Find all .cpp files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Generate .o files from the source files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target executable name
TARGET = my_program

# Default target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lraylib -opengl -v

# Compile .cpp files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
