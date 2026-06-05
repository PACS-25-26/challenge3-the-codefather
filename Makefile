CXX      = mpic++
CXXFLAGS = -std=c++23 -O2 -fopenmp -Iinclude

SRC = src/main.cpp src/solver.cpp src/jacobi.cpp src/io.cpp src/params.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)

TARGET = solver

all: $(TARGET) 

# Link object files into the final executable
$(TARGET): $(OBJ) | build output
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule to compile source files into object files
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
build:
	mkdir -p build

# Create output directory if it doesn't exist
output:
	mkdir -p output

# ---------------------------------------------------------
# Documentation Targets (Doxygen)
# ---------------------------------------------------------

# Generate documentation
docs:
	doxygen Doxyfile

# Clean up generated documentation
clean_docs:
	rm -rf docs/

# ---------------------------------------------------------
# Cleanup Targets
# ---------------------------------------------------------

# Clean: remove build artifacts
clean:
	rm -rf build $(TARGET)

# Deep clean: remove build artifacts, output, and docs
distclean: clean clean_docs
	rm -rf output/ test/data test/hw.info

.PHONY: all clean distclean docs clean_docs
