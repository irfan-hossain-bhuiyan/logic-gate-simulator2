# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS =  -std=c++20 -Iinclude -O3 
# Source directory
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
# Find all .cpp files in the source directory
SOURCES= $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS =$(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
$(info objects are $(OBJECTS))
ifeq ($(EXAMPLE),)
	MAIN_CP=main.cpp
else
	MAIN_CP=$(EXAMPLE)
endif
MAIN_O=$(addprefix $(OBJ_DIR)/,$(notdir $(MAIN_CP:.cpp=.o)))
# Generate .o files from the source files
OBJECTS+=$(MAIN_O)
# Target executable name
TARGET = my_program

# Default target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS)  $^ -lraylib -opengl -o $@

# Compile .cpp files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(MAIN_O):$(MAIN_CP)
	$(CXX) $(CXXFLAGS) -c $< -o $@
# Clean up generated files
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
