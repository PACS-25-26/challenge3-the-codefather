CXX      = mpic++
CXXFLAGS = -std=c++17 -O2 -fopenmp -Iinclude

SRC = src/main.cpp src/solver.cpp src/jacobi.cpp src/io.cpp src/params.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)

TARGET = solver

all: $(TARGET)

$(TARGET): $(OBJ) | build output
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

output:
	mkdir -p output

clean:
	rm -rf build $(TARGET)

distclean: clean
	rm -rf output/

.PHONY: all clean
