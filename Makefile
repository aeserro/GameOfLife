# Compiler
GPP = g++

# Compilation flags
GPPFlags = -Wall -Wextra -Werror -Wfatal-errors -std=c++20 -g

# Source and header files
SRC = HW1.cpp GameOfLife.cpp GameOfLifeDecay.cpp GameOfLifeWrap.cpp
HEADER = GameOfLife.h

# Object files (same as SRC but with .o)
OBJ = $(SRC:.cpp=.o)

# Output binary name
TARGET = GOLApp

# Your name and HW number for packaging
TARNAME = Alejandro_Serrano
HWNUM = HW6

# Default rule: build the program
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJ)
	$(GPP) $(GPPFlags) $(OBJ) -o $(TARGET)

# Compile each .cpp into .o
%.o: %.cpp $(HEADER)
	$(GPP) $(GPPFlags) -c $< -o $@

# Run with custom flags
# Example usage:
#    make run ARGS="-f glider -w -g 20 -p 4"
run: $(TARGET)
	@if [ -z "$(ARGS)" ]; then \
		echo "Usage: make run ARGS=\"-f <file> [-g <gens>] [-p <delay>] [-s|-w]\""; \
	else \
		./$(TARGET) $(ARGS); \
	fi

# Clean up compiled files
clean:
	rm -f $(TARGET) $(OBJ)
	@echo "Removed executable and object files."

# Package all files for submission
package:
	tar -cvf $(HWNUM)_$(TARNAME).tar Makefile $(SRC) $(HEADER)
