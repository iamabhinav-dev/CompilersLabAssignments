# Makefile for Bison, Flex, and C project

# Compiler
CC = gcc

# Flex and Bison tools
FLEX = flex
BISON = bison

# Flags
CFLAGS = -Wall -g
LDFLAGS = -lfl -lm  # Link Flex and math library

# Output executable names
ICGEN_EXEC = icgen    # For the first executable (icgen)
IC_EXEC = ic          # For the second executable (ic)

# Source files
LEX_FILE = expr.l
BISON_FILE = expr.y
UTILS_FILE = intcodegen.c

# Generated files
LEX_OUTPUT = lex.yy.c
BISON_OUTPUT = expr.tab.c
BISON_HEADER = expr.tab.h

# Input and output files
INPUT_FILE = sample.txt
INTERMEDIATE_OUTPUT = intcode.c
OUTPUT_FILE = output.txt

# Default target: Build everything
all: $(ICGEN_EXEC)

# Rule to build icgen from yacc, flex, and intcodegen.c
$(ICGEN_EXEC): $(LEX_OUTPUT) $(BISON_OUTPUT) $(UTILS_FILE)
	$(CC) $(CFLAGS) -o $(ICGEN_EXEC) $(LEX_OUTPUT) $(BISON_OUTPUT) $(UTILS_FILE) $(LDFLAGS)

# Generate lex.yy.c from expr.l
$(LEX_OUTPUT): $(LEX_FILE)
	$(FLEX) $(LEX_FILE)

# Generate expr.tab.c and expr.tab.h from expr.y
$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_FILE)
	$(BISON) -d $(BISON_FILE)

# Dependency for intcodegen.c to include Bison header
$(UTILS_FILE): $(BISON_HEADER)

# Rule to run icgen, which generates intcode.c, and then compile and run it
run: $(ICGEN_EXEC)
	./$(ICGEN_EXEC) < $(INPUT_FILE) > $(INTERMEDIATE_OUTPUT)
	$(CC) $(INTERMEDIATE_OUTPUT) -o $(IC_EXEC) $(LDFLAGS)
	./$(IC_EXEC)>$(OUTPUT_FILE)

# Clean rule to remove generated and compiled files
clean:
	rm -f $(ICGEN_EXEC) $(IC_EXEC) $(LEX_OUTPUT) $(BISON_OUTPUT) $(BISON_HEADER) $(INTERMEDIATE_OUTPUT) $(OUTPUT_FILE)

.PHONY: all run clean
