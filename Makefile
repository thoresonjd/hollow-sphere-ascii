# Makefile
# Author: Justin Thoreson
# Usage:
# - `make [hollow-sphere-ascii]`: Builds the hollow sphere ASCII program
# - `make run`: Builds and executes the hollow sphere ASCII program

CXX = g++
CXX_FLAGS = -std=c++23 -Wall -Werror -pedantic -ggdb -O0
PROGRAM = hollow-sphere-ascii

$(PROGRAM) : $(PROGRAM).cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

run : $(PROGRAM)
	./$(PROGRAM)

.PHONY : clean

clean:
	rm -f $(PROGRAM)

